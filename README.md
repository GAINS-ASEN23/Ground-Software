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

When compiled, the SPICE databases have to be placed in the x64 folder. Download the de440.bsp and naif0012.tls from the following links.

https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/planets/
https://naif.jpl.nasa.gov/pub/naif/generic_kernels/lsk/

## Usage
To run the software, click on GUI.exe in the x64 folder. If everything is correct, the software should start right up. 


## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://choosealicense.com/licenses/mit/)
