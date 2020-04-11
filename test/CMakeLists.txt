 ##############################################################################
 #                                                                            #
 # Copyright 2020 Jan Henrik Weinstock                                        #
 #                                                                            #
 # Licensed under the Apache License, Version 2.0 (the "License");            #
 # you may not use this file except in compliance with the License.           #
 # You may obtain a copy of the License at                                    #
 #                                                                            #
 #     http://www.apache.org/licenses/LICENSE-2.0                             #
 #                                                                            #
 # Unless required by applicable law or agreed to in writing, software        #
 # distributed under the License is distributed on an "AS IS" BASIS,          #
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
 # See the License for the specific language governing permissions and        #
 # limitations under the License.                                             #
 #                                                                            #
 ##############################################################################

macro(linux_boot nrcpu dmi timeout)
    set(name "linux_boot_${nrcpu}_cpus_${dmi}")
    set(dodmi $<STREQUAL:${dmi},dmi>)

    if (${nrcpu} EQUAL 1)
        set(config ${CMAKE_SOURCE_DIR}/config/up.cfg)
    else()
        set(config ${CMAKE_SOURCE_DIR}/config/smp${nrcpu}.cfg)
    endif()

    if (NOT EXISTS ${config})
        message(FATAL_ERROR "Test config file not found: ${config}")
    endif()

    # this causes the tests to use the images and configuration files from the
    # $root/sw directory, so we need to take care not to overwrite anything
    set(argv -f ${config})

    set(argv ${argv} -c system.sdcard0.readonly=true)
    set(argv ${argv} -c system.sdcard1.readonly=true)

    math(EXPR limit "${nrcpu} - 1")
    foreach(cpu RANGE ${limit}) # cmake for loop is [0, limit]
        set(argv ${argv} -c system.cpu${cpu}.enable_data_dmi=${dodmi})
        set(argv ${argv} -c system.cpu${cpu}.enable_insn_dmi=${dodmi})
    endforeach(cpu)

    add_test(NAME ${name} COMMAND
             /usr/bin/expect -f ${CMAKE_CURRENT_SOURCE_DIR}/linux_boot.exp
             $<TARGET_FILE:or1kmvp> ${argv})
    set_tests_properties(${name} PROPERTIES TIMEOUT ${timeout})
endmacro()

linux_boot(1 dmi 60)
linux_boot(2 dmi 60)
linux_boot(4 dmi 60)

linux_boot(1 nodmi 600)
#linux_boot(2 nodmi 600)
#linux_boot(4 nodmi 600)
