/* MAIN BACKEND FILE 
* GAINS Senior Project Capstone Ground Software Backend
* Authors: Derek Popich, Cannon Palmer, Brian Trybus, Alfredo Restrepo
* Purpose: This is the main file for the Orbital Trajectory Estimation Software backend.
*/

#include <iostream>
#include <cmath>

int main() 
{
	return 0;
}

//Still making the class with function, when working will move out of main
class orbitalBody {
	//https://ssd.jpl.nasa.gov/planets/approx_pos.html Sol system orbital info
	//[a,e,I,L,w,O]
	//Use Keplers equations to cacluate position of each planet starting in a sun centered frame then changing to the Earth/Moon Barycenter
	//needs orbital peramiters of planet, time, and output matrix
	//returns xyz on earth centered frame

public:
	float semiMajor; // semi major axis [au]
	float ecent; // ecentrisity [unitless]
	float inclination; // inclination [rad]
	float meanLong; // mean longitude 
	float argPeri; // argument of periapsis
	float longPeri; //longitude of periapsis
	float longAsc; // longitue of ascending node
	float meanAng; // mean angular motion
	float semiRec; // semilatus rectum
	float period; // period
	float mu; //standard gravitational parameter

	void orbitalBody::position(float& pos, float time);

	orbitalBody(float a, float e, float I, float L, float wPeri, float longAsnd, float standarGrav) {
		
		semiMajor = a;
		ecent = e;
		inclination = I;
		meanLong = L;
		longPeri = wPeri;
		longAsc = longAsnd;
		mu = standarGrav;

		argPeri = longPeri - longAsc;

		semiRec = semiMajor * (1 - (e * e));

		meanAng = sqrt(mu/(a*a*a));

		period = 2 * M_PI / meanAng;

	}
};

void orbitalBody::position(float &pos, float time){
}