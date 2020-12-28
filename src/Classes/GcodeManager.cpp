#include "GcodeManager.h"

#include "Heightmap.h"
#include "ADCmanager.h"

#include <wiringSerial.h>
#include <wiringPi.h>
#include <vector>

// Alle waardes hierboven geven aan dat de CNC aan het bewegen is.
#define CNCGestoptWaarde 60

GcodeManager::GcodeManager(ADCMaster* ADClezer) : ADClezer(ADClezer)
{
	// Open de filestream om Gcode in op te slaan.
	GcodeTracker.open("Gcode.tap");
	if(!GcodeTracker.is_open()){
		static_assert(true, "Error Opening Gcode.tap");
	}

	// Open een Seriele connectie met de CNC frees
	if((SerialID = serialOpen("/dev/ttyUSB0", 115200)) < 0){
		static_assert(true, "Unable to open ttyUSB0");
	}
	serialPuts(SerialID, "\r\n\r\n");
	// Geef extra tijd aan de CNC om een reactie te sturen.
	delay(500);
	WachtOpGRBLReactie();
}

GcodeManager::~GcodeManager()
{
	GcodeTracker.close();
	serialClose(SerialID);
}

// elektonische && Seriele logica
void GcodeManager::LeesStepperMotors(){
	double ADCWaarden = ADClezer->ADCWaarde(ADCChannel);
	static unsigned int CNCStopChecks = 0;
    if(ADCWaarden > CNCGestoptWaarde){
        // reset CNCStopChecks naar 0
        CNCbeweegt = true;
        CNCStopChecks = 0;
    }
    else
        CNCStopChecks++;
    if(CNCStopChecks > 10){
        // CNC gestopt voor meer dan 50ms
        CNCbeweegt = false;
    }
}

void GcodeManager::WachtOpGRBLReactie(){
	// print alle characters in de reactie stream
	std::cout << "response: ";
	int CharactersInSerialStream;
	do{
		CharactersInSerialStream = serialDataAvail(SerialID);
		for(int i = 0; i < CharactersInSerialStream; i++){
			int character = serialGetchar(SerialID);
			if(character != -1){
				std::cout << (char)character;
			}
		}
	}
	while(CharactersInSerialStream == 0);
}

void GcodeManager::StuurGCode(std::string gcode){
	GcodeTracker << gcode << std::endl;
	// Log de gestuurde Gcode
	std::cout << "Gcode: " << gcode << std::endl;
	
	gcode += "\r\n";
	serialPuts(SerialID, gcode.c_str());

	WachtOpGRBLReactie();

	// Geef tijd om de CNC te laten bewegen
	delay(200);
	do{
		LeesStepperMotors();
		delay(5);
	}
	while(CNCbeweegt);

	// Geef tijd zodat de CNC het volgende commando kan opvolgen
	delay(100);
}

// spiraal logica
Vec3<double> GcodeManager::GetNextPoint(Vec3<double>& P, float a, float b, float& Hoek, float R, int& teller){
	// X, Y, XN en YN worden gebruikt om bij te houden of de spiraal positie veranderd is of niet
	static float X = 0, Y = 0;
	static float XN = 0, YN = 0;

	// als teller niet het laatste punt is, bereken dan de punten
	if(teller != 0){
		while(1){
			// bereken de hoek voor het punt op de spiraal
			Hoek = (R * teller);
			// bereken X en Y coordinaat van het punt
			XN = round((a + b * Hoek) * std::cos(Hoek));
			YN = round((a + b * Hoek) * std::sin(Hoek));
			// als X of Y hetzelfde is als de nieuw berekende waardes, 
			// dan door naar het volgende punt.
			if(X == XN || YN == Y){
				teller--;
				continue;
			}
			// anders stop met nieuwe punten berekenen
			else{
				break;
			}
		}
	}
	// anders zet de punten op 0
	else{
		XN = 0; YN = 0;
	}
	// retour het nieuw berekende punt.
	P.X((int)XN);
	P.Y((int)YN);
	return P;
}

void GcodeManager::GenerateGcode(Hoogtemap matrix, double hoogstepunt, float BitGrote, float Hoek){
	// Diameter
	BitGrote -= BITOVERLAP;
	float FreesDiameter = matrix.GetMatrixGrote() / SCHAAL - BitGrote;

	// hoeveelheid loops in de spiraal
	float Loops = round(FreesDiameter / BitGrote);
	// vergrotings factor van de hoek per iteratie
	float rc = (Hoek * M_PI) / 180;
	// Hoeveelheid punten in de spiraal
	float MaxPoints = (360 * Loops) / Hoek;

	// begin positie a
	float a = BitGrote / 2;
	// afstand tussen spiraalbanen
	float b = BitGrote / (2 * M_PI);

	// Gcode Header
	std::vector<std::string> Gcode = {"G90 G94 G91.1 G40 G49 G17",
						"G21",
						"G28 G91 Z0.",
						"G90",
						"(SPIRAL1)",
						"M5",
						"T3 M6",
						"S5000 M3",
						"G54",
						"M8",
						//"G0 X-47.007 Y0.125\n"
						//"G43 Z5. H3\n"
						//"Z0.476\n"
						"G1 "};
	
	// radius van de spiraal in .5mm
	int Radius = matrix.GetMatrixGrote() / 2;

	// Houdt de positie op de spiraal bij
	Vec3<double> Point({0, 0, 0});
	bool FirstLoop = true;
	for(int i = MaxPoints-1; i >= 0; i--){
		Point = GetNextPoint(Point, a, b, Hoek, rc, i);
		// Hiervoor wordt een FirstLoop gebruikt omdat i niet bekend is
		// van de eerste loop nadat GetNextPoint aangeroepen is.
		if(FirstLoop){
			// Beweeg de freeskop boven de eerste gcode instructie
			Gcode.push_back("X" + std::to_string(Point.X() / SCHAAL) + 
				 	 "Y" + std::to_string(Point.Y() / SCHAAL) + 
				 	 "Z" + std::to_string(0.0));
			Gcode.push_back("F1000"); 
			FirstLoop = false;
		}
		
		// haal de freesdiepte uit de hoogtemap op. Haal hier het hoogstepunt waarde vanaf om de hoogtemap waarde negatief te maken.
		Point.Z(matrix[(int)(Point.Y() + Radius)][(int)(Point.X() + Radius)] - hoogstepunt);
		
		// Beweeg de freeskop naar positie:
		Gcode.push_back("X" + std::to_string(Point.X() / SCHAAL) + 
				 "Y" + std::to_string(Point.Y() / SCHAAL) + 
				 "Z" + std::to_string(Point.Z())); 
	}

	// Gcode tail
	Gcode.push_back("M9");
	Gcode.push_back("G28 G91 Z0.");
	Gcode.push_back("G90");
	Gcode.push_back("G28 G91 X0. Y0. ");
	Gcode.push_back("G90");
	Gcode.push_back("M30");
	
	// leeg Frees.tap
	std::ofstream ClearFile;
	ClearFile.open("Frees.tap", std::ofstream::out | std::ofstream::trunc);
	ClearFile.close();
	// Sla Gcode bestand op onder "Frees.tap" en stuur Gcode naar CNC
	std::ofstream OutputFile("Frees.tap");
	if(OutputFile.is_open()){
		for(unsigned int i = 0; i < Gcode.size(); i++){
			serialPuts(SerialID, Gcode[i].c_str());
			OutputFile << Gcode[i] << std::endl;
		}
		OutputFile.close();
	}
	else{
		std::cout << "Unable to open file: Frees.tap" << std::endl;
	}
}