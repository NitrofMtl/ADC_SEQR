#ifndef ADC_SEQR_h
#define ADC_SEQR_h

#if (ARDUINO >= 100)
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <Streaming.h>
  
class Adc_Seqr  {
private:	
	static const int NUM_CHANNELS=12; //number of analog channel active
	static uint16_t global_ADCounts_Array[12];  // holds the raw data from the analog to digital
	
public:
	static void begin();
	static void ADCHandler();
	static uint16_t read(uint8_t pin);

	static void prescaler(uint32_t prsc);
	static void printSetup();
	static void enable();
	static void disable();
};

#endif
