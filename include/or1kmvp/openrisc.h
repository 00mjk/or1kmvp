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

#ifndef OR1KMVP_OPENRISC_H
#define OR1KMVP_OPENRISC_H

#include "or1kmvp/common.h"
#include "or1kmvp/config.h"

namespace or1kmvp {

    class openrisc: public vcml::processor,
                    private or1kiss::env {
    private:
        or1kiss::or1k* m_iss;

        bool cmd_gdb(const std::vector<std::string>& args, std::ostream& os);
        bool cmd_pic(const std::vector<std::string>& args, std::ostream& os);
        bool cmd_spr(const std::vector<std::string>& args, std::ostream& os);

    public:
        vcml::property<bool> enable_decode_cache;
        vcml::property<bool> enable_sleep_mode;
        vcml::property<bool> enable_insn_dmi;
        vcml::property<bool> enable_data_dmi;

        vcml::property<unsigned int> irq_ompic;
        vcml::property<unsigned int> irq_uart0;
        vcml::property<unsigned int> irq_uart1;
        vcml::property<unsigned int> irq_ethoc;
        vcml::property<unsigned int> irq_ocfbc;
        vcml::property<unsigned int> irq_ockbd;
        vcml::property<unsigned int> irq_ocspi;
        vcml::property<unsigned int> irq_sdhci;

        vcml::property<std::string> insn_trace_file;
        vcml::property<std::string> gdb_term;

        vcml::u64 insn_count() const { return m_iss->get_num_instructions(); }
        void log_timing_info() const;

        openrisc(const sc_core::sc_module_name& nm, unsigned int coreid);
        virtual ~openrisc();

        virtual void reset() override;

        virtual bool disassemble(vcml::u8*, vcml::u64&, std::string&) override;

        virtual vcml::u64 program_counter() override;
        virtual vcml::u64 link_register() override;
        virtual vcml::u64 stack_pointer() override;

        virtual vcml::u64 core_id() override;
        virtual void set_core_id(vcml::u64 id);

        vcml::u64 cycle_count() const override;

    protected:
        virtual void interrupt(unsigned int irq, bool set) override;
        virtual void simulate(unsigned int) override;
        virtual void handle_clock_update(clock_t prev, clock_t curr) override;

        virtual or1kiss::response transact(const or1kiss::request& r) override;

        virtual bool read_reg_dbg(vcml::u64 idx, vcml::u64& val) override;
        virtual bool write_reg_dbg(vcml::u64 idx, vcml::u64 val) override;

        virtual bool page_size(vcml::u64& size) override;
        virtual bool virt_to_phys(vcml::u64 va, vcml::u64& pa) override;

        virtual bool insert_breakpoint(vcml::u64 addr) override;
        virtual bool remove_breakpoint(vcml::u64 addr) override;

        virtual bool insert_watchpoint(const vcml::range& mem,
                                       vcml::vcml_access acs) override;
        virtual bool remove_watchpoint(const vcml::range& mem,
                                       vcml::vcml_access acs) override;

        virtual void gdb_collect_regs(std::vector<std::string>&) override;
    };

}

#endif
