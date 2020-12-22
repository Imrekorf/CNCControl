#include "Frees.h"
#include <wiringSerial.h>
#include <wiringPi.h>

#define CNCStoppedValue 60


Frees::Frees(Vec3<double> Position, ADCMaster* ADCreader) : ADCreader(ADCreader)  //(1, &Frees::ADCValueProcessor, this)
{
	position = Position;
	GcodeTracker.open("Gcode.tap");
	if(!GcodeTracker.is_open()){
		static_assert(true, "Error Opening Gcode.tap");
	}

	if((SerialID = serialOpen("/dev/ttyUSB0", 115200)) < 0){
		static_assert(true, "Unable to open ttyUSB0");
	}
	delay(500);
	WaitForGRBLResponse();
}

Frees::~Frees()
{
	GcodeTracker.close();
	serialClose(SerialID);
}

// electonic && Serial logic
void Frees::CheckCNCMoving(){
	double ADCValue = ADCreader->GetADCValue(ADCChannel);
	static unsigned int CNCStopChecks = 0;
    if(ADCValue > CNCStoppedValue){
        // reset values to 0
        CNCmoving = 1.0;
        CNCStopChecks = 0;
		//std::cout << ADCValue << std::endl;
    }
    else
        CNCStopChecks++;
    if(CNCStopChecks > 10){
        // CNC stopped for more than 10ms
        CNCmoving = 0.0;
    }
}

void Frees::WaitForGRBLResponse(){
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

// Gcode Senders
void Frees::SendGCode(std::string gcode){
	GcodeTracker << gcode << std::endl;
	// temporary untill we can send gcode to CNC
	std::cout << "Gcode: " << gcode << std::endl;
	
	gcode += "\r\n";
	serialPuts(SerialID, gcode.c_str());

	WaitForGRBLResponse();

	// wait for CNC to start moving
	delay(200);
	do{
		CheckCNCMoving();
		delay(5);
	}
	while(CNCmoving);

	delay(100); // be sure that the CNC is able to catch up to the next command
	// char x;
	// std::cin >> x;
}

void Frees::GiveHumanGcode(std::string gcode){
	GcodeTracker << gcode << std::endl;
	std::cout << "Gcode: " << gcode << std::endl << "Finished? "; 
	std::cin.get();
}

// Instruction Parsers
void Frees::_Move(Vec3<double> V){
	std::string Gcode;
	Gcode += "G1";
	if(Relative){
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
	SendGCode(Gcode);
}

void Frees::Move(Vec3<double> V){
	if(!Relative){
		SendGCode("G91");
		Relative = 1;
	}
	_Move(V);
	
	position += V;
	position.print();
}

void Frees::MoveTo(Vec3<double> P){
	if(Relative){
		SendGCode("G90");
		Relative = 0;
	}

	_Move(P);

	position = P;
}

const Vec3<double> Frees::GetPosition(){
	return position;
}