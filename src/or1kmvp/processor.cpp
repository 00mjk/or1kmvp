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

#include "or1kmvp/processor.h"

namespace or1kmvp {

    void processor::start_gdb() {
        if (m_gdb != NULL) {
            vcml::log_warning("closing previous gdb session");
            stop_gdb();
        }

        m_gdb = new or1kiss::gdb(*m_iss, gdb_port);
        m_gdb->show_warnings();

        if (!symbols.get().empty()) {
            m_elf = new or1kiss::elf(symbols);
            m_gdb->set_elf(m_elf);
        }
    }

    void processor::stop_gdb() {
        if (m_gdb != NULL) {
            delete m_gdb;
            m_gdb = NULL;
        }

        if (m_elf != NULL) {
            delete m_elf;
            m_elf = NULL;
        }
    }

    void processor::log_timing_info() const {
        vcml::log_info("processor '%s'", name());
        vcml::log_info("  clock speed  : %.1f MHz", clock / 1e6);
        vcml::log_info("  sim speed    : %.1f MIPS",
                       m_iss->get_num_instructions() / get_run_time() * 1e-6);
        vcml::log_info("  instructions : %" PRId64,
                       m_iss->get_num_instructions());
        vcml::log_info("  cycles       : %" PRId64, get_num_cycles());
        vcml::log_info("  sleep-cycles : %" PRId64 " (%.1f%%)",
                       m_iss->get_num_sleep_cycles(),
                       m_iss->get_num_sleep_cycles() / get_num_cycles() * 100);
        vcml::log_info("  jit hit-rate : %.4f",
                       m_iss->get_decode_cache_hit_rate());
        vcml::log_info("  #lwa         : %" PRId64, m_iss->get_num_lwa());
        vcml::log_info("  #swa         : %" PRId64, m_iss->get_num_swa());
        vcml::log_info("  #swa failed  : %" PRId64,
                       m_iss->get_num_swa_failed());

        for (auto irq : IRQ) {
            vcml::irq_stats stats;
            if (!get_irq_stats(irq.first, stats) || stats.irq_count == 0)
                continue;

            std::string s;
            s += vcml::mkstr("  irq %d status :", stats.irq);
            s += vcml::mkstr(" %d#", stats.irq_count);
            s += vcml::mkstr(", avg %.1fus",
                    stats.irq_uptime.to_seconds() / stats.irq_count * 1e6);
            s += vcml::mkstr(", max %.1fus",
                    stats.irq_longest.to_seconds() * 1e6);
            vcml::log_info(s.c_str());
        }
    }

    processor::processor(const sc_core::sc_module_name& nm, unsigned int id):
        vcml::processor(nm, OR1KMVP_CPU_DEFCLK),
        or1kiss::env(or1kiss::ENDIAN_BIG),
        m_iss(NULL),
        m_gdb(NULL),
        m_elf(NULL),
        enable_decode_cache("enable_decode_cache", true),
        enable_sleep_mode("enable_sleep_mode", true),
        enable_insn_dmi("enable_insn_dmi", true),
        enable_data_dmi("enable_data_dmi", false),
        irq_mpic("irq_mpic", OR1KMVP_IRQ_MPIC),
        irq_uart("irq_uart", OR1KMVP_IRQ_UART),
        irq_eth("irq_eth", OR1KMVP_IRQ_ETH),
        irq_kb("irq_kb", OR1KMVP_IRQ_KB),
        insn_trace_file("insn_trace_file", ""),
        gdb_port("gdb_port", 0),
        gdb_wait("gdb_wait", false) {
        or1kiss::decode_cache_size sz;
        sz = enable_decode_cache ? or1kiss::DECODE_CACHE_SIZE_8M
                                 : or1kiss::DECODE_CACHE_OFF;

        m_iss = new or1kiss::or1k(this, sz);
        m_iss->set_clock(clock);
        m_iss->set_core_id(id);
        m_iss->allow_sleep(enable_sleep_mode);

        if (!insn_trace_file.get().empty())
            m_iss->trace(insn_trace_file);

        if (gdb_wait)
            start_gdb();
    }

    processor::~processor() {
        if (m_iss) delete m_iss;
        if (m_gdb) delete m_gdb;
    }

    bool processor::insert_breakpoint(vcml::u64 address) {
        m_iss->insert_breakpoint(address);
        return true;
    }
    bool processor::remove_breakpoint(vcml::u64 address) {
        m_iss->remove_breakpoint(address);
        return true;
    }

