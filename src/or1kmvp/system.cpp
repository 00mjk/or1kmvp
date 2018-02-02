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

    void system::build_processors() {
        m_cpus.resize(nrcpu);
        for (unsigned int cpu = 0; cpu < nrcpu; cpu++) {
            std::stringstream ss; ss << "cpu" << cpu;
            m_cpus[cpu] = new openrisc(ss.str().c_str(), cpu);
        }
    }

    void system::clean_processors() {
        for (unsigned int cpu = 0; cpu < m_cpus.size(); cpu++)
            delete m_cpus[cpu];
    }

    void system::build_components() {
        m_bus   = new vcml::generic::bus("bus");
        m_mem   = new vcml::generic::memory("mem", mem.get().length());
        m_uart  = new vcml::generic::uart8250("uart");
        m_ompic = new vcml::opencores::ompic("ompic", nrcpu);

        m_uart->set_big_endian();
        m_ompic->set_big_endian();
    }

    void system::clean_components() {
        delete m_bus;
        delete m_mem;
        delete m_uart;
        delete m_ompic;
    }

    void system::build_interrupts() {
        m_xbar_uart = new vcml::generic::crossbar("xbar_uart");
        m_xbar_uart->set_big_endian();

        m_irq_uart = new sc_core::sc_signal<bool>("irq_uart");

        m_irq_percpu_mpic.resize(nrcpu);
        m_irq_percpu_uart.resize(nrcpu);

        std::stringstream ss;
        for (unsigned int cpu = 0; cpu < nrcpu; cpu++) {
            ss.str(""); ss << "irq_percpu_mpic_" << cpu;
            m_irq_percpu_mpic[cpu] = new sc_core::sc_signal<bool>(ss.str().c_str());

            ss.str(""); ss << "irq_percpu_uart" << cpu;
            m_irq_percpu_uart[cpu] = new sc_core::sc_signal<bool>(ss.str().c_str());
        }
    }

    void system::clean_interrupts() {
        delete m_xbar_uart;
        delete m_irq_uart;
        for (unsigned int cpu = 0; cpu < nrcpu; cpu++) {
            delete m_irq_percpu_mpic[cpu];
            delete m_irq_percpu_uart[cpu];
        }
    }

    void system::connect_bus() {
        for (openrisc* cpu : m_cpus) {
           m_bus->bind(cpu->INSN);
           m_bus->bind(cpu->DATA);
        }

        m_bus->bind(m_mem->IN, mem);
        m_bus->bind(m_uart->IN, uart);
        m_bus->bind(m_ompic->IN, ompic);
    }

    void system::connect_irq() {
        // Connect device irq to crossbar
        m_uart->IRQ.bind(*m_irq_uart);
        m_xbar_uart->IN[0].bind(*m_irq_uart);

        // Connect crossbar, ompic and cpu
        for (unsigned int cpu = 0; cpu < nrcpu; cpu++) {
            m_ompic->IRQ[cpu].bind(*m_irq_percpu_mpic[cpu]);
            m_xbar_uart->OUT[cpu].bind(*m_irq_percpu_uart[cpu]);

            m_xbar_uart->set_broadcast(cpu);

            unsigned int irq_mpic = m_cpus[cpu]->irq_mpic;
            unsigned int irq_uart = m_cpus[cpu]->irq_uart;

            m_cpus[cpu]->IRQ[irq_mpic].bind(*m_irq_percpu_mpic[cpu]);
            m_cpus[cpu]->IRQ[irq_uart].bind(*m_irq_percpu_uart[cpu]);
        }
    }

    system::system(const sc_core::sc_module_name& nm):
        sc_core::sc_module(nm),
        m_sim_start(),
        m_sim_end(),
        m_cpus(),
        m_bus(NULL),
        m_mem(NULL),
        m_uart(NULL),
        m_ompic(NULL),
        m_xbar_uart(NULL),
        m_irq_uart(NULL),
        m_irq_percpu_mpic(),
        m_irq_percpu_uart(),
        quantum("quantum", sc_core::sc_time(1, sc_core::SC_US)),
        duration("duration", sc_core::SC_ZERO_TIME),
        nrcpu("nrcpu", 1),
        mem("mem", vcml::range(OR1KMVP_MEM_ADDR, OR1KMVP_MEM_END)),
        uart("uart",  vcml::range(OR1KMVP_UART_ADDR, OR1KMVP_UART_END)),
        ompic("ompic", vcml::range(OR1KMVP_OMPIC_ADDR, OR1KMVP_OMPIC_END)) {
        build_processors();
        build_components();
        build_interrupts();

        connect_bus();
        connect_irq();
    }

    system::~system() {
        clean_interrupts();
        clean_components();
        clean_processors();
    }

    void system::run() {
        tlm::tlm_global_quantum::instance().set(quantum);
        if (duration != sc_core::SC_ZERO_TIME) {
            gettimeofday(&m_sim_start, NULL);
            sc_core::sc_start(duration);
            gettimeofday(&m_sim_end, NULL);
        } else {
            gettimeofday(&m_sim_start, NULL);
            sc_core::sc_start();
            gettimeofday(&m_sim_end, NULL);
        }
    }

    void system::log_timing_stats() const {
        double duration = sc_core::sc_time_stamp().to_seconds();
        double realtime = (m_sim_end.tv_sec  - m_sim_start.tv_sec) +
                          (m_sim_end.tv_usec - m_sim_start.tv_usec) / 1e6;

        vcml::log_info("simulation stopped");
        vcml::log_info("  duration     : %.9fs", duration);
        vcml::log_info("  time         : %.4fs", realtime);
        vcml::log_info("  time ratio   : %.2fs / 1s", realtime / duration);

        for (auto cpu : m_cpus)
            cpu->log_timing_info();
    }

}
