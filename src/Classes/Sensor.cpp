#include "Sensor.h"

#include "ads1015/Adafruit_ADS1015.h"
#include <unistd.h>

double Sensor::ADCValue = 0.0;
std::mutex Sensor::ADCmutex;

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

	std::cout << "Sensor Thread destroyed" << std::endl;
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

void Sensor::LevelSensor(Frees& F){
	double SensorValue;
	do{
		F.Move({0, 0, -SensorMiddle});
		SensorValue = GetDistance();
		//std::cout << "PushedIn: " << SensorValue << std::endl;
		StartingHeightFromFreesTop += SensorMiddle;
	}
	while(SensorValue < 2);

	// move sensor head down to SensorLevelAim value
	F.Move({0, 0, -1 * (SensorLevelAim - SensorValue)});
	StartingHeightFromFreesTop += SensorLevelAim - SensorValue;

	// debugging
	//std::cout << "HeightToTop: " << StartingHeightFromFreesTop << std::endl;
	SensorLeveledHeight = GetDistance();
}

double Sensor::GetSensorDifference(){
	return GetDistance() - SensorLeveledHeight;
}

double Sensor::MeasureHeight(double& difference){
	double Diff = GetSensorDifference();
	// only write significant changes to heightmap
	if(fabs(Diff) > 0.1){
		// std::cout << "Difference: " << Diff << std::endl;
		RelativeHeight += Diff;
		difference = Diff;
	}
	else{
		difference = 0;
	}
	return RelativeHeight;
}