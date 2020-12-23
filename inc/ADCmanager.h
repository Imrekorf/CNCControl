#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

#include "ads1015/Adafruit_ADS1015.h"
#include <unistd.h>

class ADCMaster {
private:
    static double ADCWaarden[4];
    static std::mutex ADCmutex;

	// Leest de ADC elke 10ms uit.
    static void ADCreadThread();
public:
	ADCMaster();
	~ADCMaster();

	double ADCWaarde(unsigned int channel);
};