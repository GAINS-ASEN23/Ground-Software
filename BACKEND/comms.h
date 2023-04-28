#pragma once
/* Communications Header File
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Jason Popich, Cannon Palmer, Tucker Peyok, Brian Trybus, Derek Popich, Ben McHugh
* Purpose: This is the header file for the communications and multi-threading functionality
*/

#ifndef COMMS_H
#define COMMS_H

#include <iostream> 
#include <thread> 
#include <mutex>

// Communications includes
#include "ccsds.h"
#include "boost/array.hpp"
#include "boost/asio.hpp"
#include <ctime>
#include <boost/bind/bind.hpp>

// Configure Namespace for boost communications
using boost::asio::ip::udp;
using boost::asio::ip::address;
using boost::asio::buffer_cast;
using boost::asio::const_buffer;

// Send a string to the given ipaddress and port
void Send_String(std::string in, std::string send_ipaddress, int send_port) {

    // Configure Socket
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);

    // Send message on Socket
    socket.open(udp::v4());
    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(in), remote_endpoint, 0, err);
    socket.close();
}

// Send a float to the given ipaddress and port - Often used for sending time floats
void Send_Float(float in, std::string send_ipaddress, int send_port) {

    // Configure Socket
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);

    // Send message on Socket
    socket.open(udp::v4());
    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(&in, 4), remote_endpoint, 0, err);
    socket.close();
}

// Send a Telemetry Packet on the given ipaddress and port
void Send_TLM_Packet(GAINS_TLM_PACKET tlm_packet, std::string send_ipaddress, int send_port) {

    // Configure Socket
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);

    // Send message on Socket
    socket.open(udp::v4());
    boost::system::error_code err;
    size_t packet_size = sizeof(tlm_packet);
    auto sent = socket.send_to(boost::asio::buffer(&tlm_packet, packet_size), remote_endpoint, 0, err);
    socket.close();
}

// Send a Star Tracker Packet on the given ipaddress and port
void Send_STAR_Packet(GAINS_STAR_PACKET star_packet, std::string send_ipaddress, int send_port) {

    // Configure Socket
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4());

    // Send message on Socket
    boost::system::error_code err;
    size_t packet_size = sizeof(star_packet);
    auto sent = socket.send_to(boost::asio::buffer(&star_packet, packet_size), remote_endpoint, 0, err);
    socket.close();
}

// Client side communications struct for receiving messages
struct Client {

    // Common Variables used throughout struct
    boost::asio::io_service io_service;
    udp::socket socket{ io_service };
    boost::array<uint8_t, 72> data_buffer;
    size_t bytes_transferred{0};
    udp::endpoint remote_endpoint;

    // Initilization of receiving communications to given ipaddress and port - The receiving Ipaddress and Port can only be bound once (usually this is fine
    // since it normally receives on port 0.0.0.0)
    bool init(std::string receive_ipaddress, int receive_port) {
        std::cout << "Receiving on ipaddress: " << receive_ipaddress << ", with port: " << receive_port << "\n";
        Client::remote_endpoint = udp::endpoint(address::from_string(receive_ipaddress), receive_port);
        socket.open(remote_endpoint.protocol());
        boost::system::error_code error;
        socket.bind(remote_endpoint, error);
        if (error) {
            std::cout << "Error binding to socket: " << error.message() << "\n";
            return false;
        }
        return true;
    }

    // Print data packet receive errors
    void handle_receive(const boost::system::error_code& error) {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
    }

    // Wait to receive a data packet and send errors to handler
    void wait() {
        boost::system::error_code error;
        bytes_transferred = socket.receive_from(boost::asio::buffer(data_buffer), remote_endpoint, 0, error);
        if (error) {
            std::cout << "Error in receive_from: " << error.message() << "\n";
            return;
        }
        handle_receive(error);
    }

    // Main receive function that delegates tasks and returns the received data
    void Receiver(std::string receive_ipaddress, int receive_port, boost::array<uint8_t, 72>& data_buff, size_t& data_size)
    {
        wait();
        io_service.run();
        data_buff = data_buffer;
        data_size = bytes_transferred;
    }
};

// This class controls the mutex locks to ensure multi thread safe ethernet operations
class ethernet_data {
public:
    // Default Constructor (Don't use)
    ethernet_data() {};

    // Default Destructor
    ~ethernet_data() {};

    // Get flag for if received data is loaded
    void get_ready_flag(bool& data_ready_flag) {
        data_mut.lock();
        data_ready_flag = ethernet_data::data_ready;
        data_mut.unlock();
    }

