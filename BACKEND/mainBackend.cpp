/* MAIN BACKEND FILE
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include <chrono>
#include "..\Libraries\include\Eigen\Dense"
/*#include "boost/array.hpp"
#include "boost/asio.hpp"
#include <ctime>
//#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>*/

#include "mainBackend.h"


int main()
{

    ////begins comms testing
    //printf("Hello Backend");

    //Client client;
    //std::thread r([&] { client.Receiver(); }); // operate the client on another thread so client and server can run at the same time

    ////std::string input = argc > 1 ? argv[1] : "hello world";
    //std::string input = "hello world";
    //std::cout << "Input is '" << input.c_str() << "'\nSending it to Sender Function...\n";

    //for (int i = 0; i < 3; ++i) {
    //    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //    Sender(input);
    //    printf("Send message %d \n", i);
    //}
    //printf("Client Joined \n");
    //r.join();
    //// end comms testing


    // Recording the timestamp at the start of the code
    auto beg = std::chrono::high_resolution_clock::now();

    NBODYSIM nbodyObj;
    
    // Integration Stuff
    double totTime = 60 * 60 * 3;
    double dt = 5;

    std::cout << "NBODYSIM Running..... \n";

    std::vector<std::vector<double>> PosVector = nbodyObj.NBODYFUNC_MSC(totTime, dt, dateEx, V_scM, R_scM);

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
