#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <filesystem>
#include <string>

#ifndef __FILE_NAME__
#define __FILE_NAME__ std::filesystem::path(__FILE__).filename().string()
#endif

inline int LOG_LEVEL = 3;

void log_debug(const std::string& msg, const std::string& file, int line) {
    if (LOG_LEVEL >= 5) {
        std::cerr << "DEBUG: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void log_info(const std::string& msg, const std::string& file, int line) {
    if (LOG_LEVEL >= 3) {
        std::cerr << "INFO: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void log_warning(const std::string& msg, const std::string& file, int line) {
    if (LOG_LEVEL >= 2) {
        std::cerr << "WARNING: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void log_error(const std::string& msg, const std::string& file, int line) {
    if (LOG_LEVEL >= 1) {
        std::cerr << "ERROR: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

void log_fatal(const std::string& msg, const std::string& file, int line) {
    if (LOG_LEVEL >= 0) {
        std::cerr << "FATAL: " << msg << " (" << file << ":" << line << ")" << std::endl;
    }
}

#endif // LOGGING_H
