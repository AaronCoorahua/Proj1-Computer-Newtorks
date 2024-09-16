#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <string>

inline int LOG_LEVEL = 3;

void logTrace(const std::string &msg, const char *file, int line) {
    if (LOG_LEVEL > 5) {
        std::cerr << "TRACE: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void logDebug(const std::string &msg, const char *file, int line) {
    if (LOG_LEVEL > 4) {
        std::cerr << "DEBUG: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void logInfo(const std::string &msg, const char *file, int line) {
    if (LOG_LEVEL > 3) {
        std::cerr << "INFO: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void logWarning(const std::string &msg, const char *file, int line) {
    if (LOG_LEVEL > 2) {
        std::cerr << "WARNING: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void logError(const std::string &msg, const char *file, int line) {
    if (LOG_LEVEL > 1) {
        std::cerr << "ERROR: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void logFatal(const std::string &msg, const char *file, int line) {
    if (LOG_LEVEL > 0) {
        std::cerr << "FATAL: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

#endif // LOGGING_H

