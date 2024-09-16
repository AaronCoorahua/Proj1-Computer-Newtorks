#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <filesystem>
#include <string>

#ifndef __FILE_NAME__
#define __FILE_NAME__ std::filesystem::path(__FILE__).filename().string()
#endif

inline int LOG_LEVEL = 3;

// Logging functions
inline void logTrace(const std::string& message) {
    if (LOG_LEVEL > 5) {
        std::cerr << "TRACE: " << message << " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;
    }
}

inline void logDebug(const std::string& message) {
    if (LOG_LEVEL > 4) {
        std::cerr << "DEBUG: " << message << " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;
    }
}

inline void logInfo(const std::string& message) {
    if (LOG_LEVEL > 3) {
        std::cerr << "INFO: " << message << " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;
    }
}

inline void logWarning(const std::string& message) {
    if (LOG_LEVEL > 2) {
        std::cerr << "WARNING: " << message << " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;
    }
}

inline void logError(const std::string& message) {
    if (LOG_LEVEL > 1) {
        std::cerr << "ERROR: " << message << " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;
    }
}

inline void logFatal(const std::string& message) {
    if (LOG_LEVEL > 0) {
        std::cerr << "FATAL: " << message << " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl;
    }
}

#endif // LOGGING_H

