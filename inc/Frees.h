#pragma once

#include <string>
#include <iostream>

#include "GcodeManager.h"
#include "Vec3.h"

class Frees
{
private:
	Vec3<double> Pos;

	// houdt bij of de frees in relatieve of absolute modus is.
	bool Relatief = 0;

	// Verwerkt een Vec3 naar Gcode
	void _Beweeg(Vec3<double> V);

	GcodeManager* Gman;

public:
	Frees(Vec3<double> Positie, GcodeManager* Gman);
	~Frees();

	// Besturen de CNC op basis van Vectoren
	void Beweeg(Vec3<double> V);
	void BeweegNaar(Vec3<double> P);

	// Geeft de huidige berekende positie van de frees terug.
	const Vec3<double> Positie();
};
