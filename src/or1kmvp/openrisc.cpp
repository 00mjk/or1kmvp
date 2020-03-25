/******************************************************************************
 *                                                                            *
 * Copyright 2018 Jan Henrik Weinstock                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 *                                                                            *
 ******************************************************************************/

#include "or1kmvp/openrisc.h"

namespace or1kmvp {

    bool openrisc::cmd_gdb(const std::vector<std::string>& args,
                           std::ostream& os) {
        if (!vcml::file_exists(gdb_term)) {
            os << "gdbterm not found at " << gdb_term.str() << std::endl;
            return false;
        }

        std::stringstream ss;
        ss << gdb_term.str()  << " " << name() << " localhost "
           << gdb_port.str() << " " << symbols.str();

        log_debug("gdbterm command line:");
        log_debug("'%s'", ss.str().c_str());

        int res = system(ss.str().c_str());
        if (res == -1) {
            log_error("failed to start gdb shell");
            os << "failed to start gdb shell";
            return false;
        }

        os << "started gdb shell on port " << gdb_port.str();
        return true;
    }

    bool openrisc::cmd_pic(const std::vector<std::string>& args,
                           std::ostream& os) {
        or1kiss::u32 picsr = m_iss->get_spr(or1kiss::SPR_PICSR, true);
        or1kiss::u32 picmr = m_iss->get_spr(or1kiss::SPR_PICMR, true);
        os << basename() << " interrupt controller status" << std::endl
           << "  PICSR: 0x" << std::hex << std::setw(8) << std::setfill('0')
           << picsr << std::endl
           << "  PICMR: 0x" << std::hex << std::setw(8) << std::setfill('0')
           << picmr << std::endl
           << std::endl;

        os << "  Masked: ";
        for (int i = 0; i < 32; i++) {
            if ((picmr & (1 << i)) == 0)
                os << std::dec << i << ", ";
        }

        if (picmr == 0)
            os << "none";
        os << std::endl;

        os << "  Pending: ";
        for (int i = 0; i < 32; i++) {
            if (picsr & (1 << i))
                os << std::dec << i << ", ";
        }

        if (picsr == 0)
            os << "none";
        // no newline needed

        return true;
    }

    bool openrisc::cmd_spr(const std::vector<std::string>& args,
                           std::ostream& os) {
        unsigned int grpid = atoi(args[0].c_str());
        if (grpid & ~0x1F) {
            os << "invalid group id: " << grpid;
            return false;
        }

        unsigned int regid = atoi(args[1].c_str());
        if (regid & ~0x7FFF) {
            os << "invalid group id: " << regid;
            return false;
        }

        unsigned int sprid = (grpid & 0x1F) << 11 | (regid & 0x7FF);
        if (args.size() > 2) {
            or1kiss::u32 val = vcml::from_string<or1kiss::u32>(args[2]);
            m_iss->set_spr(sprid, true);
            os << "SPR[" << grpid << "," << regid << "] = SPR[" << sprid
               << "] = 0x" << std::hex << std::setw(8) << std::setfill('0')
               << val << " written";
        } else {
            or1kiss::u32 val = m_iss->get_spr(sprid, true);
            os << "SPR[" << grpid << "," << regid << "] = SPR[" << sprid
               << "] = 0x" << std::hex << std::setw(8) << std::setfill('0')
               << val;
        }

        return true;
    }

    void openrisc::log_timing_info() const {
        double rt = get_run_time();
        vcml::u64 nc = cycle_count();

        log_info("clock speed   %.1f MHz", m_iss->get_clock() / 1e6);
        log_info("iss runtime   %.4f s", rt);
        log_info("iss speed     %.1f MIPS", rt == 0.0 ? 0.0 :
                 m_iss->get_num_instructions() / rt * 1e-6);
        log_info("instructions  %" PRId64, m_iss->get_num_instructions());
        log_info("cycles        %" PRId64, nc);
        log_info("sleep-cycles  %" PRId64 " (%.1f%%)",
                 m_iss->get_num_sleep_cycles(), nc == 0 ? 0.0 :
                 m_iss->get_num_sleep_cycles() * 100.0 / nc);
        log_info("jit hit-rate  %.4f",
                 m_iss->get_decode_cache_hit_rate());
        log_info("#lwa          %" PRId64, m_iss->get_num_lwa());
        log_info("#swa          %" PRId64, m_iss->get_num_swa());
        log_info("#swa failed   %" PRId64, m_iss->get_num_swa_failed());

        for (auto irq : IRQ) {
            vcml::irq_stats stats;
            if (!get_irq_stats(irq.first, stats) || stats.irq_count == 0)
                continue;

            std::string s;
            s += vcml::mkstr("irq %d status", stats.irq);
            s += vcml::mkstr("  %d#", stats.irq_count);
            s += vcml::mkstr(", avg %.1fus",
                    stats.irq_uptime.to_seconds() / stats.irq_count * 1e6);
            s += vcml::mkstr(", max %.1fus",
                    stats.irq_longest.to_seconds() * 1e6);
            log_info(s.c_str());
        }
    }

