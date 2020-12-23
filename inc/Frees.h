#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "Vec3.h"

#include "ADCmanager.h"

class Frees
{
private:
	Vec3<double> Pos;

	// file stream om de gebruikte Gcode bij te houden.
	std::ofstream GcodeTracker;

	// houdt bij of de frees in relatieve of absolute modus is.
	bool Relatief = 0;

	// De ID gebruikt om met de CNC Frees te kunnen communiceren.
	int SerialID;

	// Verwerkt een Vec3 naar Gcode
	void _Beweeg(Vec3<double> V);
	// Wacht totdat de CNC frees een reactie op het vorige commando heeft gegeven.
	void WachtOpGRBLReactie();

	ADCMaster* ADClezer;
	const unsigned int ADCChannel = 0;
	bool CNCbeweegt = false;
	// Leest de ADS1015 uit om vast te stellen of de CNC beweegt of stilstaat.
	void LeesStepperMotors();

public:
	Frees(Vec3<double> Positie, ADCMaster* ADClezer);
	~Frees();

	// Besturen de CNC op basis van Vectoren
	void Beweeg(Vec3<double> V);
	void BeweegNaar(Vec3<double> P);

	// Stuurt Gcode naar de Frees, Verlaat de functie pas wanneer Frees stilstaat.
	void StuurGCode(std::string Gcode);

	// Geeft de huidige berekende positie van de frees terug.
	const Vec3<double> Positie();
};
