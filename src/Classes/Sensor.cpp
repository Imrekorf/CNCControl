#include "Sensor.h"

Sensor::Sensor(ADCMaster* ADClezer) : ADClezer(ADClezer) {}

Sensor::~Sensor() {}

double Sensor::AbsoluteHoogteMeting(){
	return (ADClezer->ADCWaarde(ADCChannel) - PotInRust) * ADC2DIST_Fact;
}

double Sensor::ADCGemiddelde(){
	return ADClezer->ADCWaarde(ADCChannel);
}

void Sensor::LevelSensor(Frees& F){
	// beweeg de sensor naar beneden totdat deze een waarde > 2 meet.
	double SensorWaarde;
	do{
		F.Beweeg({0, 0, -SensorMidden});
		SensorWaarde = AbsoluteHoogteMeting();
		StartingHoogteTotCNCTop += SensorMidden;
	}
	while(SensorWaarde < 2);

	// Beweeg de sensor verder naar beneden tot SensorLevelDoel
	F.Beweeg({0, 0, -1 * (SensorLevelDoel - SensorWaarde)});
	StartingHoogteTotCNCTop += SensorLevelDoel - SensorWaarde;
	
	// Bewaar de gelevelde SensorWaarde om later mee te vergelijken.
	SensorLeveledHoogte = AbsoluteHoogteMeting();
}

double Sensor::SensorVerschil(){
	return AbsoluteHoogteMeting() - SensorLeveledHoogte;
}

double Sensor::RelatieveHoogteMeting(double& difference){
	double Diff = SensorVerschil();
	// Verwerk alleen groot genoege hoogte verschillen.
	if(fabs(Diff) > 0.1){
		RelatieveHoogte += Diff;
		difference = Diff;
	}
	else{
		difference = 0;
	}
	return RelatieveHoogte;
}