#ifndef ADC_SEQR_h
#define ADC_SEQR_h

#if defined(ARDUINO_ARCH_AVR)
  	#error "This libraries is for arduino DUE only !!!"
#elif defined(ARDUINO_ARCH_SAM)
  // SAM-specific code


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

#else
  #error "No SAM3x (arduino DUE) achitechture detected !!!"
#endif
	
#endif