    using vcml::VCML_ACCESS_READ;
    using vcml::VCML_ACCESS_WRITE;
    using vcml::VCML_ACCESS_READ_WRITE;

#define SPR(x) (32 + or1kiss::SPR_##x)

    static const std::vector<vcml::cpureg> openrisc_cpuregs = {
        {  0,  0, "gpr/0",  4, VCML_ACCESS_READ_WRITE },
        {  1,  1, "gpr/1",  4, VCML_ACCESS_READ_WRITE },
        {  2,  2, "gpr/2",  4, VCML_ACCESS_READ_WRITE },
        {  3,  3, "gpr/3",  4, VCML_ACCESS_READ_WRITE },
        {  4,  4, "gpr/4",  4, VCML_ACCESS_READ_WRITE },
        {  5,  5, "gpr/5",  4, VCML_ACCESS_READ_WRITE },
        {  6,  6, "gpr/6",  4, VCML_ACCESS_READ_WRITE },
        {  7,  7, "gpr/7",  4, VCML_ACCESS_READ_WRITE },
        {  8,  8, "gpr/8",  4, VCML_ACCESS_READ_WRITE },
        {  9,  9, "gpr/9",  4, VCML_ACCESS_READ_WRITE },
        { 10, 10, "gpr/10", 4, VCML_ACCESS_READ_WRITE },
        { 11, 11, "gpr/11", 4, VCML_ACCESS_READ_WRITE },
        { 12, 12, "gpr/12", 4, VCML_ACCESS_READ_WRITE },
        { 13, 13, "gpr/13", 4, VCML_ACCESS_READ_WRITE },
        { 14, 14, "gpr/14", 4, VCML_ACCESS_READ_WRITE },
        { 15, 15, "gpr/15", 4, VCML_ACCESS_READ_WRITE },
        { 16, 16, "gpr/16", 4, VCML_ACCESS_READ_WRITE },
        { 17, 17, "gpr/17", 4, VCML_ACCESS_READ_WRITE },
        { 18, 18, "gpr/18", 4, VCML_ACCESS_READ_WRITE },
        { 19, 19, "gpr/19", 4, VCML_ACCESS_READ_WRITE },
        { 20, 20, "gpr/20", 4, VCML_ACCESS_READ_WRITE },
        { 21, 21, "gpr/21", 4, VCML_ACCESS_READ_WRITE },
        { 22, 22, "gpr/22", 4, VCML_ACCESS_READ_WRITE },
        { 23, 23, "gpr/23", 4, VCML_ACCESS_READ_WRITE },
        { 24, 24, "gpr/24", 4, VCML_ACCESS_READ_WRITE },
        { 25, 25, "gpr/25", 4, VCML_ACCESS_READ_WRITE },
        { 26, 26, "gpr/26", 4, VCML_ACCESS_READ_WRITE },
        { 27, 27, "gpr/27", 4, VCML_ACCESS_READ_WRITE },
        { 28, 28, "gpr/28", 4, VCML_ACCESS_READ_WRITE },
        { 29, 29, "gpr/29", 4, VCML_ACCESS_READ_WRITE },
        { 30, 30, "gpr/30", 4, VCML_ACCESS_READ_WRITE },
        { 31, 31, "gpr/31", 4, VCML_ACCESS_READ_WRITE },

        { SPR(VR),       -1, "spr/sys/vr",       4, VCML_ACCESS_READ  },
        { SPR(UPR),      -1, "spr/sys/upr",      4, VCML_ACCESS_READ  },
        { SPR(CPUCFGR),  -1, "spr/sys/cpucfgr",  4, VCML_ACCESS_READ  },
        { SPR(DMMUCFGR), -1, "spr/sys/dmmucfgr", 4, VCML_ACCESS_READ  },
        { SPR(IMMUCFGR), -1, "spr/sys/immucfgr", 4, VCML_ACCESS_READ  },
        { SPR(DCCFGR),   -1, "spr/sys/dccfgr",   4, VCML_ACCESS_READ  },
        { SPR(ICCFGR),   -1, "spr/sys/iccfgr",   4, VCML_ACCESS_READ  },
        { SPR(VR2),      -1, "spr/sys/vr2",      4, VCML_ACCESS_READ  },
        { SPR(AVR),      -1, "spr/sys/avr",      4, VCML_ACCESS_READ  },
        { SPR(EVBAR),    -1, "spr/sys/evbar",    4, VCML_ACCESS_READ_WRITE },
        { SPR(AECR),     -1, "spr/sys/aecr",     4, VCML_ACCESS_READ_WRITE },
        { SPR(AESR),     -1, "spr/sys/aesr",     4, VCML_ACCESS_READ_WRITE },
        { SPR(NPC),      33, "spr/sys/npc",      4, VCML_ACCESS_READ_WRITE },
        { SPR(SR),       34, "spr/sys/sr",       4, VCML_ACCESS_READ_WRITE },
        { SPR(PPC),      32, "spr/sys/ppc",      4, VCML_ACCESS_READ_WRITE },
        { SPR(FPCSR),    -1, "spr/sys/fpcsr",    4, VCML_ACCESS_READ_WRITE },
        { SPR(EPCR),     -1, "spr/sys/epcr",     4, VCML_ACCESS_READ_WRITE },
        { SPR(EEAR),     -1, "spr/sys/eear",     4, VCML_ACCESS_READ_WRITE },
        { SPR(ESR),      -1, "spr/sys/esr",      4, VCML_ACCESS_READ_WRITE },
        { SPR(COREID),   -1, "spr/sys/coreid",   4, VCML_ACCESS_READ  },
        { SPR(NUMCORES), -1, "spr/sys/numcores", 4, VCML_ACCESS_READ  },

        { SPR(DMMUCR),   -1, "spr/dmmu/cr",      4, VCML_ACCESS_READ_WRITE },
        { SPR(DTLBEIR),  -1, "spr/dmmu/tlbeir",  4, VCML_ACCESS_WRITE },

        { SPR(IMMUCR),   -1, "spr/immu/cr",      4, VCML_ACCESS_READ_WRITE },
        { SPR(ITLBEIR),  -1, "spr/immu/tlbeir",  4, VCML_ACCESS_WRITE },

        { SPR(DCBPR),    -1, "spr/dc/bpr",       4, VCML_ACCESS_READ_WRITE },
        { SPR(DCBFR),    -1, "spr/dc/bfr",       4, VCML_ACCESS_READ_WRITE },
        { SPR(DCBIR),    -1, "spr/dc/bir",       4, VCML_ACCESS_WRITE },
        { SPR(DCBWR),    -1, "spr/dc/bwr",       4, VCML_ACCESS_WRITE },
        { SPR(DCBLR),    -1, "spr/dc/blr",       4, VCML_ACCESS_WRITE },

        { SPR(ICBPR),    -1, "spr/ic/bpr",       4, VCML_ACCESS_WRITE },
        { SPR(ICBIR),    -1, "spr/ic/bir",       4, VCML_ACCESS_WRITE },
        { SPR(ICBLR),    -1, "spr/ic/blr",       4, VCML_ACCESS_WRITE },

        { SPR(MACLO),    -1, "spr/mac/lo",       4, VCML_ACCESS_READ_WRITE },
        { SPR(MACHI),    -1, "spr/mac/hi",       4, VCML_ACCESS_READ_WRITE },

        { SPR(PMR),      -1, "spr/pmr/pm",       4, VCML_ACCESS_READ_WRITE },

        { SPR(PICMR),    -1, "spr/pic/mr",       4, VCML_ACCESS_READ_WRITE },
        { SPR(PICSR),    -1, "spr/pic/sr",       4, VCML_ACCESS_READ_WRITE },

        { SPR(TTMR),     -1, "spr/tt/mr",        4, VCML_ACCESS_READ_WRITE },
        { SPR(TTCR),     -1, "spr/tt/cr",        4, VCML_ACCESS_READ_WRITE },
    };

#undef SPR

