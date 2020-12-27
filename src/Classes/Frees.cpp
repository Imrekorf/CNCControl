#include "Frees.h"

Frees::Frees(Vec3<double> Position, GcodeManager* Gman) : Gman(Gman)
{
	// Bewaar de meegegeven positie.
	Pos = Position;

}

Frees::~Frees() {}

// Verwerkt de vector naar een Gcode string
void Frees::_Beweeg(Vec3<double> V){
	std::string Gcode;
	Gcode += "G1";
	if(Relatief){
		if(V.Y())
			Gcode += "Y" + std::to_string(V.Y());
		if(V.X())
			Gcode += "X" + std::to_string(V.X());
		if(V.Z())
			Gcode += "Z" + std::to_string(V.Z());
	}
	else{
			Gcode += "Y" + std::to_string(V.Y());
			Gcode += "X" + std::to_string(V.X());
			Gcode += "Z" + std::to_string(V.Z());
	}
	Gman->StuurGCode(Gcode);
}

// Beweeg de frees relatief tot vorige positie
void Frees::Beweeg(Vec3<double> V){
	if(!Relatief){
		Gman->StuurGCode("G91");
		Relatief = 1;
	}
	_Beweeg(V);
	
	Pos += V;
}

// Beweeg de frees absoluut tot de nul positie
void Frees::BeweegNaar(Vec3<double> P){
	if(Relatief){
		Gman->StuurGCode("G90");
		Relatief = 0;
	}

	_Beweeg(P);

	Pos = P;
}

const Vec3<double> Frees::Positie(){
	return Pos;
}