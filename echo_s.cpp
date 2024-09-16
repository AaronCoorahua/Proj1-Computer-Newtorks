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
        logDebug("Calling read(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", 1024)");

        // Reading data from the client
        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            logError("Error reading the socket or connection closed.");
            break;
        }

        // Debug showing data received
        logDebug("Received " + std::to_string(bytesRead) + " bytes, containing the string\n\"" + std::string(buffer) + "\".");

        std::string data(buffer);

        // Handling "CLOSE" - "QUIT" commands
        if (data.find("CLOSE") == 0) {
            logDebug("CLOSE command found.");
            logDebug("Calling write(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", " + std::to_string(bytesRead) + ")");
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.");

            keepGoing = false;
        } else if (data.find("QUIT") == 0) {
            logDebug("QUIT command found.");
            logDebug("Calling write(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", " + std::to_string(bytesRead) + ")");
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.");

            quitProgram = true;
            keepGoing = false;
        } else {
            logDebug("Calling write(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", " + std::to_string(bytesRead) + ")");
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.");
        }
    }

    return quitProgram;
}

// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection() returns 1
// **************************************************************************************

int main(int argc, char *argv[]) {

    // ********************************************************************
    // * Process the command line arguments
    // ********************************************************************
    int opt = 0;
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                LOG_LEVEL = std::stoi(optarg);
                logInfo("LOG level established at: " + std::to_string(LOG_LEVEL));
                break;
            default:
                std::cout << "usage: " << argv[0] << " -d <num>" << std::endl;
                exit(-1);
        }
    }

    // *******************************************************************
    // * Creating the initial socket.
    // ********************************************************************
    logDebug("Calling socket() assigned file descriptor ");
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        logFatal("Error al crear el socket.");
        exit(1);
    }
    logDebug("Socket FD: " + std::to_string(listenFd));

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
    logDebug("Calling bind()...");
    bool bindSuccesful = false;
    while (!bindSuccesful) {
        if (bind(listenFd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            logWarning("Port number " + std::to_string(port) + " is not available. Trying another...");
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
    logDebug("CALLING LISTEN()...");
    if (listen(listenFd, 5) < 0) {
        logFatal("Error setting the socket in listening state.");
        exit(1);
    }

    // ********************************************************************
    // * Accepting connections
    // ********************************************************************
    bool quitProgram = false;
    while (!quitProgram) {
        logDebug("CALLING ACCEPT(" + std::to_string(listenFd) + "NULL,NULL)");
        int connFd = accept(listenFd, (struct sockaddr*)nullptr, nullptr);
        if (connFd < 0) {
            logError("Error accepting connection.");
            exit(1);
        }

        logInfo("The program received a connection on FD: " + std::to_string(connFd));

        quitProgram = processConnection(connFd);

        close(connFd);
    }

    close(listenFd);

    return 0;
}

