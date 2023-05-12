# Ground Software

Lead: Cannon Palmer

Backend: Derek Popich


# GAINS Ground Software

The GAINS Ground Software is made to work with the GAINS Flight software on an inertial navigation system around a lunar circular orbit. There are two main parts to the software. The Backend and Frontend. The Frontend contains all of the graphical user interface rendering while the backend has the communications protocals and the orbital estimation models. 

## Installation

Clone the repository and use Microsoft Visual Studio 2022 and newer to compile.

```bash
git clone https://github.com/GAINS-ASEN23/Ground-Software.git
```

When first compiled, the SPICE databases have to be placed in the Ground-Software/x64 folder. Download the de440.bsp and naif0012.tls from the following links.

https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/planets/

https://naif.jpl.nasa.gov/pub/naif/generic_kernels/lsk/

## Compiling

The code must be compiled after any edits or changes. The GUI.exe will be regenerated with each compilation, so a compile is necessary for GUI.exe to reflect any code changes.

How to compile using Visual Studio 2022: Under "Solution Explorer", right click on "Solution 'groundSoftware'" and click "build" for small updates, or click "rebuild" if large changes have been made to the code or if error occur.

## Usage

To run the software, click on GUI.exe in the x64 folder. If everything is correct, the software should start right up. 


There are 3 modes to the ground software. The first is Sim Mode. This mode is used to predict the trajectory of a spacecraft around the moon. The Display mode and Test modes are specific to the project this software was used for. They receive live data from the INS and display the appropiate changes to the software. To communicate with the INS, there are UDP address blocks used to select IP. The communication protocol uses UDP and the CCSDS space packed protocol combined. 

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)

## Frontend Functionality

The frontend is composed of a graphics render engine which uses the OpenGL framework and a GUI which uses the ImGUI framework. Data is received from the backend simulation and communications, and this data is plotted and shown on screen using the graphics render engine. The GUI allows control of the graphics render engine parameters.

The GUI provides control over the simulation and communications. The simulation run speed scales with the time scale shown on the GUI so that every second the frontend runs will plot the time scale value amount of seconds worth of data. The distance scale provided by the GUI allows for the graphics render engine to zoom in or out in order to show the desired data at a reasonable scale, and there is a reference bar to show the scale on the bottom left. By default the distance scale will display in kilometers unless specified otherwise. The GUI also provides communications control which includes setting the Ipaddress and port for both sending and receiving messages. Once the desired parameters for communications are set, the "Intitiate Receiver" button must be pressed in order for the connection to begin. Other controls on the GUI allow for changing the simulation mode, pausing the simulation, or sending test messages.

The graphics render engine is the core of the frontend. The graphics render engine creates the program window, creates shaders and textures, and displays all shapes and colors. In order to draw a something, a shader program must be created using the shader segments in the shader.h file, data points for each vertex must be provided and will be processed by the shader program, and then transformation matrices will be applied and the shape will be drawn on screen.

## Backend Functionality

The backend is composed of an orbital trajectory estimation package and a real time UDP communications package combined to be used with data from the GAINS. Both packages are independent of each other but can be used in conjunction if necessary. A note is made that the entire backend is made to run as a library used by the frontend. As such, all the classes and functions can be called from a different project. 

The orbital trajectory estimation package has two models built in. The first and most simple is the N-Body simulation designed to be a naive propagation. There are two functions that specify which planets the N-body takes into account. The N-body model then places the X,Y,Z data of the orbital bodies and the S/C estimation into double vectors for use by the frontend for plotting. The more complicated model is the Clohessy-Wiltshire equations implemented into a state-space model. The model has to have the initial conditions be the output of the INS to properly predict the trajectory of the INS. Once again, the resulting data is placed into double vectors for use by the frontend. Both models use the NAIF Spice data from NASA to compute orbital element postions for use by the models. 

The communications package is located in the backend. The communications package provides functions for creating and decoding CCSDS space packets, transforming these packets to and from buffers, and then sending and receiving these buffers as UDP packets using the Boost Asio library. In order to provide more efficient functionality of the ground software, the communications package is built to be run using multi-threading with thread safety build in using mutex locks for data transfer.
In order to send a message, use the frontend GUI to select the proper Ipaddress and port to send to. Next, load data into the space packet protocol using the provided functions and send this packet using the provided send functions. Note that sending messages does not require an additional thread because it is a non-blocking function.
In order to receive messages, create a thread to the "ethernet_backend" function. Then, using the frontend GUI, select the proper Ipaddress and port to receive from if there is a specific one, and if not, the thread will received all UDP messages sent to it. Once a message is received, the thread will automatically receive the message and set the data received flag to true. Next, the data can be transferred for use through the given mutex lock functions. Provided functions will decode the received data into usable data and the data ready flag must be set to false in order to receive more data.
