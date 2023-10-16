#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>

const char* serverAddress = "127.0.0.1";
const int serverPort = 4444;

const char* appName = "localvuln";
const char* dataFileName = "userdata.txt";

// send credentials 
bool sendCredentials(const std::string& username, const std::string& password, std::string& response) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return false;
    }

    struct hostent* server = gethostbyname(serverAddress);
    if (!server) {
        std::cerr << "Error: Could not resolve host." << std::endl;
        close(sockfd);
        return false;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: Could not connect to the server." << std::endl;
        close(sockfd);
        return false;
    }

    std::string message = username + " " + password;
    if (send(sockfd, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Error sending data to the server." << std::endl;
        close(sockfd);
        return false;
    }

    char buffer[256];
    int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n >= 0) {
        buffer[n] = '\0';
        response = buffer;
        close(sockfd);
        return true;
    } else {
        std::cerr << "Error: Server did not respond." << std::endl;
        close(sockfd);
        return false;
    }
}

int main() {
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        std::cerr << "Error: HOME environment variable not set." << std::endl;
        return 1;
    }

    std::string username, password, response;

    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    if (sendCredentials(username, password, response)) {
        if (response == "Invalid creds") {
            std::cerr << "Invalid credentials. Please try again." << std::endl;
        } else {
            std::string directoryPath = std::string(homeDir) + "/.local/share/" + appName;
            std::string filePath = directoryPath + "/" + dataFileName;

            if (mkdir(directoryPath.c_str(), 0777) == -1) {
                // Check if the directory already exists
                if (errno != EEXIST) {
                    std::cerr << "Error: Failed to create directory." << std::endl;
                    return 1;
                }
            }

            std::ofstream dataFile(filePath);

            if (dataFile.is_open()) {
                dataFile << response << std::endl;
                dataFile.close();
                std::cout << "Response saved to " << filePath << std::endl;
            }
        }
    } else {
        std::cerr << "Error communicating with the server. Please try again." << std::endl;
    }

    return 0;
}
