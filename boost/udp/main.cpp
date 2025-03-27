#include <boost/asio.hpp>
#include <iostream>
#include <stdio.h>

#define     UDP_STAT_CON        9
#define     UDP_STAT_REQ        10

using boost::asio::ip::udp;

int main()
{
    std::string server_ip = "16.0.0.128";  // Replace with the server's IP
    int port = 1234;

    // setup tx socket
    boost::asio::io_service tx_io_service;
    udp::socket tx_socket(tx_io_service);
    udp::endpoint device_endpoint(boost::asio::ip::address::from_string(server_ip), port);
    tx_socket.open(udp::v4());

    // setup rx socket
    boost::asio::io_service rx_io_service;
    udp::socket rx_socket(rx_io_service, udp::endpoint(udp::v4(), port));
    std::cout << "UDP server listening on port " << port << "..." << std::endl;


    // send packet
    char txbuf[2048];
    txbuf[0] = 0xaa; txbuf[1] = 0xbb; txbuf[2] = 0xcc; txbuf[3] = UDP_STAT_REQ;
    tx_socket.send_to(boost::asio::buffer(std::string(txbuf,4)), device_endpoint);

    // receive packets
    char rxbuf[1024];
    udp::endpoint remote_endpoint;
    uint8_t fpga_source;
    do {

        size_t length = rx_socket.receive_from(boost::asio::buffer(rxbuf), remote_endpoint);
        fpga_source = rxbuf[3];

    } while (fpga_source != UDP_STAT_CON);


    printf("fpga_source = 0x%02x", fpga_source);
    std::cout << " from " << remote_endpoint << std::endl;

    return 0;
}