    openrisc::openrisc(const sc_core::sc_module_name& nm, unsigned int id):
        vcml::processor(nm),
        or1kiss::env(or1kiss::ENDIAN_BIG),
        m_iss(NULL),
        enable_decode_cache("enable_decode_cache", true),
        enable_sleep_mode("enable_sleep_mode", true),
        enable_insn_dmi("enable_insn_dmi", allow_dmi),
        enable_data_dmi("enable_data_dmi", allow_dmi),
        irq_ompic("irq_ompic", OR1KMVP_IRQ_OMPIC),
        irq_uart0("irq_uart0", OR1KMVP_IRQ_UART0),
        irq_uart1("irq_uart1", OR1KMVP_IRQ_UART1),
        irq_ethoc("irq_ethoc", OR1KMVP_IRQ_ETHOC),
        irq_ocfbc("irq_ocfbc", OR1KMVP_IRQ_OCFBC),
        irq_ockbd("irq_ockbd", OR1KMVP_IRQ_OCKBD),
        irq_ocspi("irq_ocspi", OR1KMVP_IRQ_OCSPI),
        irq_sdhci("irq_sdhci", OR1KMVP_IRQ_SDHCI),
        insn_trace_file("insn_trace_file", ""),
        gdb_term("gdb_term", "or1kmvp-gdbterm") {
        or1kiss::decode_cache_size sz;
        sz = enable_decode_cache ? or1kiss::DECODE_CACHE_SIZE_8M
                                 : or1kiss::DECODE_CACHE_OFF;

        m_iss = new or1kiss::or1k(this, sz);
        m_iss->set_core_id(id);
        m_iss->allow_sleep(enable_sleep_mode);

        if (!insn_trace_file.get().empty())
            m_iss->trace(insn_trace_file);

        register_command("gdb", 0, this, &openrisc::cmd_gdb,
                         "opens a new gdb debug session");
        register_command("pic", 0, this, &openrisc::cmd_pic,
                         "prints PIC status and pending interrupts");
        register_command("spr", 2, this, &openrisc::cmd_spr,
                         "reads or writes SPR <grpid> <regid> [value]");

        set_big_endian();
        define_cpuregs(openrisc_cpuregs);
    }

