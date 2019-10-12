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

#include "or1kmvp/system.h"

namespace or1kmvp {

    system::system(const sc_core::sc_module_name& nm):
        vcml::module(nm),
        session("session", 0),
        vspdebug("vspdebug", false),
        quantum("quantum", sc_core::sc_time(1, sc_core::SC_US)),
        duration("duration", sc_core::SC_ZERO_TIME),
        nrcpu("nrcpu", 1),
        mem("mem", vcml::range(OR1KMVP_MEM_ADDR, OR1KMVP_MEM_END)),
        uart0("uart0", vcml::range(OR1KMVP_UART0_ADDR, OR1KMVP_UART0_END)),
        uart1("uart1", vcml::range(OR1KMVP_UART1_ADDR, OR1KMVP_UART1_END)),
        rtc  ("rtc",   vcml::range(OR1KMVP_RTC_ADDR,   OR1KMVP_RTC_END)),
        gpio ("gpio",  vcml::range(OR1KMVP_GPIO_ADDR,  OR1KMVP_GPIO_END)),
        ethoc("ethoc", vcml::range(OR1KMVP_ETHOC_ADDR, OR1KMVP_ETHOC_END)),
        ocfbc("ocfbc", vcml::range(OR1KMVP_OCFBC_ADDR, OR1KMVP_OCFBC_END)),
        ockbd("ethoc", vcml::range(OR1KMVP_OCKBD_ADDR, OR1KMVP_OCKBD_END)),
        ocspi("ocspi", vcml::range(OR1KMVP_OCSPI_ADDR, OR1KMVP_OCSPI_END)),
        ompic("ompic", vcml::range(OR1KMVP_OMPIC_ADDR, OR1KMVP_OMPIC_END)),
        m_cpus(nrcpu),
        m_clock("clock", OR1KMVP_CPU_DEFCLK),
        m_reset("reset"),
        m_bus("bus"),
        m_mem("mem", mem.get().length()),
        m_uart0("uart0"),
        m_uart1("uart1"),
        m_rtc("rtc", vcml::generic::rtc1742::NVMEM_8K),
        m_gpio("gpio"),
        m_ethoc("ethoc"),
        m_ocfbc("ocfbc"),
        m_ockbd("ockbd"),
        m_ocspi("ocspi"),
        m_ompic("ompic", nrcpu),
        m_spibus("spibus"),
        m_spi2sd("spi2sd"),
        m_sdcard("sdcard"),
        m_sig_clock("sig_clock"),
        m_sig_reset("sig_reset"),
        m_gpio_spi0("gpio_spi0"),
        m_irq_uart0("irq_uart0"),
        m_irq_uart1("irq_uart1"),
        m_irq_ethoc("irq_ethoc"),
        m_irq_ocfbc("irq_ocfbc"),
        m_irq_ockbd("irq_ockbd"),
        m_irq_ocspi("irq_ocspi"),
        m_irq_ompic(nrcpu) {

        m_uart0.set_big_endian();
        m_uart1.set_big_endian();
        m_rtc.set_big_endian();
        m_gpio.set_big_endian();
        m_ethoc.set_big_endian();
        m_ocfbc.set_big_endian();
        m_ockbd.set_big_endian();
        m_ocspi.set_big_endian();
        m_ompic.set_big_endian();

        for (unsigned int cpu = 0; cpu < nrcpu; cpu++) {
            std::stringstream ss; ss << "cpu" << cpu;
            m_cpus[cpu] = new openrisc(ss.str().c_str(), cpu);
        }

        // Bus mapping
        for (openrisc* cpu : m_cpus) {
           m_bus.bind(cpu->INSN);
           m_bus.bind(cpu->DATA);
        }

        m_bus.bind(m_mem.IN, mem);
        m_bus.bind(m_uart0.IN, uart0);
        m_bus.bind(m_uart1.IN, uart1);
        m_bus.bind(m_rtc.IN, rtc);
        m_bus.bind(m_gpio.IN, gpio);
        m_bus.bind(m_ompic.IN, ompic);
        m_bus.bind(m_ethoc.IN, ethoc);
        m_bus.bind(m_ethoc.OUT);
        m_bus.bind(m_ocfbc.IN, ocfbc);
        m_bus.bind(m_ocfbc.OUT);
        m_bus.bind(m_ockbd.IN, ockbd);
        m_bus.bind(m_ocspi.IN, ocspi);

        // Clock
        m_clock.CLOCK.bind(m_sig_clock);
        m_bus.CLOCK.bind(m_sig_clock);
        m_mem.CLOCK.bind(m_sig_clock);
        m_uart0.CLOCK.bind(m_sig_clock);
        m_uart1.CLOCK.bind(m_sig_clock);
        m_rtc.CLOCK.bind(m_sig_clock);
        m_gpio.CLOCK.bind(m_sig_clock);
        m_ethoc.CLOCK.bind(m_sig_clock);
        m_ocfbc.CLOCK.bind(m_sig_clock);
        m_ockbd.CLOCK.bind(m_sig_clock);
        m_ocspi.CLOCK.bind(m_sig_clock);
        m_ompic.CLOCK.bind(m_sig_clock);
        m_spibus.CLOCK.bind(m_sig_clock);
        m_spi2sd.CLOCK.bind(m_sig_clock);
        m_sdcard.CLOCK.bind(m_sig_clock);

        for (auto cpu : m_cpus)
            cpu->CLOCK.bind(m_sig_clock);

        // Reset
        m_reset.RESET.bind(m_sig_reset);
        m_bus.RESET.bind(m_sig_reset);
        m_mem.RESET.bind(m_sig_reset);
        m_uart0.RESET.bind(m_sig_reset);
        m_uart1.RESET.bind(m_sig_reset);
        m_rtc.RESET.bind(m_sig_reset);
        m_gpio.RESET.bind(m_sig_reset);
        m_ethoc.RESET.bind(m_sig_reset);
        m_ocfbc.RESET.bind(m_sig_reset);
        m_ockbd.RESET.bind(m_sig_reset);
        m_ocspi.RESET.bind(m_sig_reset);
        m_ompic.RESET.bind(m_sig_reset);
        m_spibus.RESET.bind(m_sig_reset);
        m_spi2sd.RESET.bind(m_sig_reset);
        m_sdcard.RESET.bind(m_sig_reset);

        for (auto cpu : m_cpus)
            cpu->RESET.bind(m_sig_reset);

        // GPIOs
        m_gpio.GPIO[0].bind(m_gpio_spi0);

        // IRQ mapping
        m_uart0.IRQ.bind(m_irq_uart0);
        m_uart1.IRQ.bind(m_irq_uart1);
        m_ethoc.IRQ.bind(m_irq_ethoc);
        m_ocfbc.IRQ.bind(m_irq_ocfbc);
        m_ockbd.IRQ.bind(m_irq_ockbd);
        m_ocspi.IRQ.bind(m_irq_ocspi);

        for (auto cpu : m_cpus) {
            unsigned int irq_uart0 = cpu->irq_uart0;
            unsigned int irq_uart1 = cpu->irq_uart1;
            unsigned int irq_ethoc = cpu->irq_ethoc;
            unsigned int irq_ocfbc = cpu->irq_ocfbc;
            unsigned int irq_ockbd = cpu->irq_ockbd;
            unsigned int irq_ocspi = cpu->irq_ocspi;
            unsigned int irq_ompic = cpu->irq_ompic;

            cpu->IRQ[irq_uart0].bind(m_irq_uart0);
            cpu->IRQ[irq_uart1].bind(m_irq_uart1);
            cpu->IRQ[irq_ethoc].bind(m_irq_ethoc);
            cpu->IRQ[irq_ocfbc].bind(m_irq_ocfbc);
            cpu->IRQ[irq_ockbd].bind(m_irq_ockbd);
            cpu->IRQ[irq_ocspi].bind(m_irq_ocspi);

            vcml::u64 id = cpu->get_core_id();
            std::stringstream ss; ss << "irq_ompic_cpu" << id;
            m_irq_ompic[id] = new sc_core::sc_signal<bool>(ss.str().c_str());
            cpu->IRQ[irq_ompic].bind(*m_irq_ompic[id]);
            m_ompic.IRQ[id].bind(*m_irq_ompic[id]);
        }

        // SPI controller -> SPI bus -> SD bus -> SD card
        m_ocspi.SPI_OUT.bind(m_spibus.SPI_IN);
        m_spibus.bind(m_spi2sd.SPI_IN, m_gpio_spi0, false); // CS_ACTIVE_LOW
        m_spi2sd.SD_OUT.bind(m_sdcard.SD_IN);
    }

