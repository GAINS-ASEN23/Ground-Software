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

## Backend Functionality


