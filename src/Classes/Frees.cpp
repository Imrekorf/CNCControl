#include "Frees.h"
#include <wiringSerial.h>
#include <wiringPi.h>

// Alle waardes hierboven geven aan dat de CNC aan het bewegen is.
#define CNCGestoptWaarde 60


Frees::Frees(Vec3<double> Position, ADCMaster* ADClezer) : ADClezer(ADClezer)
{
	// Bewaar de meegegeven positie.
	Pos = Position;
	// Open de filestream om Gcode in op te slaan.
	GcodeTracker.open("Gcode.tap");
	if(!GcodeTracker.is_open()){
		static_assert(true, "Error Opening Gcode.tap");
	}

	// Open een Seriele connectie met de CNC frees
	if((SerialID = serialOpen("/dev/ttyUSB0", 115200)) < 0){
		static_assert(true, "Unable to open ttyUSB0");
	}
	// Geef extra tijd aan de CNC om een reactie te sturen.
	delay(500);
	WachtOpGRBLReactie();

}

Frees::~Frees()
{
	GcodeTracker.close();
	serialClose(SerialID);
}

// elektonische && Seriele logica
void Frees::LeesStepperMotors(){
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

void Frees::WachtOpGRBLReactie(){
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

void Frees::StuurGCode(std::string gcode){
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
	StuurGCode(Gcode);
}

// Beweeg de frees relatief tot vorige positie
void Frees::Beweeg(Vec3<double> V){
	if(!Relatief){
		StuurGCode("G91");
		Relatief = 1;
	}
	_Beweeg(V);
	
	Pos += V;
}

// Beweeg de frees absoluut tot de nul positie
void Frees::BeweegNaar(Vec3<double> P){
	if(Relatief){
		StuurGCode("G90");
		Relatief = 0;
	}

	_Beweeg(P);

	Pos = P;
}

const Vec3<double> Frees::Positie(){
	return Pos;
}