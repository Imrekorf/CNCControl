#include "ADCmanager.h"
#include <wiringPi.h>

double ADCMaster::ADCWaarden[4];
std::mutex ADCMaster::ADCmutex;

ADCMaster::ADCMaster(){
    std::cout << "ADCMaster thread started" << std::endl;
    std::thread ReadADC(ADCreadThread);
    ReadADC.detach();
}

ADCMaster::~ADCMaster(){
    ADCmutex.lock();
    ADCWaarden[0] = -1.0;
    ADCmutex.unlock();
}

void ADCMaster::ADCreadThread(){
	// maak een ADS1015 object aan
	Adafruit_ADS1015 ads;
	uint16_t adc0;
	// houdt het gemiddelde voor ADC1 bij.
    static double ADCAverage = 0;

	// stel ADS1015 in.
	ads.setGain(GAIN_ONE);
	ads.begin();

	// lees ADC1 1 keer uit om deze in te stellen voor het gemiddelde
    ADCAverage = ads.readADC_SingleEnded(1);
	while(ADCWaarden[0] != -1.0){
		// lees de ADC kanalen uit
		adc0 = ads.readADC_SingleEnded(0);  // lees A0
		delay(5);
		ADCAverage = (ads.readADC_SingleEnded(1) + ADCAverage) / 2;  // lees A1

		// lock de mutex zodat de ADCWaarden array kan worden aangepast
		ADCmutex.lock();
		// check if thread should exit
		if(ADCWaarden[0] != -1.0){
			// update values
			ADCWaarden[0] = adc0;
			ADCWaarden[1] = ADCAverage;
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

double ADCMaster::ADCWaarde(unsigned int channel){
	ADCmutex.lock();
	double ADCCopy = ADCWaarden[channel];
	ADCmutex.unlock();
	return ADCCopy;
}

