#include "echo_s.h"
#include "logging.h" // Include logging.h for log handling

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

        // Debug when waiting for new data
        DEBUG std::cerr << "Calling read(" << sockFd << ", " << static_cast<void*>(buffer) << ", 1024)" << ENDL;

        // Reading data from the client
        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            ERROR std::cerr << "Error reading the socket or connection closed. (echo_s.cpp:" << __LINE__ << ")" << ENDL;
            break;
        }

        // Debug showing data received
        DEBUG std::cerr << "Received " << bytesRead << " bytes, containing the string\n\"" << std::string(buffer) << "\". (echo_s.cpp:" << __LINE__ << ")" << ENDL;

        std::string data(buffer);

        // Handling "CLOSE" - "QUIT" commands
        if (data.find("CLOSE") == 0) {
            DEBUG std::cerr << "CLOSE command found. (echo_s.cpp:" << __LINE__ << ")" << ENDL;
            DEBUG std::cerr << "Calling write(" << sockFd << ", " << static_cast<void*>(buffer) << ", " << bytesRead << ")" << ENDL;
            write(sockFd, buffer, bytesRead);
            DEBUG std::cerr << "Wrote " << bytesRead << " back to client. (echo_s.cpp:" << __LINE__ << ")" << ENDL;

            keepGoing = false;
        } else if (data.find("QUIT") == 0) {
            DEBUG std::cerr << "QUIT command found. (echo_s.cpp:" << __LINE__ << ")" << ENDL;
            DEBUG std::cerr << "Calling write(" << sockFd << ", " << static_cast<void*>(buffer) << ", " << bytesRead << ")" << ENDL;
            write(sockFd, buffer, bytesRead);
            DEBUG std::cerr << "Wrote " << bytesRead << " back to client. (echo_s.cpp:" << __LINE__ << ")" << ENDL;

            quitProgram = true;
            keepGoing = false;
        } else {
            DEBUG std::cerr << "Calling write(" << sockFd << ", " << static_cast<void*>(buffer) << ", " << bytesRead << ")" << ENDL;
            write(sockFd, buffer, bytesRead);
            DEBUG std::cerr << "Wrote " << bytesRead << " back to client. (echo_s.cpp:" << __LINE__ << ")" << ENDL;
        }
    }

    return quitProgram;
}

// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection() returns 1
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
                INFO std::cerr << "LOG level established at: " << LOG_LEVEL << ENDL;
                break;
            default:
                std::cout << "usage: " << argv[0] << " -d <num>" << std::endl;
                exit(-1);
        }
    }

    // *******************************************************************
    // * Creating the initial socket.
    // ********************************************************************
    DEBUG std::cerr << "Calling socket() assigned file descriptor ";
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        FATAL std::cerr << "Error al crear el socket. (echo_s.cpp: " << __LINE__ << ")" << ENDL;
        exit(1);
    }
    DEBUG std::cerr << listenFd << " (echo_s.cpp:" << __LINE__ << ")" << ENDL;

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
    DEBUG std::cerr << "Calling bind()... (echo_s.cpp:" << __LINE__ << ")" << ENDL;
    bool bindSuccesful = false;
    while (!bindSuccesful) {
        if (bind(listenFd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            WARNING std::cerr << "Port number " << port << " is not available. Trying another... (echo_s.cpp:" << __LINE__ << ")" << ENDL;
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
    DEBUG std::cerr << "CALLING LISTEN()... (echo_s.cpp:" << __LINE__ << ")" << ENDL;
    if (listen(listenFd, 5) < 0) {
        FATAL std::cerr << "Error setting the socket in listening state. (echo_s.cpp:" << __LINE__ << ")" << ENDL;
        exit(1);
    }

    // ********************************************************************
    // * Accepting connections
    // ********************************************************************
    bool quitProgram = false;
    while (!quitProgram) {
        DEBUG std::cerr << "CALLING ACCEPT(" << listenFd << "NULL,NULL). (echo_s.cpp:" << __LINE__ << ")" << ENDL;
        int connFd = accept(listenFd, (struct sockaddr*)nullptr, nullptr);
        if (connFd < 0) {
            ERROR std::cerr << "Error accepting connection. (echo_s.cpp:" << __LINE__ << ")" << ENDL;
            exit(1);
        }

        INFO std::cerr << "The program received a connection on " << connFd << " (echo_s.cpp:" << __LINE__ << ")" << ENDL;

        quitProgram = processConnection(connFd);

        close(connFd);
    }

    close(listenFd);

    return 0;
}

