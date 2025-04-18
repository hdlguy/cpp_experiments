#include <iostream>
#include <boost/asio.hpp>

#ifdef __linux__
    #include <ifaddrs.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#elif _WIN32
    #include <winsock2.h>
    #include <iphlpapi.h>
    #pragma comment(lib, "iphlpapi.lib")
#endif


void LoadNetworkInterfaces() {
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) == -1) {
        std::cout << "Error Fetching interfaces\n";
        return;
    }

    for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && (ifa->ifa_flags & IFF_UP)) {
            char ip[INET_ADDRSTRLEN] = {0};
            struct sockaddr_in* sa = (struct sockaddr_in*)ifa->ifa_addr;
            inet_ntop(AF_INET, &sa->sin_addr, ip, INET_ADDRSTRLEN);
            std::cout << ifa->ifa_name << "(" << ip << ")\n";
        }
    }
    freeifaddrs(ifaddr);
}


int main()
{
    LoadNetworkInterfaces();

    return 0;
}

/*
*/
