#include <wiringPi.h>
#include <iostream>
#include <fstream>

#include "Heightmap.h"
#include "Sensor.h"
#include "Frees.h"
#include "Vec3.h"


int main(int argc, char** argv)
{	
	// Bereken de benodigde punten voor de hoogtemap matrix
	unsigned int PuntenPerLijn = ScanBreedtemm / AfstandTussenPuntenmm + 1;
	unsigned int ScanLijnen = ScanHoogtemm / AfstandTussenLijnenmm + 1;

	// initialiseer WiringPI library
	if(wiringPiSetup() == -1){
		std::cerr << "unable to initialize wiringPi" << std::endl;
		return 1;
	}

	// initialiseer ADS1015 manager
	ADCMaster ADClezer;

	// Maak Sensor en Frees managers aan.
	Sensor S(&ADClezer);
	Frees F({0, 0, 0}, &ADClezer);
	// Creëer Hoogtemap object
	Hoogtemap heightmap(ScanLijnen, PuntenPerLijn);

	// Wees er zeker van dat alle objecten zijn aangemaakt en dat WiringPi is ge-instantieerd.
	delay(15);

	// Zet de Freessnelheid op 800mm per minuut
	F.StuurGCode("F800");

	// Zet de sensor op 15mm
	S.LevelSensor(F);

	// scan hoogtemap
	for(unsigned int i = 0; i < ScanLijnen; i++){
		// zigzag logica 
		unsigned int j = i % 2 ? PuntenPerLijn-1 : 0;
		int toename = i % 2 ? -1 : 1;
		double SensorVerschil = 0;
		for(;i % 2 ? j > 0 : j < PuntenPerLijn - 1; j += toename){
			// Meet de hoogte en sla deze op.
			heightmap[i][j] = S.RelatieveHoogteMeting(SensorVerschil);

			// Beweeg de Frees kop naar het volgende punt, houdt rekening met het sensorverschil.
			F.Beweeg({0, (double)toename * AfstandTussenPuntenmm, SensorVerschil});
		}
		// Meet laatste punt van de lijn.
		heightmap[i][j] = S.RelatieveHoogteMeting(SensorVerschil);

		// Beweeg naar de volgende lijn, en punt als deze er is.
		double Zhoogte = F.Positie().Z();
		F.Beweeg({0, 0, -1 * Zhoogte});
		if(i+1 < ScanLijnen){
			F.Beweeg({AfstandTussenLijnenmm, 0, 0});
			F.Beweeg({0, 0, Zhoogte + SensorVerschil});
		}
	}

	// Sla de hoogtemap op.
	heightmap.SlaHoogtemapOp("HeightmapTest.dat");

	std::cout << "job's done" << std::endl;

	return 0;
}