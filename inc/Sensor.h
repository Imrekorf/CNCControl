#pragma once

#include <thread>
#include <mutex>
#include <memory>

#include "Frees.h"

#include "ads1015/Adafruit_ADS1015.h"

#define SENSORPIN A0

#define MaxLengthDiff   41.36
#define PotAtRest       27.00
#define PotAtFull     1645.00
#define MaxPotDiff    (PotAtFull-PotAtRest)   // 773 @ full 219 @ rest => 773-219 = 554

class Sensor
{
private:   
    // TODO: 
    const double SensorLevel = 5.0;
    Vec3<double> StartPos;
    const Vec3<double> Offset = {2, 2, 0};    
    const float ADC2DIST_Fact = MaxLengthDiff / MaxPotDiff;
    
    // static so that ADCreadThread can access after deconstructor
    static double ADCValue;
    static std::mutex ADCmutex;
    static void ADCreadThread();

protected: 
    Frees frees;

public:    
    Sensor(Vec3<double> StartPos);
    Sensor(Vec3<double> StartPosFrees, Frees &F);
    Sensor();
    ~Sensor();

    void StartPosFrees(Vec3<double> StartPosFrees);
    void SetFrees(Frees &F);

    double GetDistance();
    double GetADCAverage();
};