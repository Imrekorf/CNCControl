#include "ADCmanager.h"
#include <wiringPi.h>

double ADCMaster::ADCValue[4];
std::mutex ADCMaster::ADCmutex;

ADCMaster::ADCMaster(){
    std::cout << "ADCMaster thread started" << std::endl;
    std::thread ReadADC(ADCreadThread);
    ReadADC.detach();
}

ADCMaster::~ADCMaster(){
    ADCmutex.lock();
    ADCValue[0] = -1.0;
    ADCmutex.unlock();
}

void ADCMaster::ADCreadThread(){
	Adafruit_ADS1015 ads;
	uint16_t adc0; //, adc2, adc3;
    static double ADCAverage = 0;

	ads.setGain(GAIN_ONE);
	ads.begin();

    ADCAverage = ads.readADC_SingleEnded(1);
	while(ADCValue[0] != -1.0){
		adc0 = ads.readADC_SingleEnded(0);  // read A0
		delay(5);
		ADCAverage = (ads.readADC_SingleEnded(1) + ADCAverage) / 2;  // read A1
		std::cout << adc0 << " " << ADCAverage << std::endl;

		//adc2 = ads.readADC_SingleEnded(2);  // read A2
		//adc3 = ads.readADC_SingleEnded(3);  // read A3
		ADCmutex.lock();
		if(ADCValue[0] != -1.0){
			// update ADCmutex
			ADCValue[0] = adc0;
			ADCValue[1] = ADCAverage;
			//ADCValue[2] = adc2;
			//ADCValue[3] = adc3;
			ADCmutex.unlock();
			usleep(5000);
		}
		else{
			// exit thread
			ADCmutex.unlock();
		}
	}

	std::cout << "ADCMaster Thread destroyed" << std::endl;
}

double ADCMaster::GetADCValue(unsigned int channel){
	ADCmutex.lock();
	double ADCCopy = ADCValue[channel];
	ADCmutex.unlock();
	return ADCCopy;
}

