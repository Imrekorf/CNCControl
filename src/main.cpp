#include <wiringPi.h>
#include <iostream>
#include <fstream>

#include "GcodeManager.h"
#include "Heightmap.h"
#include "Sensor.h"
#include "Frees.h"
#include "Vec3.h"

#define RADIUS			 	75
#define CONISCHEHOEK		12
#define STAPGROTE			 0.5
#define BITGROTE			 6
#define SPIRAALHOEK			15

int main(int argc, char** argv)
{	
	// Bereken de benodigde punten voor de hoogtemap matrix
	unsigned int PuntenPerLijn = ScanGrotemm / AfstandTussenPuntenmm + 1;	// 4

	// initialiseer WiringPI library
	if(wiringPiSetup() == -1){
		std::cerr << "unable to initialize wiringPi" << std::endl;
		return 1;
	}

	// initialiseer ADS1015 manager
	ADCMaster ADClezer;
	GcodeManager Gman(&ADClezer);

	// Maak Sensor en Frees managers aan.
	Sensor S(&ADClezer);
	Frees F({0, 0, 0}, &Gman);
	// Creëer Hoogtemap object
	Hoogtemap heightmap(ScanGrotemm * SCHAAL + 1, PuntenPerLijn);	// 180 x 180
	// first  point =   0 - 60
	// second point =  60 - 120
	// third  point = 120 - 180

	// Wees er zeker van dat alle objecten zijn aangemaakt en dat WiringPi is ge-instantieerd.
	delay(15);

	// Zet de Freessnelheid op 800mm per minuut
	Gman.StuurGCode("F800");

	// Zet de sensor op 15mm
	S.LevelSensor(F);

	// scan hoogtemap
	for(unsigned int i = 0; i < PuntenPerLijn; i++){
		// zigzag logica 
		unsigned int j = i % 2 ? PuntenPerLijn-1 : 0;
		int toename = i % 2 ? -1 : 1;
		double SensorVerschil = 0;
		for(;i % 2 ? j > 0 : j < PuntenPerLijn - 1; j += toename){
			// Meet de hoogte en sla deze op.
			heightmap[i * AfstandTussenPuntenmm * SCHAAL][j * AfstandTussenPuntenmm * SCHAAL] = S.RelatieveHoogteMeting(SensorVerschil);
			// Beweeg de Frees kop naar het volgende punt, houdt rekening met het sensorverschil.
			F.Beweeg({0, (double)toename * AfstandTussenPuntenmm, SensorVerschil});
		}
		// Meet laatste punt van de lijn.
		heightmap[i * AfstandTussenPuntenmm  * SCHAAL][j * AfstandTussenPuntenmm * SCHAAL] = S.RelatieveHoogteMeting(SensorVerschil);

		// Beweeg naar de volgende lijn, en punt als deze er is.
		double Zhoogte = F.Positie().Z();
		F.Beweeg({0, 0, -1 * Zhoogte});
		if(i+1 < PuntenPerLijn){
			F.Beweeg({AfstandTussenPuntenmm, 0, 0});
			F.Beweeg({0, 0, Zhoogte + SensorVerschil});
		}
	}

	double Hoogstepunt = heightmap.MaakConischGat(RADIUS, CONISCHEHOEK, STAPGROTE);

	Gman.GenerateGcode(heightmap, Hoogstepunt, BITGROTE, SPIRAALHOEK);
	std::cout << "job's done" << std::endl;

	return 0;
}