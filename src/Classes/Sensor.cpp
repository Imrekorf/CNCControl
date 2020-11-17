#include "Sensor.h"

#include <unistd.h>

double Sensor::ADCValue = 0.0;
std::mutex Sensor::ADCmutex;

Sensor::Sensor(Vec3<double> StartPos) : StartPos(StartPos), frees(StartPos - Offset) {
	std::thread ReadADC(ADCreadThread);
	ReadADC.detach();
}

Sensor::Sensor(Vec3<double> StartPosFrees, Frees &F) : StartPos(StartPosFrees + Offset), frees(F) {
	std::thread ReadADC(ADCreadThread);
	ReadADC.detach();
}

Sensor::Sensor() {
	std::thread ReadADC(ADCreadThread);
	ReadADC.detach();
}

Sensor::~Sensor() {
	ADCmutex.lock();
	ADCValue = -1.0;
	std::cout << "sensor destroyed" << std::endl;
	ADCmutex.unlock();
}

void Sensor::StartPosFrees(Vec3<double> StartPosFrees){
	this->StartPos = StartPosFrees + Offset;
}
void Sensor::SetFrees(Frees &F){
	this->frees = F;
}

void Sensor::ADCreadThread(){
	Adafruit_ADS1015 ads;
	uint16_t adc0;
	double average;

	ads.setGain(GAIN_ONE);
	ads.begin();
	
	average = ads.readADC_SingleEnded(0); // read A0 once to set average
	while(ADCValue != -1.0){
		adc0 = ads.readADC_SingleEnded(0);  // read A0
		average = (adc0 + average)/2;
		ADCmutex.lock();
		if(ADCValue != -1.0){
			// update ADCmutex
			ADCValue = average;
			ADCmutex.unlock();
			usleep(5000);
		}
		else{
			// exit thread
			ADCmutex.unlock();
		}
	}

	std::cout << "Thread destroyed" << std::endl;
}

double Sensor::GetDistance(){
	double distance;
		ADCmutex.lock();
		distance = (ADCValue-PotAtRest)*ADC2DIST_Fact;
		//distance = ADCValue;
		ADCmutex.unlock();
	return distance;
}

double Sensor::GetADCAverage(){
	double ADCAverage;
		ADCmutex.lock();
		ADCAverage = ADCValue;
		ADCmutex.unlock();
	return ADCAverage;
}