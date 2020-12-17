#pragma once

#include <thread>
#include <mutex>
#include <memory>

#define SENSORPIN A0

#define MaxLengthDiff   32.72
#define PotAtRest      355.00
#define PotAtFull     1645.00
#define MaxPotDiff    (PotAtFull-PotAtRest)   // 773 @ full 219 @ rest => 773-219 = 554

class Sensor
{
private:   
    const double SensorLevel = 15.0; 
    const float ADC2DIST_Fact = MaxLengthDiff / MaxPotDiff;
    
    // static so that ADCreadThread can access after deconstructor
    static double ADCValue;
    static std::mutex ADCmutex;
    static void ADCreadThread();

public:    
    Sensor();
    ~Sensor();

    double GetDistance();
    double GetADCAverage();
};