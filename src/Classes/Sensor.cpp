#include "Sensor.h"

Sensor::Sensor(ADCMaster* ADCreader) : ADCreader(ADCreader) //(0, &Sensor::ADCValueProcessor, this)
{}

Sensor::~Sensor() {}

double Sensor::GetDistance(){
	return (ADCreader->GetADCValue(ADCChannel) - PotAtRest) * ADC2DIST_Fact;
}

double Sensor::GetADCAverage(){
	return ADCreader->GetADCValue(ADCChannel);
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