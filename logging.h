//
// Created by Phillip Romig on 7/15/24.
//

#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <filesystem>
#include <string>

#ifndef __FILE_NAME__
#define __FILE_NAME__ std::filesystem::path(__FILE__).filename().string()
#endif

inline int LOG_LEVEL = 3;

#define TRACE   do { if (LOG_LEVEL > 5) { std::cerr << "TRACE: "; } } while(0)
#define DEBUG   do { if (LOG_LEVEL > 4) { std::cerr << "DEBUG: "; } } while(0)
#define INFO    do { if (LOG_LEVEL > 3) { std::cerr << "INFO: "; } } while(0)
#define WARNING do { if (LOG_LEVEL > 2) { std::cerr << "WARNING: "; } } while(0)
#define ERROR   do { if (LOG_LEVEL > 1) { std::cerr << "ERROR: "; } } while(0)
#define FATAL   do { if (LOG_LEVEL > 0) { std::cerr << "FATAL: "; } } while(0)
#define ENDL  " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;

#endif // LOGGING_H
