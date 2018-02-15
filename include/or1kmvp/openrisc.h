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
                    public or1kiss::env {
    private:
        or1kiss::or1k* m_iss;

    public:
        vcml::property<bool> enable_decode_cache;
        vcml::property<bool> enable_sleep_mode;
        vcml::property<bool> enable_insn_dmi;
        vcml::property<bool> enable_data_dmi;

        vcml::property<unsigned int> irq_ompic;
        vcml::property<unsigned int> irq_uart;
        vcml::property<unsigned int> irq_ethoc;

        vcml::property<std::string> insn_trace_file;

        void log_timing_info() const;

        openrisc(const sc_core::sc_module_name& nm, unsigned int coreid);
        virtual ~openrisc();

        virtual std::string disassemble(vcml::u64&, unsigned char*) override;

        virtual vcml::u64 get_program_counter() override;
        virtual vcml::u64 get_stack_pointer()   override;
        virtual vcml::u64 get_core_id()         override;

        virtual void interrupt(unsigned int irq, bool set) override;
        virtual void simulate(unsigned int&) override;

        virtual or1kiss::response transact(const or1kiss::request& r) override;

        virtual void end_of_elaboration() override;

        virtual vcml::u64 gdb_num_registers() override;
        virtual vcml::u64 gdb_register_width() override;

        virtual bool gdb_read_reg  (vcml::u64 reg, void* buffer,
                                    vcml::u64 size) override;
        virtual bool gdb_write_reg (vcml::u64 reg, const void* buffer,
                                    vcml::u64 size) override;

        virtual bool gdb_page_size(vcml::u64& size) override;
        virtual bool gdb_virt_to_phys(vcml::u64 va, vcml::u64& pa) override;

        virtual bool gdb_insert_breakpoint(vcml::u64 addr) override;
        virtual bool gdb_remove_breakpoint(vcml::u64 addr) override;
    };

}

#endif