    // Set flag for if received data is loaded
    void set_ready_flag(const bool set_data_ready) {
        data_mut.lock();
        ethernet_data::data_ready = set_data_ready;
        data_mut.unlock();
    }

    // Get the data buffer and size of loaded data
    void get_data(boost::array<uint8_t, 72>& data, size_t& data_size) {
        data_mut.lock();
        data = ethernet_data::ethernet_data_buf;
        data_size = ethernet_data::bytes_transferred;
        data_mut.unlock();
    }

    // Set the loaded data
    void set_data(const boost::array<uint8_t, 72> data, size_t data_size) {
        data_mut.lock();
        ethernet_data::ethernet_data_buf = data;
        ethernet_data::bytes_transferred = data_size;
        data_mut.unlock();
    }

    // Get the ipaddress and port
    void get_ip(std::string& out_ipaddress, int& out_port) {
        data_mut.lock();
        out_ipaddress = ethernet_data::ipaddress;
        out_port = ethernet_data::port;
        data_mut.unlock();
    }

    // Set the ipaddress and port used on the thread
    void set_ip(std::string new_ipaddress, int new_port) {
        data_mut.lock();
        ethernet_data::ipaddress = new_ipaddress;
        ethernet_data::port = new_port;
        ethernet_data::establish_ip = true;
        data_mut.unlock();
    }

    // Get flag for if the receiver connection was successfully established
    void get_connection_established(bool& connection_established) {
        data_mut.lock();
        connection_established = ethernet_data::establish_ip;
        data_mut.unlock();
    }

    // Set flag for if the receiver connection was successfully established
    void set_connection_established(bool connection_established) {
        data_mut.lock();
        ethernet_data::establish_ip = connection_established;
        data_mut.unlock();
    }

    // Get flag for if the additional thread should close
    void get_shouldCloseThread(bool& shouldCloseThread) {
        data_mut.lock();
        shouldCloseThread = ethernet_data::close_thread;
        data_mut.unlock();
    }

    // Set flag for if the additional thread should close
    void set_close_thread() {
        data_mut.lock();
        ethernet_data::close_thread = true;
        data_mut.unlock();
    }

private:
    std::mutex data_mut;
    bool data_ready{ false };
    boost::array<uint8_t, 72> ethernet_data_buf{ NULL };
    size_t bytes_transferred{ 0 };
    std::string ipaddress{ "0.0.0.0" };
    int port{ 8889 };
    bool establish_ip{ false };
    bool close_thread{ false };

};


// Define the class
class ethernet_backend {
public:
    // Overload the operator () by doing operator() and defining what arguments you want to pass through on the next set of parenthesis
    // Which in this case is the pointer to our ethernet data object
    void operator()(ethernet_data* data_obj)
    {
        // Initialization Variables
        bool data_ready_flag = false;
        Client client_thread;
        std::string ipaddress;
        int port;
        bool connection_established = false;
        bool data_ready;
        int count = 0;
        bool initiated = false;
        bool shouldCloseThread = false;
        boost::array<uint8_t, 72> data_buff;
        size_t bytes_received = 0;

        // Now loop infinitely in here until the thread is signaled to close - Note that receiving is a blocking operation so it cannot close until any message is received
        while (!shouldCloseThread)
        {
            // Update bools
            data_obj->get_connection_established(connection_established);
            data_obj->get_shouldCloseThread(shouldCloseThread);

            // Will run once thread connection is initialized in main.cpp
            if (connection_established) {
                data_obj->get_ip(ipaddress, port);
                count++;

                // on first time will initiate the connection to the receiver
                if (count == 1) {
                    initiated = client_thread.init(ipaddress, port);
                    std::cout << "Initiated = '" << initiated << "\n";
                }

                // Once receiver is initialized we can start receiving messages
                if (initiated) {
                    data_obj->get_ready_flag(data_ready);
                    if (!data_ready) {
                        client_thread.Receiver(ipaddress, port, data_buff, bytes_received); // blocking function - should include a handler to stop this when closing window

                        // Now set the data in our thread safe object class that is shared between threads by copying the local packet
                        data_obj->set_data(data_buff, bytes_received);

                        // Set the flag that the new data is ready
                        data_obj->set_ready_flag(true);
                    }
                }
                else {
                    // Catch if the connection is initiated in main but an exception is thrown
                    data_obj->set_connection_established(false);
                }
            }
        }
    }
};

#endif