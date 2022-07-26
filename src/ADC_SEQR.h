#ifndef ADC_SEQR_h
#define ADC_SEQR_h

#ifndef ARDUINO_ARCH_SAM
  	#error "This libraries is for arduino DUE only !!!"
#else
  // SAM-specific code

#define ADC_MR_PRESCALER_VALUE ((ADC->ADC_MR & ADC_MR_PRESCAL_Msk ) >> ADC_MR_PRESCAL_Pos)
#define ADC_MR_STARTUP_VALUE (( ADC->ADC_MR & ADC_MR_STARTUP_Msk )  >> ADC_MR_STARTUP_Pos)
#define ADC_MR_TRANSFER_VALUE (( ADC->ADC_MR & ADC_MR_TRANSFER_Msk ) >> ADC_MR_TRANSFER_Pos)
#define ADC_MR_TRACKTIM_VALUE (( ADC->ADC_MR & ADC_MR_TRACKTIM_Msk ) >> ADC_MR_TRACKTIM_Pos)

#define ADC_MR_PRESCALER_MAX 255
#define ADC_MR_STARTUP_MAX   ADC_MR_STARTUP_SUT960
#define ADC_MR_TRANSFER_MAX  3
#define ADC_MR_TRACKTIM_MAX  15

#define ADC_MR_PRESCALER_SET 100
#define ADC_MR_STARTUP_SET   ADC_MR_STARTUP_MAX
#define ADC_MR_TRANSFER_SET  ADC_MR_TRANSFER_MAX
#define ADC_MR_TRACKTIM_SET  ADC_MR_TRACKTIM_MAX

#define GLOBAL_ARRAY_SIZE 16
#define ALL_ANALOG_CHANNEL A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11
#define SEQR1_PIN_SEQUENCE 0x01234567
#define SEQR2_PIN_SEQUENCE 0xfedcba98
#define INTERNAL_TEMP 15
#define PIN52_Pos 14
#define INTERNAL_TEMP_OFFSET 27
#define INTERNAL_MV_OFFSET 800
#define INTERNAL_TEMP_MV_PER_C 2.65f
#define CLOCK_CYCLE_PER_CONVERSION 21//20 table 45-30 p.1403  ?? but tested 21 on micros()

class Adc_SeqrClass;													 								

static void enableChX(int8_t pin);

template<typename Pin, typename ... PinX>
static void enableChX(Pin pin, PinX ... pinX);


class Adc_SeqrClass  {
public:
	static void begin();
	static void begin(int8_t pin) { init(); enableChX(pin); start(); };
	template<typename ... PinX>
	static void begin(int8_t pin, PinX ... pinX){ init(); enableChX(pin, pinX...); start(); };

	static void ADCHandler();
	static int16_t read(int8_t pin);
	static void prescaler(uint32_t prsc);
	static void printSetup();
	static void enable();
	static void disable();
	static float ADC_sampleRate();
	static void setFrequency(float f);
	static void setTracktim(uint8_t tracktim);

	private:
	static uint8_t numChannels; //number of analog channel active
	static volatile uint16_t global_ADCounts_Array[GLOBAL_ARRAY_SIZE];  // holds the raw data from the analog to digital	
	static void init();
	static void start();
	static int8_t analogPinToChannelPos(int8_t pin);
	static uint8_t getArrayPos( uint8_t pin );
	static uint8_t getEnabledChNum();
	static uint32_t ADC_Startup_Clk();	
	static uint32_t ADC_Tracktim_Clk();
	static uint32_t ADC_Clock_f();
	static uint32_t ADC_Transfer_Clk();
	friend void enableChX(int8_t pin);
};

static void enableChX(int8_t pin){ 
	if(pin==INTERNAL_TEMP) ADC->ADC_ACR |= ADC_ACR_TSON; //enable internal temp sensor
	ADC->ADC_CHER|=(1<<Adc_SeqrClass::analogPinToChannelPos(pin));
};

template<typename Pin, typename ... PinX>
static void enableChX(Pin pin, PinX ... pinX) { enableChX((int8_t)pin); enableChX((int8_t)pinX...); };

float internalTemp();

extern Adc_SeqrClass Adc_Seqr;

#endif //ARDUINO_ARCH_SAM
#endif //ADC_SEQR_h
