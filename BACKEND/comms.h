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

#include "ccsds.h"

#include "boost/array.hpp"
#include "boost/asio.hpp"
#include <ctime>
//#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>

using boost::asio::ip::udp;
using boost::asio::ip::address;
using boost::asio::buffer_cast;
using boost::asio::const_buffer;

void Send_String(std::string in, std::string send_ipaddress, int send_port) {
    // this function sends a string udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4()); 

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(in), remote_endpoint, 0, err); 
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

void Send_Float(float in, std::string send_ipaddress, int send_port) {
    // this function sends a float udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4());

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(&in,4), remote_endpoint, 0, err); 
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

void Send_TLM_Packet(GAINS_TLM_PACKET tlm_packet, std::string send_ipaddress, int send_port) {
    // this function sends a GAINS_TLM_PACEKT udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4()); 

    boost::system::error_code err;
    size_t packet_size = sizeof(tlm_packet);
    std::cout << "Sending TLM Data Packet of size: " << packet_size << "\n";
    auto sent = socket.send_to(boost::asio::buffer(&tlm_packet, packet_size), remote_endpoint, 0, err);
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

void Send_STAR_Packet(GAINS_STAR_PACKET star_packet, std::string send_ipaddress, int send_port) {
    // this function sends a GAINS_STAR_PACKET udp message to a specific ipaddress and port
    std::cout << "Sending on ipaddress: " << send_ipaddress << ", with port: " << send_port << "\n";
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(send_ipaddress), send_port);
    socket.open(udp::v4());

    boost::system::error_code err;
    size_t packet_size = sizeof(star_packet);
    auto sent = socket.send_to(boost::asio::buffer(&star_packet, packet_size), remote_endpoint, 0, err); 
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

struct Client {

    boost::asio::io_service io_service;
    udp::socket socket{ io_service };
    boost::array<uint8_t, 72> recv_buffer;
    boost::array<uint8_t, 720> data_buffer;
    int data_size_array[10];
    size_t bytes_transferred;
    udp::endpoint remote_endpoint;
    boost::system::error_code receive_error;
    size_t packetReceiveLen;

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

    void handle_receive(const boost::system::error_code& error) {
        if (error) {
            std::cout << "Receive failed: " << error.message() << "\n";
            return;
        }
        //if (bytes_transferred == 4) {
        //    float receive_float = 0;
        //    memcpy(&receive_float, &recv_buffer[0], sizeof(float));
        //    std::cout << "Received Time: '" << receive_float << "' (" << error.message() << ")\n";
        //}
        //else {
        //    if (recv_buffer[12] == 0) {
        //        // receive telemetry packet
        //        GAINS_TLM_PACKET tlm_packet;
        //        tlm_packet = read_TLM_Packet(recv_buffer);
        //        std::cout << "Successfully read in the data packet. These are the contents of the TLM Packet: \n";
        //        print_GAINS_TLM_PACKET(tlm_packet);
        //        headerData recvHdr = readHeader(tlm_packet.FullHeader.SpacePacket.Hdr);
        //    }
        //    else if (recv_buffer[12] == 1) {
        //        // receive star tracker packet
        //        GAINS_STAR_PACKET star_packet;
        //        star_packet = read_STAR_Packet(recv_buffer);
        //        std::cout << "Successfully read in the data packet. These are the contents of the STAR packet: \n";
        //        print_GAINS_STAR_PACKET(star_packet);
        //    }
        //}
    }

    void wait() {
        boost::system::error_code error;
        //bytes_transferred = socket.receive_from(boost::asio::buffer(recv_buffer), remote_endpoint, 0, error);
        bytes_transferred = socket.receive_from(boost::asio::buffer(data_buffer), remote_endpoint, 0, error);
        if (error) {
            std::cout << "Error in receive_from: " << error.message() << "\n";
            return;
        }
        /*for (int i = 0; i < 10; i++) {
            if (data_size_array[i] == 0) {
                data_size_array[i] = bytes_transferred;
                for (int j = 0; j < bytes_transferred; j++) {
                    data_buffer[72 * i + j] = recv_buffer[j];
                }
                break;
            }
        }*/
        printf("bytes received = %d \n", int(bytes_transferred));
        handle_receive(error);
    }

    //void Receiver(std::string receive_ipaddress, int receive_port, boost::array<uint8_t, 720> &data_buff, int* data_size)
    void Receiver(std::string receive_ipaddress, int receive_port, boost::array<uint8_t, 720>& data_buff, size_t &data_size)
    {
        /*for (int i = 0; i < 10; i++) {
            data_size_array[i] = 0;
        }*/
        wait();
        io_service.run();
        data_buff = data_buffer; // this only seems to send the data from the last message received
        //data_size = data_size_array;
        data_size = bytes_transferred;
    }
};
// end comms setup

// Begin multi threading setup

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

    //void get_data(boost::array<uint8_t, 720> &data, int* data_size_array) {
    void get_data(boost::array<uint8_t, 720> &data, size_t data_size) {
        data_mut.lock();
        data = ethernet_data::ethernet_data_buf;
        /*for (int i = 0; i < 10; i++) {
            data_size_array[i] = ethernet_data::data_sizes[i];
        }*/
        data_size = ethernet_data::bytes_transferred;
        data_mut.unlock();
    }

    void set_data(const boost::array<uint8_t, 720> data, size_t data_size){//int* data_size_array) {
        data_mut.lock();
        ethernet_data::ethernet_data_buf = data;
        /*for (int i = 0; i < 10; i++) {
            ethernet_data::data_sizes[i] = data_size_array[i];
        }*/
        ethernet_data::bytes_transferred = data_size;
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

    void set_close_thread() {
        data_mut.lock();
        ethernet_data::close_thread = true;
        data_mut.unlock();
    }

    void get_shouldCloseThread(bool& shouldCloseThread) {
        data_mut.lock();
        shouldCloseThread = ethernet_data::close_thread;
        data_mut.unlock();
    }


private:
    std::mutex data_mut;
    bool data_ready{ false };
    boost::array<uint8_t, 720> ethernet_data_buf {NULL};
    size_t bytes_transferred {0};
    int data_sizes[10] = { 0,0,0,0,0,0,0,0,0,0 };
    std::string ipaddress{ "0.0.0.0" };
    int port{8889};
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
        // Init vars
        bool data_ready_flag = false;
        //GAINS_TLM_PACKET local_packet;
        Client client_thread;
        std::string ipaddress;
        int port;
        bool connection_established = false;
        int count = 0;
        bool initiated = false;
        bool shouldCloseThread = false;
        boost::array<uint8_t, 720> data_buff;
        int data_size[10];
        size_t bytes_received = 0;

        // Dummy vars
        int i = 0;
        int j = 0;
        int k = 0;

        // Now loop infinitely in here unless some condition is met
        // Typically instead of true you would be looking for the CTRL-C keybind to safely exit...
        while (!shouldCloseThread)
        {
            data_obj->get_connection_established(connection_established);
            data_obj->get_shouldCloseThread(shouldCloseThread);
            if (connection_established) {
                data_obj->get_ip(ipaddress, port);
                count++;
                if (count == 1) {
                    initiated = client_thread.init(ipaddress, port);
                    std::cout << "Initiated = '" << initiated << "\n";
                }
                if (initiated) {
                    //client_thread.Receiver(ipaddress, port, data_buff, data_size);
                    client_thread.Receiver(ipaddress, port, data_buff, bytes_received); // blocking function - should include a handler to stop this when closing window

                    // We got new data yay! (Dummy data)
                    //i++;
                    //j++;
                    //k++;

                    // Set the local packet with the new data
                    /*local_packet.position_x = i;
                    local_packet.position_y = j;
                    local_packet.position_z = k;
                    local_packet.velocity_x = i * i;
                    local_packet.velocity_y = j * j;
                    local_packet.velocity_z = k * k;*/

                    // Now set the data in our thread safe object class that is shared between threads by copying the local packet
                    //data_obj->set_data(data_buff, data_size);
                    data_obj->set_data(data_buff, bytes_received);

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