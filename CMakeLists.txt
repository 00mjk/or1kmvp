 ##############################################################################
 #                                                                            #
 # Copyright 2018 Jan Henrik Weinstock                                        #
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

project(or1kmvp)
cmake_minimum_required(VERSION 3.5)

option(STATIC "Build static simulator binary" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(LibElf REQUIRED)
find_package(SystemC REQUIRED)
find_package(vcml REQUIRED)
find_package(or1kiss REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_EXE_LINKER_FLAGS -rdynamic)

set(src "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(inc "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(sources
    ${src}/or1kmvp/openrisc.cpp
    ${src}/or1kmvp/system.cpp
    ${src}/main.cpp)

add_executable(or1kmvp ${sources})

target_include_directories(or1kmvp PRIVATE ${inc})
target_include_directories(or1kmvp PRIVATE ${LIBELF_INCLUDE_DIRS})
target_include_directories(or1kmvp PRIVATE ${OR1KISS_INCLUDE_DIRS})
target_include_directories(or1kmvp PRIVATE ${VCML_INCLUDE_DIRS})
target_include_directories(or1kmvp SYSTEM PRIVATE ${SYSTEMC_INCLUDE_DIRS})

target_link_libraries(or1kmvp ${OR1KISS_LIBRARIES})
target_link_libraries(or1kmvp ${VCML_LIBRARIES})
target_link_libraries(or1kmvp ${SYSTEMC_LIBRARIES})
target_link_libraries(or1kmvp ${LIBELF_LIBRARIES})
target_link_libraries(or1kmvp -pthread)

if (STATIC)
    target_link_libraries(or1kmvp -static)
endif()

install(TARGETS or1kmvp DESTINATION bin)
install(DIRECTORY config/ DESTINATION config)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/sw/)
    install(DIRECTORY sw/ DESTINATION sw)
endif()

add_subdirectory(utils)

foreach(util tapctl tapnet)
    set(usrc $ENV{VCML_HOME}/bin/vcml-${util})
    if (EXISTS ${usrc})
        install(PROGRAMS ${usrc} DESTINATION bin RENAME or1kmvp-${util})
    endif()
endforeach()
