#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <filesystem>
#include <string>
#include <iomanip>

inline int LOG_LEVEL = 3;

// Logging functions with line and file parameters
inline void logTrace(const std::string& message, const char* file, int line) {
    if (LOG_LEVEL > 5) {
        std::cerr << "TRACE: " << message << " (" << file << ":" << line << ")" << std::endl;
    }
}

inline void logDebug(const std::string& message, const char* file, int line) {
    if (LOG_LEVEL > 4) {
        std::cerr << "DEBUG: " << message << " (" << file << ":" << line << ")" << std::endl;
    }
}

inline void logInfo(const std::string& message, const char* file, int line) {
    if (LOG_LEVEL > 3) {
        std::cerr << "INFO: " << message << " (" << file << ":" << line << ")" << std::endl;
    }
}

inline void logWarning(const std::string& message, const char* file, int line) {
    if (LOG_LEVEL > 2) {
        std::cerr << "WARNING: " << message << " (" << file << ":" << line << ")" << std::endl;
    }
}

inline void logError(const std::string& message, const char* file, int line) {
    if (LOG_LEVEL > 1) {
        std::cerr << "ERROR: " << message << " (" << file << ":" << line << ")" << std::endl;
    }
}

inline void logFatal(const std::string& message, const char* file, int line) {
    if (LOG_LEVEL > 0) {
        std::cerr << "FATAL: " << message << " (" << file << ":" << line << ")" << std::endl;
    }
}

#endif // LOGGING_H

