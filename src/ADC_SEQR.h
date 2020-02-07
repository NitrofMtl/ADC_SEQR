#ifndef ADC_SEQR_h
#define ADC_SEQR_h

#if defined(ARDUINO_ARCH_AVR)
  	#error "This libraries is for arduino DUE only !!!"
#elif defined(ARDUINO_ARCH_SAM)
  // SAM-specific code

#ifndef BIT_FIELD
#define BIT_FIELD(field)   0xFFFFFFFF >> (32-field)
#define MAX_FIELD (uint32_t)-1
#endif

#ifndef ADC_MR_FIELD
#define ADC_MR_FIELD
#define ADC_MR_TRANSFER(x) ( x & BIT_FIELD(2) ) << 28
#define ADC_MR_TRACKTIM(x) ( x & BIT_FIELD(4) ) << 24
#define ADC_MR_SETTLING(x) ( x & BIT_FIELD(2) ) << 20
#define ADC_MR_STARTUP(x)  ( x & BIT_FIELD(4) ) << 16
#define ADC_MR_PRESCAL(x)  ( x & BIT_FIELD(8) ) << 8
#define ADC_MR_TRGSEL(x)   ( x & BIT_FIELD(3) ) << 1
#define MAX_FIELD (uint32_t)-1
#endif

#define INTERNAL_TEMP 13

#define CLOCK_CYCLE_PER_CONVERSION 20

								  							        
static void enableChX(uint8_t pin){ 
	if(pin==52) pin==14; //PIN 52 (AD14)  
	else if(pin>=A0) pin-=A0;  //shift analog pin to int
	else if (pin>7 && pin!=14) pin+=2; //Shift 2 bit for the 2 adc channel (8,9)not use in samx
	if(pin>15) return;  //ignore input bigger than register
	if(pin==INTERNAL_TEMP+2) ADC->ADC_ACR |= ADC_ACR_TSON; //enable internal temp sensor
	ADC->ADC_CHER|=(1<<pin);
};
														 								

template<typename Pin, typename ... PinX>
static void enableChX(Pin pin, PinX ... pinX) { enableChX((uint8_t)pin); enableChX((uint8_t)pinX...); };


class Adc_Seqr  {
public:
	static void begin();
	template<typename ... PinX>
	static void begin(PinX ... pinX){ init(); enableChX(pinX...); start(); };
	static void ADCHandler();
	static uint16_t read(uint8_t pin);
	static void prescaler(uint32_t prsc);
	static void printSetup();
	static void enable();
	static void disable();
	static uint32_t ADC_sampleRate();
	static void setTracktim(uint8_t tracktim);

	private:

	static uint8_t numChannels; //number of analog channel active
	static const uint8_t MAX_NUM_CHANNELS = 14;
	static volatile uint16_t global_ADCounts_Array[MAX_NUM_CHANNELS];  // holds the raw data from the analog to digital	
		
	static void init();
	static void start();

	static uint8_t getArrayPos( uint8_t pin = 0 );
	static uint8_t getSettedCh();
};

float internalTemp();


#else
  #error "No SAM3x (arduino DUE) achitechture detected !!!"
#endif
	
#endif


