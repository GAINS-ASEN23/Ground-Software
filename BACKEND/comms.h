#pragma once
/* Communications Header File
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Jason Popich, Cannon Palmer, Tucker Peyok, Brian Trybus, Derek Popich
* Purpose: This is the header file for the communications and multi-threading functionality
*/

#ifndef COMMS_H
#define COMMS_H

#include <iostream> 
#include <thread> 
#include <mutex>

//#ifdef _WIN32
//    #include <Windows.h>
//#else
//    #include <unistd.h>
//#endif

#include "ccsds.h"

#include "boost/array.hpp"
#include "boost/asio.hpp"
#include <ctime>
//#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>

// Comms setup
#define IPADDRESS "127.0.0.1" // "192.168.1.64" //ipaddress to send UDP message to
#define UDP_PORT 13251 //UDP Port to send UDP message to

#define IPADDRESS_Teensy "169.254.64.233" //"192.168.1.177" // Teensy self defined  IpAddress
#define UDP_PORT_Teensy 8888 // Teensy self defined port

#define IPADDRESS_RECEIVE "192.168.1.25"//"169.254.102.36"
#define UDP_PORT_RECEIVE 8888

using boost::asio::ip::udp;
using boost::asio::ip::address;

void Sender(std::string in, std::string send_ipaddress, int send_port) {
    // this function sends a udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    //udp::endpoint remote_endpoint = udp::endpoint(address::from_string(IPADDRESS_Teensy), UDP_PORT_Teensy);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    //udp::endpoint remote_endpoint = udp::endpoint(address::from_string(IPADDRESS), UDP_PORT); // sends message back to self - for testing purposes only
    socket.open(udp::v4()); //opens a socket using the IPv4 protocol
    //socket.open(remote_endpoint.protocol());

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
    boost::system::error_code receive_error;
    size_t packetReceiveLen;

    int count = 1; // will wait for this many messages before allowing the main program to roll on

    void init(std::string receive_ipaddress, int receive_port) {
        std::cout << "Receiving on ipaddress: " << receive_ipaddress << ", with port: " << receive_port << "\n";
        Client::remote_endpoint = udp::endpoint(address::from_string(receive_ipaddress), receive_port);
        socket.open(remote_endpoint.protocol());
        boost::system::error_code error;
        socket.bind(remote_endpoint, error);
        if (error) {
            std::cout << "Error binding to socket: " << error.message() << "\n";
            return;
        }
    }

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
        boost::system::error_code error;
        size_t bytes_transferred = socket.receive_from(boost::asio::buffer(recv_buffer), remote_endpoint, 0, error);
        if (error) {
            std::cout << "Error in receive_from: " << error.message() << "\n";
            return;
        }
        printf("bytes received = %d \n", int(bytes_transferred));
        handle_receive(error, bytes_transferred);
    }

    void Receiver(std::string receive_ipaddress, int receive_port)
    {

        wait();

        //std::cout << "Receiving\n";
        io_service.run();
        //std::cout << "Receiver exit\n";
    }
};
// end comms setup

// Begin multi threading setup

struct GAINS_TLM_PACKET {
    uint8_t     TlmHeader[CFE_SB_TLM_HDR_SIZE]{ 0 };
    uint8_t     ci_command_error_count{ 0 };
    double      position_x{ 0 };
    double      position_y{ 0 };
    double      position_z{ 0 };
    double      velocity_x{ 0 };
    double      velocity_y{ 0 };
    double      velocity_z{ 0 };
};


class ethernet_data {
public:
    // Default Constructor (Don't use)
    ethernet_data() {};

    // Default Destructor
    ~ethernet_data() {};

    void get_ready_flag(bool& data_ready_flag) {
        data_mut.lock();
        data_ready_flag = ethernet_data::data_ready;
        data_mut.unlock();
    }

    void set_ready_flag(const bool set_data_ready) {
        data_mut.lock();
        ethernet_data::data_ready = set_data_ready;
        data_mut.unlock();
    }

    void get_data(GAINS_TLM_PACKET& tlm_packet) {
        data_mut.lock();
        tlm_packet = ethernet_data::ethernet_data_buf;
        data_mut.unlock();
    }

    void set_data(const GAINS_TLM_PACKET new_packet) {
        data_mut.lock();
        ethernet_data::ethernet_data_buf = new_packet;
        data_mut.unlock();
    }

    void get_ip(std::string& out_ipaddress, int& out_port) {
        data_mut.lock();
        out_ipaddress = ethernet_data::ipaddress;
        out_port = ethernet_data::port;
        data_mut.unlock();
    }

    void set_ip(std::string new_ipaddress, int new_port) {
        data_mut.lock();
        ethernet_data::ipaddress = new_ipaddress;
        ethernet_data::port = new_port;
        printf("Set_ip in ethernet class \n");
        data_mut.unlock();
    }


private:
    std::mutex data_mut;
    bool data_ready{ false };
    GAINS_TLM_PACKET ethernet_data_buf;
    std::string ipaddress{ "0.0.0.0" };
    int port{8888};
};


// Define the class
class ethernet_backend {
public:
    // Overload the operator () by doing operator() and defining what arguments you want to pass through on the next set of parenthesis
    // Which in this case is the pointer to our ethernet data object
    void operator()(ethernet_data* data_obj)
    {
        // Init vars
        bool data_ready_flag = false;
        GAINS_TLM_PACKET local_packet;
        Client client_thread;
        std::string ipaddress;
        int port;

        // Dummy vars
        int i = 0;
        int j = 0;
        int k = 0;
        data_obj->get_ip(ipaddress, port);
        client_thread.init(ipaddress, port);

        // Now loop infinitely in here unless some condition is met
        // Typically instead of true you would be looking for the CTRL-C keybind to safely exit...
        while (true)
        {
            //printf("start wait for receive \n");
            // We will likely have to give an input when we make a new thread to feed in the teensy_ipaddress and teensy_port
            data_obj->get_ip(ipaddress, port);
            client_thread.Receiver(ipaddress, port);

            //printf("Received a message in ethernet class \n");

            // We got new data yay! (Dummy data)
            i++;
            j++;
            k++;

            // Set the local packet with the new data
            local_packet.position_x = i;
            local_packet.position_y = j;
            local_packet.position_z = k;
            local_packet.velocity_x = i * i;
            local_packet.velocity_y = j * j;
            local_packet.velocity_z = k * k;

            // Now set the data in our thread safe object class that is shared between threads by copying the local packet
            data_obj->set_data(local_packet);

            // Set the flag that the new data is ready
            data_obj->set_ready_flag(true);

            //printf("Finished 1 iteration \n");
        }
    }
};

#endif