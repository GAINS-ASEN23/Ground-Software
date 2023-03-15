/* MAIN BACKEND HEADER FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main header file for the Orbital Trajectory Estimation Software backend.
*/
#ifndef MAIN_H
#define MAIN_H

#include "SPICEQuery.h"
#include "N_Body_Simulation.h"

#include "boost/array.hpp"
#include "boost/asio.hpp"
#include <ctime>
//#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>

// Example Data
std::string dateEx = { "2022 October 01, 13:00:00 PST" };				// Date
Eigen::RowVector3d R_scM{ 1.7633e6/1000, 2.9656e5/1000, 0 };						// Initial Position Relative to Moon
Eigen::RowVector3d V_scM{ -274.68/1000, 1.6323e3/1000, 0 };						// Initial Velocity Relative to Moon


// Comms setup
#define IPADDRESS "127.0.0.1" // "192.168.1.64" //ipaddress to send UDP message to
#define UDP_PORT 13251 //UDP Port to send UDP message to

#define IPADDRESS_Teensy "169.254.64.233" //"192.168.1.177" // Teensy self defined  IpAddress
#define UDP_PORT_Teensy 8888 // Teensy self defined port

using boost::asio::ip::udp;
using boost::asio::ip::address;

void Sender(std::string in) {
    // this function sends a udp message to a specific ipaddress and port

    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(IPADDRESS_Teensy), UDP_PORT_Teensy);
    socket.open(udp::v4()); //opens a socket using the IPv4 protocol

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(in), remote_endpoint, 0, err); // "boost::asio::buffer" function takes a string as input and writes it to a boost buffer
    //"send_to" is used to send the buffer to the remote endpoint
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

struct Client {

    boost::asio::io_service io_service;
    udp::socket socket{ io_service };
    boost::array<char, 1024> recv_buffer;
    udp::endpoint remote_endpoint;

    int count = 1; // will wait for this many messages before allowing the main program to roll on

    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        std::cout << "Received: '" << std::string(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred) << "' (" << error.message() << ")\n";

        if (--count > 0) {
            std::cout << "Count: " << count << "\n";
            wait();
        }
    }

    void wait() {
        //socket.async_receive_from(boost::asio::buffer(recv_buffer),
        //    remote_endpoint,
        //    boost::bind(&Client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        
        boost::system::error_code error;
        size_t bytes_transferred = socket.receive_from(boost::asio::buffer(recv_buffer), remote_endpoint, 0, error);
        handle_receive(error, bytes_transferred);

        //synchronous attempt
        //socket.receive(boost::asio::buffer(recv_buffer));
        //std::cout << "Received: '" << std::string(recv_buffer.begin(), recv_buffer.begin()) << "\n";
        // synchronous version of this function - will likely need more code changes than just this to implement
    }

    void Receiver()
    {
        socket.open(udp::v4());
        socket.bind(udp::endpoint(address::from_string(IPADDRESS), UDP_PORT));

        wait();

        std::cout << "Receiving\n";
        io_service.run();
        std::cout << "Receiver exit\n";
    }
};

// end comms setup

#endif