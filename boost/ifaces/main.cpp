#include <boost/asio.hpp>
#include <iostream>

void list_network_interfaces()
{
    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
        boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
        
        std::cout << "Network Interfaces:\n";

        while (it != boost::asio::ip::tcp::resolver::iterator())
        {
            boost::asio::ip::tcp::endpoint endpoint = *it++;
            std::cout << " - " << endpoint.address().to_string() << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main()
{
    list_network_interfaces();
    return 0;
}