    bool processor::virt_to_phys(vcml::u64 vaddr, vcml::u64& paddr) {
        if (!m_iss->is_immu_active()) {
            paddr = vaddr;
            return true;
        }

        or1kiss::request req;
        req.set_imem();
        req.set_read();
        req.set_debug();
        req.addr = vaddr;

        if (m_iss->get_immu()->translate(req) != or1kiss::MMU_OKAY)
            return false;

        paddr = req.addr;
        return true;
    }

    std::string processor::disassemble(vcml::u64& addr, unsigned char* insn) {
        or1kiss::u32* pinsn = (or1kiss::u32*)insn;
        addr += 4;
        return or1kiss::disassemble(or1kiss::byte_swap(*pinsn));
    }

    vcml::u64 processor::get_program_counter() {
        return m_iss->get_spr(or1kiss::SPR_NPC, true);
    }

    vcml::u64 processor::get_stack_pointer() {
        return m_iss->GPR[1];
    }

    vcml::u64 processor::get_core_id() {
        return m_iss->get_core_id();
    }

    void processor::interrupt(unsigned int irq, bool set) {
        m_iss->interrupt(irq, set);
    }

    void processor::simulate(unsigned int& n) {
        if ((m_gdb != NULL) && (!m_gdb->is_connected()))
            stop_gdb();

        m_iss->set_clock(clock);
        or1kiss::step_result result;
        result = (m_gdb != NULL) ? m_gdb->step(n)
                                 : m_iss->step(n);
        switch (result) {
        case or1kiss::STEP_EXIT:
            sc_core::sc_stop();
            break;

        case or1kiss::STEP_BREAKPOINT:
            m_iss->remove_breakpoint(get_program_counter());
            sc_core::sc_pause();
            break;

        case or1kiss::STEP_WATCHPOINT:
            sc_core::sc_pause();
            break;

        case or1kiss::STEP_OK:
        default:
            break;
        }
    }

    static const char* g_buserror =
        "[E %.9fs] detected bus error during operation\n"
        "  addr: 0x%08" PRIx32 " (%s)\n"
        "  pc  : 0x%08" PRIx32 "\n"
        "  sp  : 0x%08" PRIx32 "\n"
        "  size: %d bytes\n"
        "  core: %d\n"
        "  port: %s\n"
        "  code: %s\n";

    or1kiss::response processor::transact(const or1kiss::request& req) {
        int flags = vcml::VCML_FLAG_NONE;
        if (req.is_debug())
            flags |= vcml::VCML_FLAG_DEBUG;
        if (req.is_exclusive())
            flags |= vcml::VCML_FLAG_EXCL;

        tlm::tlm_response_status rs;
        vcml::master_socket& port = req.is_imem() ? INSN : DATA;
        sc_core::sc_time now = sc_core::sc_time_stamp();

        unsigned int nbytes = 0;
        if (req.is_write())
            rs = port.write(req.addr, req.data, req.size, flags, &nbytes);
        else
            rs = port.read(req.addr, req.data, req.size, flags, &nbytes);

        // Time-keeping
        if (!req.is_debug()) {
            sc_core::sc_time delta = sc_core::sc_time_stamp() - now;
            req.cycles = delta.to_seconds() * this->clock + 0.5; // round up
        }

        // Check bus error
        if (rs != tlm::TLM_OK_RESPONSE) {
            if (!req.is_debug()) {
                double timestamp = sc_core::sc_time_stamp().to_seconds();
                char str[256];
                snprintf(str, sizeof(str), g_buserror, timestamp, req.addr,
                         req.is_write() ? "write" : "read",
                         m_iss->get_spr(or1kiss::SPR_NPC, true), m_iss->GPR[1],
                         req.size, m_iss->get_core_id(),
                         req.is_imem() ? "INSN" : "DATA",
                         vcml::tlm_response_to_str(rs).c_str());
                vcml::log_warning(str);
            }

            return or1kiss::RESP_ERROR;
        }

        tlm::tlm_dmi dmi;
        if (get_dmi().lookup(req.addr, req.addr + req.size -1,
                             tlm::TLM_READ_COMMAND, dmi)) {
            if (enable_data_dmi && req.is_dmem()) {
                set_data_ptr(dmi.get_dmi_ptr(),
                             dmi.get_start_address(),
                             dmi.get_end_address());
            }

            if (enable_insn_dmi && req.is_imem()) {
                set_insn_ptr(dmi.get_dmi_ptr(),
                             dmi.get_start_address(),
                             dmi.get_end_address());
            }
        }

        if (req.is_exclusive() && (nbytes != req.size))
            return or1kiss::RESP_FAILED;
        return or1kiss::RESP_SUCCESS;
    }

}
