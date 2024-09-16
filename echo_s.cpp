// **************************************************************************************
// * Echo Strings (echo_s.cpp)
// * -- Accepts TCP connections and then echos back each string sent.
// **************************************************************************************
#include "echo_s.h"
#include <algorithm> // For removing newline

// **************************************************************************************
// * processConnection()
// * - Handles reading the line from the network and sending it back to the client.
// * - Returns true if the client sends "QUIT" command, false if the client sends "CLOSE".
// **************************************************************************************
int processConnection(int sockFd) {

    bool quitProgram = false;
    bool keepGoing = true;
    char buffer[1024];

    while (keepGoing) {
        // First, we clean the buffer before each lecture
        memset(buffer, 0, sizeof(buffer));

        // Reading data from the client
        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            log_error("Error reading the socket or connection closed.", __FILE__, __LINE__);
            break;
        }

        // Remove any newline characters from the buffer
        std::string data(buffer);
        data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());

        log_debug("Received " + std::to_string(bytesRead) + " bytes, containing the string \"" + data + "\"", __FILE__, __LINE__);

        // Handling "CLOSE" - "QUIT" commands
        if (data.find("CLOSE") == 0) {
            log_debug("CLOSE command found.", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            log_debug("Wrote " + std::to_string(bytesRead) + " bytes back to client.", __FILE__, __LINE__);

            keepGoing = false;
        } else if (data.find("QUIT") == 0) {
            log_debug("QUIT command found.", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            log_debug("Wrote " + std::to_string(bytesRead) + " bytes back to client.", __FILE__, __LINE__);

            quitProgram = true;
            keepGoing = false;
        } else {
            log_debug("CALLING WRITE(" + std::to_string(sockFd) + ")", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            log_debug("Wrote " + std::to_string(bytesRead) + " bytes back to client.", __FILE__, __LINE__);
        }
    }

    return quitProgram;
}

// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connections until processConnection() returns 1
// **************************************************************************************

int main (int argc, char *argv[]) {

    // ********************************************************************
    // * Process the command line arguments
    // ********************************************************************
    int opt = 0;
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                LOG_LEVEL = std::stoi(optarg);
                if (LOG_LEVEL >= 5) {
                    log_info("LOG level established at: " + std::to_string(LOG_LEVEL), __FILE__, __LINE__);
                }
                break;
            default:
                std::cout << "usage: " << argv[0] << " -d <num>" << std::endl;
                exit(-1);
        }
    }

    // *******************************************************************
    // * Creating the initial socket.
    // ********************************************************************
    log_debug("Calling socket() assigned file descriptor", __FILE__, __LINE__);
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        log_fatal("Error creating the socket.", __FILE__, __LINE__);
        exit(1);
    }
    log_debug("Socket created with file descriptor: " + std::to_string(listenFd), __FILE__, __LINE__);

    // *******************************************************************
    // * Configuring the initial Address and the Port of the server.
    // ********************************************************************
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Selecting port number
    int port = 2024;
    servaddr.sin_port = htons(port);

    // ********************************************************************
    // * Linking Socket with the Port.
    // ********************************************************************
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

    // ********************************************************************
    // * Setting the socket to the listening state.
    // ********************************************************************
    log_debug("Calling listen()...", __FILE__, __LINE__);
    if (listen(listenFd, 5) < 0) {
        log_fatal("Error setting the socket in listening state.", __FILE__, __LINE__);
        exit(1);
    }

    // ********************************************************************
    // * Accepting connections
    // ********************************************************************
    bool quitProgram = false;
    while (!quitProgram) {
        log_debug("Calling accept()...", __FILE__, __LINE__);
        int connFd = accept(listenFd, (struct sockaddr*)nullptr, nullptr);
        if (connFd < 0) {
            log_error("Error accepting connection.", __FILE__, __LINE__);
            exit(1);
        }

        log_info("Received a connection on " + std::to_string(connFd), __FILE__, __LINE__);

        quitProgram = processConnection(connFd);

        close(connFd);
    }

    close(listenFd);

    return 0;
}
