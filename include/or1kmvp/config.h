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

#ifndef OR1KMVP_CONFIG_H
#define OR1KMVP_CONFIG_H

#include "or1kmvp/common.h"

/* Default cpu clock */
#define OR1KMVP_CPU_DEFCLK      (100000000)  // 100MHz

/* Memory map */
#define OR1KMVP_MEM_ADDR        (0x00000000)
#define OR1KMVP_MEM_SIZE        (0x08000000) // 128 MB
#define OR1KMVP_MEM_END         (OR1KMVP_MEM_ADDR + OR1KMVP_MEM_SIZE - 1)

#define OR1KMVP_OMPIC_ADDR      (0x98000000)
#define OR1KMVP_OMPIC_SIZE      (OR1KISS_PAGE_SIZE)
#define OR1KMVP_OMPIC_END       (OR1KMVP_OMPIC_ADDR + OR1KMVP_OMPIC_SIZE - 1)

#define OR1KMVP_UART_ADDR       (0x90000000)
#define OR1KMVP_UART_SIZE       (OR1KISS_PAGE_SIZE)
#define OR1KMVP_UART_END        (OR1KMVP_UART_ADDR + OR1KMVP_UART_SIZE - 1)

#define OR1KMVP_ETH_ADDR        (0x92000000)
#define OR1KMVP_ETH_SIZE        (OR1KISS_PAGE_SIZE)
#define OR1KMVP_ETH_END         (OR1KMVP_ETH_ADDR + OR1KMVP_ETH_SIZE - 1)

#define OR1KMVP_FLASH_ADDR      (0xb0000000)
#define OR1KMVP_FLASH_SIZE      (OR1KISS_PAGE_SIZE)
#define OR1KMVP_FLASH_END       (OR1KMVP_FLASH_ADDR + OR1KMVP_FLASH_SIZE - 1)

#define OR1KMVP_FB_ADDR         (0x97000000)
#define OR1KMVP_FB_SIZE         (OR1KISS_PAGE_SIZE)
#define OR1KMVP_FB_END          (OR1KMVP_FB_ADDR + OR1KMVP_FB_SIZE - 1)

#define OR1KMVP_KB_ADDR         (0x94000000)
#define OR1KMVP_KB_SIZE         (OR1KISS_PAGE_SIZE)
#define OR1KMVP_KB_END          (OR1KMVP_FB_ADDR + OR1KMVP_FB_SIZE - 1)

/* Interrupt map */
#define OR1KMVP_IRQ_MPIC        (1)
#define OR1KMVP_IRQ_UART        (2)
#define OR1KMVP_IRQ_ETH         (4)
#define OR1KMVP_IRQ_KB          (5)

/* Important memory locations */
#define OR1KMVP_KERNEL_ADDR     (0x00000000)
#define OR1KMVP_DTB_ADDR        (0x04000000)

#endif
