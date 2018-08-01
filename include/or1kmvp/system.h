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

#ifndef OR1KMVP_SYSTEM_H
#define OR1KMVP_SYSTEM_H

#include "or1kmvp/common.h"
#include "or1kmvp/config.h"
#include "or1kmvp/openrisc.h"

namespace or1kmvp {

    class system: public sc_core::sc_module
    {
    private:
        struct timeval m_sim_start;
        struct timeval m_sim_end;

        std::vector<openrisc*> m_cpus;

        void build_processors();
        void clean_processors();

        vcml::generic::bus*      m_bus;
        vcml::generic::memory*   m_mem;
        vcml::generic::uart8250* m_uart;
        vcml::opencores::ompic*  m_ompic;
        vcml::opencores::ethoc*  m_ethoc;

        void build_components();
        void clean_components();

        vcml::generic::crossbar* m_xbar_uart;
        vcml::generic::crossbar* m_xbar_ethoc;

        sc_core::sc_signal<bool>* m_irq_uart;
        sc_core::sc_signal<bool>* m_irq_ethoc;

        std::vector<sc_core::sc_signal<bool>*> m_irq_percpu_uart;
        std::vector<sc_core::sc_signal<bool>*> m_irq_percpu_ethoc;
        std::vector<sc_core::sc_signal<bool>*> m_irq_percpu_ompic;

        void build_interrupts();
        void clean_interrupts();

        void connect_bus();
        void connect_irq();

    public:
        vcml::property<unsigned short> session;
        vcml::property<bool> vspdebug;

        vcml::property<sc_core::sc_time> quantum;
        vcml::property<sc_core::sc_time> duration;

        vcml::property<unsigned int> nrcpu;

        vcml::property<vcml::range> mem;
        vcml::property<vcml::range> uart;
        vcml::property<vcml::range> ompic;
        vcml::property<vcml::range> ethoc;

        system(const sc_core::sc_module_name& name);
        virtual ~system();

        void construct();
        void run();

        void log_timing_stats() const;

        virtual void end_of_elaboration() override;
    };

}


#endif
