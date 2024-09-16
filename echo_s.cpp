#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <algorithm>

int LOG_LEVEL = 5; // Default log level, modify it based on the -d flag

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

int processConnection(int sockFd) {
    bool quitProgram = false;
    bool keepGoing = true;
    char buffer[1024];

    while (keepGoing) {
        memset(buffer, 0, sizeof(buffer));

        log_debug("CALLING READ(" + std::to_string(sockFd) + ", 1024)", __FILE__, __LINE__);

        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            log_error("Error reading the socket or connection closed.", __FILE__, __LINE__);
            break;
        }

        log_debug("Received " + std::to_string(bytesRead) + " bytes, containing the string \"" + std::string(buffer) + "\"", __FILE__, __LINE__);

        std::string data(buffer);

        if (data.find("CLOSE") == 0) {
            log_debug("CLOSE command found.", __FILE__, __LINE__);
            log_debug("CALLING WRITE(" + std::to_string(sockFd) + ", " + std::to_string(bytesRead) + ")", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            log_debug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
            keepGoing = false;
        } else if (data.find("QUIT") == 0) {
            log_debug("QUIT command found.", __FILE__, __LINE__);
            log_debug("CALLING WRITE(" + std::to_string(sockFd) + ", " + std::to_string(bytesRead) + ")", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            log_debug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
            quitProgram = true;
            keepGoing = false;
        } else {
            log_debug("CALLING WRITE(" + std::to_string(sockFd) + ", " + std::to_string(bytesRead) + ")", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            log_debug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
        }
    }

    return quitProgram;
}

int main(int argc, char *argv[]) {
    int opt = 0;
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                LOG_LEVEL = std::stoi(optarg);
                log_info("LOG level established at: " + std::to_string(LOG_LEVEL), __FILE__, __LINE__);
                break;
            default:
                std::cout << "usage: " << argv[0] << " -d <num>" << std::endl;
                exit(-1);
        }
    }

    log_debug("Calling socket() assigned file descriptor", __FILE__, __LINE__);
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        log_fatal("Error al crear el socket.", __FILE__, __LINE__);
        exit(1);
    }

    log_debug(std::to_string(listenFd) + " assigned as the file descriptor.", __FILE__, __LINE__);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int port = 2024;
    servaddr.sin_port = htons(port);

    log_debug("Calling bind()...", __FILE__, __LINE__);
    bool bindSuccesful = false;
    while (!bindSuccesful) {
        if (bind(listenFd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            log_warning("Port number " + std::to_string(port) + " is not available. Trying another...", __FILE__, __LINE__);
            port++;
            servaddr.sin_port = htons(port);
        } else {
            bindSuccesful = true;
        }
    }
    std::cout << "Using port: " << port << std::endl;

    log_debug("CALLING LISTEN()...", __FILE__, __LINE__);
    if (listen(listenFd, 5) < 0) {
        log_fatal("Error setting the socket in listening state.", __FILE__, __LINE__);
        exit(1);
    }

    bool quitProgram = false;
    while (!quitProgram) {
        log_debug("CALLING ACCEPT(" + std::to_string(listenFd) + ", NULL, NULL).", __FILE__, __LINE__);
        int connFd = accept(listenFd, (struct sockaddr*)nullptr, nullptr);
        if (connFd < 0) {
            log_error("Error accepting connection.", __FILE__, __LINE__);
            exit(1);
        }

        log_info("The program received a connection on " + std::to_string(connFd), __FILE__, __LINE__);

        quitProgram = processConnection(connFd);

        close(connFd);
    }

    close(listenFd);
    return 0;
}
