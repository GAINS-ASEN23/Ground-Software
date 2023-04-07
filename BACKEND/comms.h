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

using boost::asio::ip::udp;
using boost::asio::ip::address;

void Send_String(std::string in, std::string send_ipaddress, int send_port) {
    // this function sends a udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4()); //opens a socket using the IPv4 protocol

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(in), remote_endpoint, 0, err); // "boost::asio::buffer" function takes a string as input and writes it to a boost buffer
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

void Send_Uint32(uint32_t in, std::string send_ipaddress, int send_port) {
    // this function sends a udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4()); //opens a socket using the IPv4 protocol

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(&in,4), remote_endpoint, 0, err); // "boost::asio::buffer" function takes a string as input and writes it to a boost buffer
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
    //std::string prev_ipaddress = "0";
    //int prev_port = 0;

    int count = 1; // will wait for this many messages before allowing the main program to roll on

    bool init(std::string receive_ipaddress, int receive_port) {
        /*if ((prev_ipaddress == receive_ipaddress) && (prev_port == receive_port)) {
            return;
        }
        else {*/
            //Client::close();
        std::cout << "Receiving on ipaddress: " << receive_ipaddress << ", with port: " << receive_port << "\n";
        Client::remote_endpoint = udp::endpoint(address::from_string(receive_ipaddress), receive_port);
        //printf("Made it past creating remote endpoint \n");
        socket.open(remote_endpoint.protocol());
        //printf("Made it past open \n");
        boost::system::error_code error;
        socket.bind(remote_endpoint, error);
        //printf("Made it past bind \n");
        //socket.connect(remote_endpoint, error);
        if (error) {
            std::cout << "Error binding to socket: " << error.message() << "\n";
            return false;
        }
        //printf("Returned True \n");
        return true;

            //prev_ipaddress = receive_ipaddress;
            //prev_port = receive_port;
        //}
    }

    //void close()
    //{
    //    printf("Close function initiated \n");
    //    io_service.post([this]() {
    //        socket.close();
    //        printf("socket.close done \n");
    //        // As long as outstanding completion handlers do not
    //        // invoke operations on socket_, then socket_ can be 
    //        // destroyed.
    //        //socket.release();
    //        //printf("socket.release done \n");
    //        });
    //    printf("close function done \n");
    //}

    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        // receive uint32_t
        uint32_t receive_uint32 = uint32_t(((recv_buffer[0] << 24) | (recv_buffer[1] << 16) | (recv_buffer[2] << 8) | recv_buffer[3]));
        std::cout << "Received: '" << receive_uint32 << "' (" << error.message() << ")\n";

        // receive string
        //std::cout << "Received: '" << std::string(recv_buffer.begin(), recv_buffer.begin() + bytes_transferred) << "' (" << error.message() << ")\n";

        // receive struct (put this stuff below)

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
        // maybe can double check if previous ipaddress == new_ipaddress, if not then can bind to a new socket

        wait();

        //std::cout << "Receiving\n";
        io_service.run();
        //std::cout << "Receiver exit\n";
    }
};
// end comms setup

// Begin multi threading setup

struct GAINS_TLM_PACKET {
    uint8_t     TlmHeader[CFE_SB_TLM_HDR_SIZE]{ 0 }; //can give type telemetry packet
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
        ethernet_data::establish_ip = true;
        data_mut.unlock();
    }

    void get_connection_established(bool& connection_established) {
        data_mut.lock();
        connection_established = ethernet_data::establish_ip;
        data_mut.unlock();
    }

    void set_connection_established(bool connection_established) {
        data_mut.lock();
        ethernet_data::establish_ip = connection_established;
        data_mut.unlock();
    }


private:
    std::mutex data_mut;
    bool data_ready{ false };
    GAINS_TLM_PACKET ethernet_data_buf;
    std::string ipaddress{ "0.0.0.0" };
    int port{8889};
    bool establish_ip{ false };
    //bool close_thread{false}; // is there a handler I can enable when thread.join() occurs that will properly end the receive and stop the thread

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
        bool connection_established = false;
        int count = 0;
        bool initiated = false;

        // Dummy vars
        int i = 0;
        int j = 0;
        int k = 0;

        // Now loop infinitely in here unless some condition is met
        // Typically instead of true you would be looking for the CTRL-C keybind to safely exit...
        while (true)
        {
            data_obj->get_connection_established(connection_established);
            if (connection_established) {
                data_obj->get_ip(ipaddress, port);
                count++;
                if (count == 1) {
                    initiated = client_thread.init(ipaddress, port);
                    std::cout << "Initiated = '" << initiated << "\n";
                }
                if (initiated) {
                    client_thread.Receiver(ipaddress, port);

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
                }
                else {
                    data_obj->set_connection_established(false);
                    //count = 0;
                }
            }
        }
    }
};

#endif