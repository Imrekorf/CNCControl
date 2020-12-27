#pragma once

#include <cmath>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159
#endif
// SCHAAL is de vergrotings factor van de hoogtemap | 300mmx300mm : 600x600 
#ifndef SCHAAL
#define SCHAAL			  2
#endif

#define BITOVERLAP		  1
#define BITGROTE		  6
#define SPIRAALHOEK		 15

class Hoogtemap;
class ADCMaster;

struct point {
	double x;
	double y;
};

class GcodeManager
{
private:
	// bereken volgend punt op de spiraal
	point GetNextPoint(point& P, float a, float b, float& Hoek, float R, int& teller);

	// Wacht totdat de CNC frees een reactie op het vorige commando heeft gegeven.
	void WachtOpGRBLReactie();
	// Leest de ADS1015 uit om vast te stellen of de CNC beweegt of stilstaat.
	void LeesStepperMotors();

	ADCMaster* ADClezer;
	const unsigned int ADCChannel = 0;
	bool CNCbeweegt = false;

	// De ID gebruikt om met de CNC Frees te kunnen communiceren.
	int SerialID;

	// file stream om de gebruikte Gcode bij te houden.
	std::ofstream GcodeTracker;

public:
	GcodeManager(ADCMaster* ADClezer);
	~GcodeManager();

	// Stuurt Gcode naar de Frees, Verlaat de functie pas wanneer Frees stilstaat.
	void StuurGCode(std::string Gcode);

	void GenerateGcode(Hoogtemap matrix, double hoogstepunt, float BitGrote = BITGROTE, float Hoek = SPIRAALHOEK);
};
