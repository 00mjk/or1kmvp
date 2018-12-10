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

    class system: public vcml::component
    {
    public:
        vcml::property<unsigned short> session;
        vcml::property<bool> vspdebug;

        vcml::property<sc_core::sc_time> quantum;
        vcml::property<sc_core::sc_time> duration;

        vcml::property<unsigned int> nrcpu;

        vcml::property<vcml::range> mem;
        vcml::property<vcml::range> uart0;
        vcml::property<vcml::range> uart1;
        vcml::property<vcml::range> rtc;
        vcml::property<vcml::range> ethoc;
        vcml::property<vcml::range> ocfbc;
        vcml::property<vcml::range> ockbd;
        vcml::property<vcml::range> ompic;

        system(const sc_core::sc_module_name& name);
        virtual ~system();

        void run();
        void run_timed();

        virtual void end_of_elaboration() override;

    private:
        std::vector<openrisc*>   m_cpus;

        vcml::generic::bus       m_bus;
        vcml::generic::memory    m_mem;
        vcml::generic::uart8250  m_uart0;
        vcml::generic::uart8250  m_uart1;
        vcml::generic::rtc1742   m_rtc;
        vcml::opencores::ethoc   m_ethoc;
        vcml::opencores::ocfbc   m_ocfbc;
        vcml::opencores::ockbd   m_ockbd;
        vcml::opencores::ompic   m_ompic;

        sc_core::sc_signal<bool> m_irq_uart0;
        sc_core::sc_signal<bool> m_irq_uart1;
        sc_core::sc_signal<bool> m_irq_ethoc;
        sc_core::sc_signal<bool> m_irq_ocfbc;
        sc_core::sc_signal<bool> m_irq_ockbd;

        std::vector<sc_core::sc_signal<bool>*> m_irq_ompic;
    };

}


#endif
