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

find_path(OR1KISS_INCLUDE_DIRS NAMES or1kiss.h
          HINTS $ENV{OR1KISS_HOME}/include /opt/or1kiss/include)

find_library(OR1KISS_LIBRARIES NAMES or1kiss
             HINTS $ENV{OR1KISS_HOME}/lib /opt/or1kiss/lib)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OR1KISS DEFAULT_MSG
                                  OR1KISS_LIBRARIES
                                  OR1KISS_INCLUDE_DIRS)

mark_as_advanced(OR1KISS_INCLUDE_DIRS OR1KISS_LIBRARIES)

#message(STATUS "OR1KISS_FOUND        " ${OR1KISS_FOUND})
#message(STATUS "OR1KISS_INCLUDE_DIRS " ${OR1KISS_INCLUDE_DIRS})
#message(STATUS "OR1KISS_LIBRARIES    " ${OR1KISS_LIBRARIES})
