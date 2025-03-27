#include <iostream>
#include <cstring>
#include <cstdlib>

#ifdef __linux__
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#elif _WIN32
    #include <winsock2.h>
    #include <windows.h>
#endif

// Platform-specific initialization and cleanup
#ifdef __linux__
    #define PLATFORM_INIT()   (void)0
    #define PLATFORM_CLEANUP() (void)0
#elif _WIN32
    #define PLATFORM_INIT()    \
        WSADATA wsaData;       \
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #define PLATFORM_CLEANUP() WSACleanup();
#endif

// Function to set IP address (platform-specific)
void SetIPAddress(const std::string& interface, const std::string& ipAddress)
{
    std::string command;
    
    #ifdef __linux__
        command = "sudo ifconfig " + interface + " " + ipAddress;
    #elif _WIN32
        command = "netsh interface ip set address name=\"" + interface + "\" static " + ipAddress + " 255.255.255.0";
    #else
        std::cerr << "Unsupported platform" << std::endl;
        return;
    #endif

    if (system(command.c_str()) != 0)
    {
        std::cerr << "Error setting IP address" << std::endl;
        exit(1);
    }
}

void SendUDPPacket(const std::string& server_ip, int port)
{
    // Create socket
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // Send message
    const char* message = "Hello, Device!";
    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cerr << "Error sending message" << std::endl;
        close(sockfd);
        return;
    }

    std::cout << "UDP packet sent to " << server_ip << std::endl;
    close(sockfd);
}

void ReceiveUDPPacket(int port)
{
    // Create socket
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Bind the socket to the port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        close(sockfd);
        exit(1);
    }

    len = sizeof(cliaddr);
    int n = recvfrom(sockfd, (char*)buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
    if (n < 0)
    {
        std::cerr << "Error receiving message" << std::endl;
        close(sockfd);
        return;
    }

    buffer[n] = '\0';
    std::cout << "Received: " << buffer << std::endl;
    close(sockfd);
}

int main()
{
    PLATFORM_INIT();

    // Set the IP address of the network interface (Linux specific)
    SetIPAddress("enx94103eb7e201", "10.0.0.200");  // Set IP on 'enx94103eb7e201'

    // Send UDP packet to device at 10.0.0.128
    SendUDPPacket("10.0.0.128", 12345);

    //// Optionally, receive a UDP packet
    //ReceiveUDPPacket(12345);

    PLATFORM_CLEANUP();

    return 0;
}


