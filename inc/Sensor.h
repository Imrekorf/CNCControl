#pragma once

#include <iostream>

#include "ADCmanager.h"

// Maximaal sensor verschil
#define MaxLengthDiff   27.59
// ADC waarde in rust
#define PotInRust      503.00
// ADC waarde wanneer maximaal ingedrukt
#define PotIngedrukt  2047.00
// Hoeveelheid dat de sensor maximaal naar beneden beweegt in 1 keer tijdens SensorLevel.
#define SensorMidden    15.00
// 2047 @ full 500 @ rest => 2047-500 = 1547
#define MaxPotDiff    (PotIngedrukt-PotInRust)

class Frees;

class Sensor
{
private:   
    const double SensorLevelDoel = 15.0; 
    const float ADC2DIST_Fact = MaxLengthDiff / MaxPotDiff;
    double SensorLeveledHoogte;
    double StartingHoogteTotCNCTop = 0;
    double RelatieveHoogte = 0;
    
    ADCMaster* ADClezer;
    const unsigned int ADCChannel = 1;


public:    
    Sensor(ADCMaster* ADClezer);
    ~Sensor();
    
    // Lees hoeveel de sensor is ingedrukt.
    double AbsoluteHoogteMeting();
    // Lees de ADC waarde.
    double ADCGemiddelde();

    // Zet de sensor in het midden
    void LevelSensor(Frees& F);
    
    // Geeft het hoogte verschil ten opzichte van de gelevelde sensor waarde.
    double SensorVerschil();
    // Meet de relatieve hoogte ten opzichte van het eerste meet punt.
    double RelatieveHoogteMeting(double& difference);

};