    openrisc::~openrisc() {
        if (m_iss) delete m_iss;
    }

    void openrisc::reset() {
        processor::reset();

        m_iss->reset_cycles();
        m_iss->reset_instructions();
        m_iss->reset_compiles();
        m_iss->reset_sleep_cycles();

        memset(m_iss->GPR, 0, sizeof(m_iss->GPR));
        m_iss->set_spr(or1kiss::SPR_NPC, 0x100, true);
    }

    std::string openrisc::disassemble(vcml::u64& addr, unsigned char* insn) {
        or1kiss::u32* pinsn = (or1kiss::u32*)insn;
        addr += 4;
        return or1kiss::disassemble(or1kiss::byte_swap(*pinsn));
    }

    vcml::u64 openrisc::get_cpureg(int regno) {
        if (regno < 0)
            return 0;

        if (regno < 32)
            return m_iss->GPR[regno];

        return m_iss->get_spr(regno - 32, true);
    }

    void openrisc::set_cpureg(int regno, vcml::u64 val) {
        if (regno < 0)
            return;

        if (regno < 32)
            m_iss->GPR[regno] = val;
        else
            m_iss->set_spr(regno - 32, val, true);
    }

    vcml::u64 openrisc::get_program_counter() {
        return m_iss->get_spr(or1kiss::SPR_NPC, true);
    }

    vcml::u64 openrisc::get_stack_pointer() {
        return m_iss->GPR[1];
    }

    vcml::u64 openrisc::get_core_id() {
        return m_iss->get_core_id();
    }

    vcml::u64 openrisc::cycle_count() const {
        return m_iss->get_num_instructions() + m_iss->get_num_sleep_cycles();
    }

    void openrisc::interrupt(unsigned int irq, bool set) {
        m_iss->interrupt(irq, set);
    }

    void openrisc::simulate(unsigned int n) {
        switch (m_iss->step(n)) {
        case or1kiss::STEP_EXIT:
            sc_core::sc_stop();
            wait();
            break;

        case or1kiss::STEP_BREAKPOINT:
            m_iss->remove_breakpoint(get_program_counter());
            gdb_notify(vcml::debugging::GDBSIG_TRAP);
            break;

        case or1kiss::STEP_WATCHPOINT:
            gdb_notify(vcml::debugging::GDBSIG_TRAP);
            break;

        case or1kiss::STEP_OK:
        default:
            break;
        }
    }

    void openrisc::handle_clock_update(clock_t oldclk, clock_t newclk) {
        processor::handle_clock_update(oldclk, newclk);
        m_iss->set_clock(newclk);
    }

