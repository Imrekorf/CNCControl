#include "Frees.h"
#include <wiringSerial.h>
#include <wiringPi.h>

#include "ads1015/Adafruit_ADS1015.h"
#include <unistd.h>

double Frees::ADCValue = 0.0;
std::mutex Frees::ADCmutex;

Frees::Frees(Vec3<double> Position)
{
	position = Position;
	GcodeTracker.open("Gcode.tap");
	if(!GcodeTracker.is_open()){
		static_assert(true, "Error Opening Gcode.tap");
	}

	if((SerialID = serialOpen("/dev/ttyUSB0", 115200)) < 0){
		static_assert(true, "Unable to open ttyUSB0");
	}

	WaitForGRBLResponse();

	std::thread ReadADC(ADCreadThread);
	ReadADC.detach();
}

Frees::~Frees()
{
	GcodeTracker.close();
	serialClose(SerialID);

	ADCmutex.lock();
	ADCValue = -1.0;
	std::cout << "Frees destroyed" << std::endl;
	ADCmutex.unlock();
}


void Frees::ADCreadThread(){
	Adafruit_ADS1015 ads;
	uint16_t adc0;

	ads.setGain(GAIN_ONE);
	ads.begin();
	
	//average = ads.readADC_SingleEnded(1); // read A0 once to set average
	while(ADCValue != -1.0){
		adc0 = ads.readADC_SingleEnded(1);  // read A0
		//average = (adc0 + average)/2;
		ADCmutex.lock();
		if(ADCValue != -1.0){
			// update ADCmutex
			ADCValue = adc0;
			ADCmutex.unlock();
			usleep(5000);
		}
		else{
			// exit thread
			ADCmutex.unlock();
		}
	}

	std::cout << "Frees Thread destroyed" << std::endl;
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


void Frees::SendGCode(std::string gcode){
	GcodeTracker << gcode << std::endl;
	// temporary untill we can send gcode to CNC
	std::cout << "Gcode: " << gcode << std::endl;
	
	gcode += "\r\n";
	serialPuts(SerialID, gcode.c_str());

	WaitForGRBLResponse();

	// wait for CNC to start moving
	delay(500);

	do{
		ADCmutex.lock();
		double value = ADCValue;
		ADCmutex.unlock();
		if(value > 59){
			//std::cout << "checking for 50ms" << std::endl;
			int i = 0;
			for(; i < 1000; i++){
				usleep(10);
				ADCmutex.lock();
				double value = ADCValue;
				ADCmutex.unlock();
				if(value > 59){
					// still moving
					//std::cout << "still moving" << std::endl;
					break;
				}
			}
			if(i > 999){
				// not moving for 50ms = stopped
				break;
			}
		}
		else{
			//std::cout << "CNC moving" << std::endl;
		}
	}
	while(true);
	std::cout << "CNC stopped" << std::endl;

	char x;
	std::cin >> x;
}

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