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
        // Clear the buffer before each read
        memset(buffer, 0, sizeof(buffer));

        // Debug when waiting for new data
        logDebug("Calling read", __FILE__, __LINE__);

        // Reading data from the client
        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            logError("Error reading the socket or connection closed.", __FILE__, __LINE__);
            break;
        }

        // Clean the buffer to remove newline characters or unwanted characters
        std::string cleanBuffer(buffer);
        cleanBuffer.erase(std::remove_if(cleanBuffer.begin(), cleanBuffer.end(), [](char c){ return c == '\n'; }), cleanBuffer.end());

        // Debug showing data received
        logDebug("Received " + std::to_string(bytesRead) + " bytes, containing the string \"" + cleanBuffer + "\"", __FILE__, __LINE__);

        // Handling "CLOSE" - "QUIT" commands
        if (cleanBuffer.find("CLOSE") == 0) {
            logDebug("CLOSE command found.", __FILE__, __LINE__);
            logDebug("Calling write", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
            keepGoing = false;
        } else if (cleanBuffer.find("QUIT") == 0) {
            logDebug("QUIT command found.", __FILE__, __LINE__);
            logDebug("Calling write", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
            quitProgram = true;
            keepGoing = false;
        } else {
            logDebug("Calling write", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
        }
    }

    return quitProgram;
}

// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection() returns 1
// **************************************************************************************

int main(int argc, char *argv[]) {

    // Process the command line arguments
    int opt = 0;
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                LOG_LEVEL = std::stoi(optarg);
                logInfo("LOG level established at: " + std::to_string(LOG_LEVEL), __FILE__, __LINE__);
                break;
            default:
                std::cout << "usage: " << argv[0] << " -d <num>" << std::endl;
                exit(-1);
        }
    }

    // Creating the initial socket
    logDebug("Calling socket() assigned file descriptor", __FILE__, __LINE__);
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        logFatal("Error creating the socket.", __FILE__, __LINE__);
        exit(1);
    }
    logDebug("File descriptor assigned: " + std::to_string(listenFd), __FILE__, __LINE__);

    // Configuring the initial Address and the Port of the server
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Selecting port number
    int port = 2024;
    servaddr.sin_port = htons(port);

    // Linking Socket with the Port
    logDebug("Calling bind()...", __FILE__, __LINE__);
    bool bindSuccesful = false;
    while (!bindSuccesful) {
        if (bind(listenFd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            logWarning("Port number " + std::to_string(port) + " is not available. Trying another...", __FILE__, __LINE__);
            port++;
            servaddr.sin_port = htons(port);
        } else {
            bindSuccesful = true;
        }
    }
    std::cout << "Using port: " << port << std::endl;

    // Setting the socket to the listening state
    logDebug("Calling listen()...", __FILE__, __LINE__);
    if (listen(listenFd, 5) < 0) {
        logFatal("Error setting the socket in listening state.", __FILE__, __LINE__);
        exit(1);
    }

    // Accepting connections
    bool quitProgram = false;
    while (!quitProgram) {
        logDebug("Calling accept()", __FILE__, __LINE__);
        int connFd = accept(listenFd, (struct sockaddr*)nullptr, nullptr);
        if (connFd < 0) {
            logError("Error accepting connection.", __FILE__, __LINE__);
            exit(1);
        }

        logInfo("The program received a connection on " + std::to_string(connFd), __FILE__, __LINE__);

        quitProgram = processConnection(connFd);

        close(connFd);
    }

    close(listenFd);

    return 0;
}
