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

    class system: public vcml::system
    {
    public:
        vcml::property<unsigned int> nrcpu;
        vcml::property<vcml::range>  mem;
        vcml::property<vcml::range>  uart0;
        vcml::property<vcml::range>  uart1;
        vcml::property<vcml::range>  rtc;
        vcml::property<vcml::range>  gpio;
        vcml::property<vcml::range>  ethoc;
        vcml::property<vcml::range>  ocfbc;
        vcml::property<vcml::range>  ockbd;
        vcml::property<vcml::range>  ocspi;
        vcml::property<vcml::range>  ompic;
        vcml::property<vcml::range>  hwrng;

        system() = delete;
        system(const sc_core::sc_module_name& name);
        virtual ~system();

        virtual int run() override;

        virtual void end_of_elaboration() override;

    private:
        std::vector<openrisc*>       m_cpus;

        vcml::generic::clock         m_clock;
        vcml::generic::reset         m_reset;
        vcml::generic::bus           m_bus;
        vcml::generic::memory        m_mem;
        vcml::generic::uart8250      m_uart0;
        vcml::generic::uart8250      m_uart1;
        vcml::generic::rtc1742       m_rtc;
        vcml::generic::gpio          m_gpio;
        vcml::generic::hwrng         m_hwrng;
        vcml::opencores::ethoc       m_ethoc;
        vcml::opencores::ocfbc       m_ocfbc;
        vcml::opencores::ockbd       m_ockbd;
        vcml::opencores::ocspi       m_ocspi;
        vcml::opencores::ompic       m_ompic;

        vcml::generic::spibus        m_spibus;
        vcml::generic::spi2sd        m_spi2sd;
        vcml::generic::sdcard        m_sdcard;

        sc_core::sc_signal<clock_t>  m_sig_clock;
        sc_core::sc_signal<bool>     m_sig_reset;

        sc_core::sc_signal<bool>     m_gpio_spi0;

        sc_core::sc_signal<bool>     m_irq_uart0;
        sc_core::sc_signal<bool>     m_irq_uart1;
        sc_core::sc_signal<bool>     m_irq_ethoc;
        sc_core::sc_signal<bool>     m_irq_ocfbc;
        sc_core::sc_signal<bool>     m_irq_ockbd;
        sc_core::sc_signal<bool>     m_irq_ocspi;

        std::vector<sc_core::sc_signal<bool>*> m_irq_ompic;
    };

}


#endif
