#include "echo_s.h"
#include "logging.h"

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
        memset(buffer, 0, sizeof(buffer));

        // Debug when waiting for new data
        logDebug("Calling read(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", 1024)", __FILE__, __LINE__);

        int bytesRead = read(sockFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            logError("Error reading the socket or connection closed.", __FILE__, __LINE__);
            break;
        }

        // Debug showing data received, removing extra newlines from buffer
        std::string cleanBuffer = std::string(buffer);
        cleanBuffer.erase(std::remove(cleanBuffer.begin(), cleanBuffer.end(), '\n'), cleanBuffer.end());
        logDebug("Received " + std::to_string(bytesRead) + " bytes, containing the string \"" + cleanBuffer + "\".", __FILE__, __LINE__);

        std::string data(buffer);

        if (data.find("CLOSE") == 0) {
            logDebug("CLOSE command found.", __FILE__, __LINE__);
            logDebug("Calling write(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", " + std::to_string(bytesRead) + ")", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
            keepGoing = false;
        } else if (data.find("QUIT") == 0) {
            logDebug("QUIT command found.", __FILE__, __LINE__);
            logDebug("Calling write(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", " + std::to_string(bytesRead) + ")", __FILE__, __LINE__);
            write(sockFd, buffer, bytesRead);
            logDebug("Wrote " + std::to_string(bytesRead) + " back to client.", __FILE__, __LINE__);
            quitProgram = true;
            keepGoing = false;
        } else {
            logDebug("Calling write(" + std::to_string(sockFd) + ", " + std::to_string(reinterpret_cast<uintptr_t>(buffer)) + ", " + std::to_string(bytesRead) + ")", __FILE__, __LINE__);
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

    logDebug("Calling socket() assigned file descriptor ", __FILE__, __LINE__);
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        logFatal("Error al crear el socket.", __FILE__, __LINE__);
        exit(1);
    }
    logDebug("Socket FD: " + std::to_string(listenFd), __FILE__, __LINE__);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    int port = 2024;
    servaddr.sin_port = htons(port);

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

    logDebug("CALLING LISTEN()...", __FILE__, __LINE__);
    if (listen(listenFd, 5) < 0) {
        logFatal("Error setting the socket in listening state.", __FILE__, __LINE__);
        exit(1);
    }

    bool quitProgram = false;
    while (!quitProgram) {
        logDebug("CALLING ACCEPT(" + std::to_string(listenFd) + "NULL,NULL)", __FILE__, __LINE__);
        int connFd = accept(listenFd, (struct sockaddr*)nullptr, nullptr);
        if (connFd < 0) {
            logError("Error accepting connection.", __FILE__, __LINE__);
            exit(1);
        }

        logInfo("The program received a connection on FD: " + std::to_string(connFd), __FILE__, __LINE__);

        quitProgram = processConnection(connFd);

        close(connFd);
    }

    close(listenFd);

    return 0;
}

