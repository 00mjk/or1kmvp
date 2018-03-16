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

#include "or1kmvp/common.h"
#include "or1kmvp/config.h"
#include "or1kmvp/system.h"

void exit_usage() {
#define PRINT(...) fprintf(stderr, ##__VA_ARGS__)
    PRINT("Usage: soc [optional arguments]\n");
    PRINT("  -d | --debug          Switch on debug logging\n");
    PRINT("  -t | --trace          Switch on trace logging\n");
    PRINT("  -f | --file   <file>  Read config from <file>\n");
    PRINT("  -c | --config <x>=<y> Set property <x> to value <y>\n");
    PRINT("  -h | --help           Prints this message\n");
#undef PRINT
    exit(EXIT_FAILURE);
}

extern "C" int sc_main(int argc, char** argv)  try {
    vcml::report::report_segfaults();
    vcml::log_level log_level = vcml::LOG_INFO;
    std::string config;

    // parse command line
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--debug") == 0) ||
            (strcmp(argv[i],      "-d") == 0)) {
            log_level = vcml::LOG_DEBUG;
        } else if ((strcmp(argv[i], "--trace") == 0) ||
                   (strcmp(argv[i],      "-t") == 0)) {
            log_level = vcml::LOG_TRACE;
        } else if ((strcmp(argv[i], "--file") == 0) ||
                   (strcmp(argv[i],     "-f") == 0)) {
            if (++i < argc)
                config = argv[i];
            else
                exit_usage();
        } else if ((strcmp(argv[i], "--help") == 0) ||
                   (strcmp(argv[i],     "-h") == 0)) {
            exit_usage();
        }
    }

    vcml::log_term logger;
    logger.set_level(vcml::LOG_ERROR, log_level);
    logger.set_colors();

    vcml::property_provider_arg  provider_arg(argc, argv);
    vcml::property_provider_env  provider_env;
    vcml::property_provider_file provider_file(config);

#ifdef NDEBUG
    // disable deprecated warning for release builds
    sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",
                                            sc_core::SC_DO_NOTHING);
#endif

    or1kmvp::system system("system");
    system.construct();
    system.run();
    system.log_timing_stats();
    return EXIT_SUCCESS;

} catch (vcml::report& r) {
    vcml::logger::log(r);
    return EXIT_FAILURE;
} catch (std::exception& e) {
    vcml::log_error(e.what());
    return EXIT_FAILURE;
}