    system::~system() {
        for (auto irq : m_irq_ompic)
            SAFE_DELETE(irq);
        for (auto cpu : m_cpus)
            SAFE_DELETE(cpu);
    }

    void system::run() {
        tlm::tlm_global_quantum::instance().set(quantum);
        if (session > 0) {
            vcml::debugging::vspserver vspsession(session);
            vspsession.echo(vspdebug);
            vspsession.start();
        } else if (duration != sc_core::SC_ZERO_TIME) {
            log_info("starting simulation until %s using %s quantum",
                           duration.get().to_string().c_str(),
                           quantum.get().to_string().c_str());
            sc_core::sc_start(duration);
            log_info("simulation stopped");
        } else {
            log_info("starting infinite simulation using %s quantum",
                           quantum.get().to_string().c_str());
            sc_core::sc_start();
            log_info("simulation stopped");
        }
    }

    void system::run_timed() {
        double simstart = vcml::realtime();
        run();
        double realtime = vcml::realtime() - simstart;
        double duration = sc_core::sc_time_stamp().to_seconds();

        log_info("duration           %.9fs", duration);
        log_info("runtime            %.4fs", realtime);
        log_info("real time ratio    %.2fs / 1s", duration == 0.0 ? 0.0 :
                                                  realtime / duration);

        vcml::u64 ninsn = 0;
        for (auto cpu : m_cpus)
            ninsn += cpu->insn_count();
        log_info("sim speed          %.1f MIPS", realtime == 0.0 ? 0.0 :
                                                 ninsn / realtime / 1e6);

        for (auto cpu : m_cpus)
            cpu->log_timing_info();
    }

    void system::end_of_elaboration() {
        std::stringstream ss;
        m_bus.execute("show", VCML_NO_ARGS, ss);
        vcml::log_debug(ss.str().c_str());
    }

}
