#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

#include "ads1015/Adafruit_ADS1015.h"
#include <unistd.h>

class ADCMaster {
private:
    static double ADCValue[4];
    static std::mutex ADCmutex;

    static void ADCreadThread();
public:
	ADCMaster();
	~ADCMaster();

	double GetADCValue(unsigned int channel);
};

/*
template<typename T>
class ADCmanager
{
private:
	static ADCMaster ADCreader;
    static double ADCValue;
    static std::mutex ADCmutex;
    static void ADCreadThread();

    static unsigned int ADC_ID;

    static T* Context;
    static double (T::*ADCValueProcessor)(uint16_t adc0);

public:
    ADCmanager(unsigned int Channel, double (T::*ADCValueProcessor)(uint16_t adc0), T *Context);
    ~ADCmanager();

    static double GetADCValue(){
        ADCmutex.lock();
        double ADCValue_copy = ADCValue;
        ADCmutex.unlock();
        return ADCValue_copy;
    }
};

template<typename T> double ADCmanager<T>::ADCValue = 0.0;
template<typename T> unsigned int ADCmanager<T>::ADC_ID = 0;
template<typename T> std::mutex ADCmanager<T>::ADCmutex;
template<typename T> double (T::*ADCmanager<T>::ADCValueProcessor)(uint16_t adc0);
template<typename T> T* ADCmanager<T>::Context;
template<typename T> ADCMaster ADCmanager<T>::ADCreader;

template<typename T>
ADCmanager<T>::ADCmanager(unsigned int Channel, double (T::*ADCValueProcessor)(uint16_t adc0), T* Context)
{
    ADC_ID = Channel;
    this->Context = Context;
    this->ADCValueProcessor = ADCValueProcessor;
    std::thread ReadADC(ADCreadThread);
	ReadADC.detach();
}

template<typename T>
ADCmanager<T>::~ADCmanager()
{
    ADCmutex.lock();
	ADCValue = -1.0;
	ADCmutex.unlock();
}

template<typename T>
void ADCmanager<T>::ADCreadThread(){
	uint16_t adc0;
	double ProcessedADCValue;

	while(ADCValue != -1.0){
		adc0 = ADCreader.GetADCValue(ADC_ID);
        ProcessedADCValue = (Context->*ADCValueProcessor)(adc0);
		ADCmutex.lock();
		if(ADCValue != -1.0){
			// update ADCmutex
			ADCValue = ProcessedADCValue;
			ADCmutex.unlock();
			usleep(5000);
		}
		else{
			// exit thread
			ADCmutex.unlock();
		}
	}

	std::cout << "ADC Channel " << ADC_ID << " manager thread destroyed" << std::endl;
}
*/