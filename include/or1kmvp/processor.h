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

#ifndef OR1KMVP_PROCESSOR_H
#define OR1KMVP_PROCESSOR_H

#include "or1kmvp/common.h"
#include "or1kmvp/config.h"

namespace or1kmvp {

    class processor: public vcml::processor,
                     public or1kiss::env {
    private:
        or1kiss::or1k* m_iss;
        or1kiss::gdb*  m_gdb;
        or1kiss::elf*  m_elf;

    public:
        vcml::property<bool> enable_decode_cache;
        vcml::property<bool> enable_sleep_mode;
        vcml::property<bool> enable_insn_dmi;
        vcml::property<bool> enable_data_dmi;

        vcml::property<unsigned int> irq_mpic;
        vcml::property<unsigned int> irq_uart;
        vcml::property<unsigned int> irq_eth;
        vcml::property<unsigned int> irq_kb;

        vcml::property<std::string> insn_trace_file;

        vcml::property<unsigned short> gdb_port;
        vcml::property<bool>           gdb_wait;

        void start_gdb();
        void stop_gdb();

        void log_timing_info() const;

        processor(const sc_core::sc_module_name& nm, unsigned int coreid);
        virtual ~processor();

        virtual bool insert_breakpoint(vcml::u64 address) override;
        virtual bool remove_breakpoint(vcml::u64 address) override;

        virtual bool virt_to_phys(vcml::u64 vaddr, vcml::u64& paddr) override;
        virtual std::string disassemble(vcml::u64&, unsigned char*) override;

        virtual vcml::u64 get_program_counter() override;
        virtual vcml::u64 get_stack_pointer()   override;
        virtual vcml::u64 get_core_id()         override;

        virtual void interrupt(unsigned int irq, bool set) override;
        virtual void simulate(unsigned int&) override;

        virtual or1kiss::response transact(const or1kiss::request& r) override;
    };

}

#endif
