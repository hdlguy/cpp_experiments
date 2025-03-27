#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::udp;

void udp_client(const std::string& server_ip, int port)
{
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint receiver_endpoint(boost::asio::ip::address::from_string(server_ip), port);

    std::string message = "Hello, UDP Server!";
    socket.open(udp::v4());

    socket.send_to(boost::asio::buffer(message), receiver_endpoint);
    std::cout << "Sent message to server: " << message << std::endl;
}

int main()
{
    std::string server_ip = "16.0.0.128";  // Replace with the server's IP
    int port = 1234;
    udp_client(server_ip, port);
    return 0;
}