    or1kiss::response openrisc::transact(const or1kiss::request& req) {
        vcml::sideband info = vcml::SBI_NONE;
        if (req.is_debug())
            info |= vcml::SBI_DEBUG;
        if (req.is_exclusive())
            info |= vcml::SBI_EXCL;

        tlm::tlm_response_status rs;
        vcml::master_socket& port = req.is_imem() ? INSN : DATA;

        sc_core::sc_time now = local_time_stamp();

        unsigned int nbytes = 0;
        if (req.is_write())
            rs = port.write(req.addr, req.data, req.size, info, &nbytes);
        else
            rs = port.read(req.addr, req.data, req.size, info, &nbytes);

        // Time-keeping
        if (!req.is_debug())
            req.cycles = (local_time_stamp() - now) / clock_cycle();

        // Check bus error
        if (rs != tlm::TLM_OK_RESPONSE) {
            log_bus_error(port, req.is_read() ? vcml::VCML_ACCESS_READ :
                          vcml::VCML_ACCESS_WRITE, rs, req.addr, req.size);
            return or1kiss::RESP_ERROR;
        }

        tlm::tlm_dmi dmi;
        if (req.is_dmem() && enable_data_dmi && !get_data_ptr(req.addr)) {
            if (DATA.dmi().lookup(req.addr, req.addr + req.size - 1,
                                  tlm::TLM_READ_COMMAND, dmi)) {
                set_data_ptr(dmi.get_dmi_ptr(),
                             dmi.get_start_address(),
                             dmi.get_end_address());

            }
        }

        if (req.is_imem() && enable_insn_dmi && !get_insn_ptr(req.addr)) {
            if (INSN.dmi().lookup(req.addr, req.addr + req.size - 1,
                                  tlm::TLM_READ_COMMAND, dmi)) {
                set_insn_ptr(dmi.get_dmi_ptr(),
                             dmi.get_start_address(),
                             dmi.get_end_address());
            }
        }

        if (req.is_exclusive() && (nbytes != req.size))
            return or1kiss::RESP_FAILED;
        return or1kiss::RESP_SUCCESS;
    }

    bool openrisc::gdb_page_size(vcml::u64& size) {
        size = OR1KISS_PAGE_SIZE;
        return m_iss->is_dmmu_active() || m_iss->is_immu_active();
    }

    bool openrisc::gdb_virt_to_phys(vcml::u64 va, vcml::u64& pa) {
        if (!m_iss->is_dmmu_active() && !m_iss->is_immu_active()) {
            pa = va;
            return true;
        }

        or1kiss::request req;
        req.set_imem();
        req.set_read();
        req.set_debug();
        req.addr = va;

        if (m_iss->get_dmmu()->translate(req) == or1kiss::MMU_OKAY) {
            pa = req.addr;
            return true;
        }

        if (m_iss->get_immu()->translate(req) == or1kiss::MMU_OKAY) {
            pa = req.addr;
            return true;
        }

        return false;
    }

    bool openrisc::gdb_insert_breakpoint(vcml::u64 addr) {
        if (addr > std::numeric_limits<or1kiss::u32>::max())
            return false;

        m_iss->insert_breakpoint((or1kiss::u32)addr);
        return true;
    }

    bool openrisc::gdb_remove_breakpoint(vcml::u64 addr) {
        if (addr > std::numeric_limits<or1kiss::u32>::max())
            return false;

        m_iss->remove_breakpoint((or1kiss::u32)addr);
        return true;
    }

    bool openrisc::gdb_insert_watchpoint(const vcml::range& mem,
                                         vcml::vcml_access acs) {
        if (mem.end > std::numeric_limits<or1kiss::u32>::max())
            return false;

        or1kiss::u32 addr = (or1kiss::u32)mem.start;
        or1kiss::u32 size = (or1kiss::u32)mem.length();

        if (vcml::is_read_allowed(acs))
            m_iss->insert_watchpoint_r(addr, size);
        if (vcml::is_write_allowed(acs))
            m_iss->insert_watchpoint_w(addr, size);

        return true;
    }

    bool openrisc::gdb_remove_watchpoint(const vcml::range& mem,
                                         vcml::vcml_access acs) {
        if (mem.end > std::numeric_limits<or1kiss::u32>::max())
            return false;

        or1kiss::u32 addr = (or1kiss::u32)mem.start;
        or1kiss::u32 size = (or1kiss::u32)mem.length();

        if (vcml::is_read_allowed(acs))
            m_iss->remove_watchpoint_r(addr, size);
        if (vcml::is_write_allowed(acs))
            m_iss->remove_watchpoint_w(addr, size);

        return true;
    }

}
