/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include <chrono>
#include "..\Libraries\include\Eigen\Dense"
#include "boost/array.hpp"
#include "boost/asio.hpp"
#include <ctime>
#include <boost/bind.hpp>

#include "mainBackend.h"

// Comms setup
#define IPADDRESS "127.0.0.1" // "192.168.1.64"
#define UDP_PORT 13251

using boost::asio::ip::udp;
using boost::asio::ip::address;

void Sender(std::string in) {
    boost::asio::io_service io_service;
    udp::socket socket(io_service);
    udp::endpoint remote_endpoint = udp::endpoint(address::from_string(IPADDRESS), UDP_PORT);
    socket.open(udp::v4());

    boost::system::error_code err;
    auto sent = socket.send_to(boost::asio::buffer(in), remote_endpoint, 0, err);
    socket.close();
    std::cout << "Sent Payload --- " << sent << "\n";
}

struct Client {
    boost::asio::io_service io_service;
    udp::socket socket{ io_service };
    boost::array<char, 1024> recv_buffer;
    udp::endpoint remote_endpoint;

    int count = 3;

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
        socket.async_receive_from(boost::asio::buffer(recv_buffer),
            remote_endpoint,
            boost::bind(&Client::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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

int main()
{
    //begins comms testing
    printf("Hello Backend");

    Client client;
    std::thread r([&] { client.Receiver(); });

    //std::string input = argc > 1 ? argv[1] : "hello world";
    std::string input = "hello world";
    std::cout << "Input is '" << input.c_str() << "'\nSending it to Sender Function...\n";

    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        Sender(input);
        printf("Send message %d \n", i);
    }
    printf("Client Joined \n");
    r.join();
    // end comms testing


    // Recording the timestamp at the start of the code
    auto beg = std::chrono::high_resolution_clock::now();

    NBODYSIM nbodyObj;
    
    // Integration Stuff
    double totTime = 60 * 60 * 3;
    double dt = 10;

    std::cout << "NBODYSIM Running..... \n";

    std::vector<std::vector<double>> PosVector = nbodyObj.NBODYFUNC_SEMSC(totTime, dt, dateEx, V_scM, R_scM);

    for (size_t j = PosVector.size(); j-- > 0; ) {
        printf("\n %g %g %g;", PosVector.at(j).at(0), PosVector.at(j).at(1), PosVector.at(j).at(2));
    }

    // Taking a timestamp after the code is ran
    auto end = std::chrono::high_resolution_clock::now();

    // Subtracting the end timestamp from the beginning
    // And we choose to receive the difference in
    // microseconds
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - beg);

    // Displaying the elapsed time
    std::cout << "Elapsed Time: " << duration.count();

    return 0;
}
