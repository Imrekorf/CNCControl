#pragma once

#include <iostream>

#include "ADCmanager.h"
#include "Frees.h"

#define SENSORPIN A0

#define MaxLengthDiff   32.72
#define PotAtRest      355.00
#define PotAtFull     1645.00
#define MaxPotDiff    (PotAtFull-PotAtRest)   // 773 @ full 219 @ rest => 773-219 = 554
#define SensorMiddle    15.00

class Sensor
{
private:   
    const double SensorLevelAim = 15.0; 
    const float ADC2DIST_Fact = MaxLengthDiff / MaxPotDiff;
    double SensorLeveledHeight;
    double StartingHeightFromFreesTop = 0;
    double RelativeHeight = 0;
    
    ADCMaster* ADCreader;
    const unsigned int ADCChannel = 1;


public:    
    Sensor(ADCMaster* ADCreader);
    ~Sensor();

    double GetDistance();
    double GetADCAverage();

    void LevelSensor(Frees& F);
    double GetSensorDifference();
    double MeasureHeight(double& difference);

};