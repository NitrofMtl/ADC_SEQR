#include "Arduino.h"
#include "ADC_SEQR.h"
  
volatile uint16_t Adc_SeqrClass::global_ADCounts_Array[] = {0};
uint8_t Adc_SeqrClass::numChannels = GLOBAL_ARRAY_SIZE;

/*SAM3X ADC_MR Register, refer to data sheet:43.7.2 ADC Mode Register 

	31 	|	30	|	29 28	|	 27 26 25 24
USEQ 		-	  TRANSFER 		  TRACKTIM

	23 	|	22	|	21 20	|	 19 18 17 16
  ANACH		– 	  SETTLING 		   STARTUP

|	15	14	13	12		11	10	9	8	|
				PRESCAL

	7	|	6	|	5	|	4	|	321	|	0
FREERUN 	FWUP  SLEEP   LOWRES  TRGSEL  TRGEN
*/

void Adc_SeqrClass::init()
{
	pmc_enable_periph_clk(ID_ADC);   //power management controller told to turn on adc
	ADC->ADC_CR = ADC_CR_SWRST; //reset the adc
	ADC->ADC_MR = 0; //reset register
	ADC->ADC_MR |= ADC_MR_USEQ
				|  ADC_MR_TRANSFER(ADC_MR_TRANSFER_SET)
				|  ADC_MR_TRACKTIM(ADC_MR_TRACKTIM_SET)
				//|  ADC_MR_SETTLING_AST3  //ADC_MR_SETTLING(0)
				|  ADC_MR_STARTUP_SET
				|  ADC_MR_PRESCAL(ADC_MR_PRESCALER_SET)
				|  ADC_MR_TRANSFER(ADC_MR_TRANSFER_SET);
	ADC->ADC_CHDR = -1ul;   // disable all channels
	ADC->ADC_SEQR1 = SEQR1_PIN_SEQUENCE;  // use A0 to A7 in order into array 
	ADC->ADC_SEQR2 = SEQR2_PIN_SEQUENCE;//0x00fedcba;      //use A8 to A11, 52 and INTERNAL_TEMP following in order into array
}

void Adc_SeqrClass::start()
{
	numChannels = getEnabledChNum(); //set how namy channel will be on buffer
	NVIC_EnableIRQ(ADC_IRQn); // interrupt controller set to enable adc.
	ADC->ADC_IDR = ~ADC_IDR_ENDRX; // interrupt disable register, disables all interrupts but ENDRX
	ADC->ADC_IER = ADC_IDR_ENDRX;   // interrupt enable register, enables only ENDRX
 
 // following are the DMA controller registers for this peripheral
 // "receive buffer address" 
	ADC->ADC_RPR = (uint32_t)global_ADCounts_Array;   // DMA receive pointer register  points to beginning of global_ADCount
	// "receive count" 
	ADC->ADC_RCR = numChannels;  //  receive counter set
	// "next-buffer address"
	ADC->ADC_RNPR = (uint32_t)global_ADCounts_Array; // next receive pointer register DMA global_ADCounts_Arrayfer  points to second set of data 
	// and "next count"
	ADC->ADC_RNCR = numChannels;   //  and next counter is set
	// "transmit control register"
	ADC->ADC_PTCR = ADC_PTCR_RXTEN;  // transfer control register for the DMA is set to enable receiver channel requests
	// now that all things are set up, it is safe to start the ADC.....
	ADC->ADC_MR |= ADC_MR_FREERUN;//0x80; // mode register of adc bit seven, free run, set to free running. starts ADC
	delay(100); //for stability
}


void Adc_SeqrClass::begin(){
	init();

	ADC->ADC_ACR |= ADC_ACR_TSON; // enable internal temp sensor
	ADC->ADC_CHER  = ADC_CHER_CH0 
					| ADC_CHER_CH1 
					| ADC_CHER_CH2 
					| ADC_CHER_CH3 
					| ADC_CHER_CH4 
					| ADC_CHER_CH5 
					| ADC_CHER_CH6 
					| ADC_CHER_CH7
					//| ADC_CHER_CH8 //ch8 8 and 9 are nor anable by default
					//| ADC_CHER_CH9 //if anable, I2C0 on pin  21 and 22 is disable
					| ADC_CHER_CH10 
					| ADC_CHER_CH11 
					| ADC_CHER_CH12 
					| ADC_CHER_CH13
					| ADC_CHER_CH14
					| ADC_CHER_CH15;
	start();
}

int8_t Adc_SeqrClass::analogPinToChannelPos(int8_t pin)
{
	switch (pin) { //handle 0-11 and A0 to A11 pin, if invalid return -2
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7: break;
		case 8: pin = 0xa; break;
		case 9:  pin = 0xb; break;
		case 10: pin = 0xc; break;
		case 11: pin = 0xd; break;
		case INTERNAL_TEMP: pin = INTERNAL_TEMP; break;
		case 20: pin = 8; break;
		case 21: pin = 9; break;		
		case 52: pin = PIN52_Pos; break;
		case A0:;
		case A1:;
		case A2:;
		case A3:;
		case A4:;
		case A5:;
		case A6:;
		case A7:;
		case A8:;
		case A9:;
		case A10:;
		case A11: pin = analogPinToChannelPos(pin -= A0); break;
		default: pin = -2;
		
	}
	return pin;
}

uint8_t Adc_SeqrClass::getEnabledChNum()
{ //found how many channel are set
	uint8_t numChannels = 0;
	for (int i = 0; i < 16; i++){
		if ( ADC->ADC_CHSR & (1<<i) ) numChannels++;
	}
	return numChannels;
}

void Adc_SeqrClass::printSetup()
{
	Serial.println("ADC configs:");
	Serial.print("ADC sample rate Hz =     "); Serial.println(ADC_sampleRate());
	Serial.print("ADC Clock KHz =          "); Serial.println(ADC_Clock_f());
	Serial.print("mode register =          "); Serial.println(REG_ADC_MR, BIN);
	Serial.print("channel enabled register="); Serial.println(REG_ADC_CHSR, BIN);
	Serial.print("sequence register1 =     "); Serial.println(REG_ADC_SEQR1, HEX);
	Serial.print("sequence register2 =     "); Serial.println(REG_ADC_SEQR2, HEX);
	Serial.print("Prescaler =              "); Serial.println(ADC_MR_PRESCALER_VALUE);
	Serial.print("Startim =                "); Serial.print(ADC_MR_STARTUP_VALUE); Serial.print(" "); Serial.print(ADC_Startup_Clk()); Serial.println(" ADC clk");
	Serial.print("TrackTim =               "); Serial.print(ADC_MR_TRACKTIM_VALUE); Serial.print(" "); Serial.print(ADC_Tracktim_Clk()); Serial.println(" ADC clk");
	Serial.print("Transfert =              "); Serial.print(ADC_MR_TRANSFER_VALUE); Serial.print(" "); Serial.print(ADC_Transfer_Clk()); Serial.println(" ADC clk");
}

int16_t Adc_SeqrClass::read(int8_t pin)
{
	pin = analogPinToChannelPos(pin);
	if ( pin == -2) {
		return pin;
	}
	if( !(ADC->ADC_CHSR & (1<<pin)) ) {
		return -1; //return disable pin
	}
	uint8_t pos = getArrayPos(pin);	
	return global_ADCounts_Array[pos];
}

uint8_t Adc_SeqrClass::getArrayPos( uint8_t pin )
{
  uint8_t pos = 0;
  for (uint8_t i=0; i<pin; i++){  //loop add a position in buffer for each active pin in ADC_CHSR
    if ( ADC->ADC_CHSR & (1<<i) ) pos++; 
  }
  return pos;
}

void Adc_SeqrClass::ADCHandler()
{     // for the ATOD: re-initialize DMA pointers and count		
	//   read the interrupt status register 
	ADC->ADC_ISR & ADC_ISR_ENDRX; /// check the bit "endrx"  in the status register
	ADC->ADC_RNPR = (uint32_t)global_ADCounts_Array;  // "receive next pointer" register set to global_ADCounts_Array 
	ADC->ADC_RNCR = numChannels;  // "receive next" counter
}

void Adc_SeqrClass::prescaler(uint32_t prsc)
{
	ADC->ADC_MR &= ~(ADC_MR_PRESCAL_Msk << ADC_MR_PRESCAL_Pos);   //mode register "prescale" zeroed out.
	ADC->ADC_MR |= ADC_MR_PRESCAL(prsc);  //setup the prescale frequency
}

void Adc_SeqrClass::setFrequency(float f)
{
	float adcClock = f * CLOCK_CYCLE_PER_CONVERSION * getEnabledChNum();
	prescaler(VARIANT_MCK / adcClock / 2 -1);
}

void Adc_SeqrClass::setTracktim(uint8_t tracktim) {
	ADC->ADC_MR &= ~(ADC_MR_TRACKTIM_Msk << ADC_MR_TRACKTIM_Pos);//(ADC_MR_TRACKTIM(MAX_MASK));
	ADC->ADC_MR |= ADC_MR_TRACKTIM(tracktim);
}

void Adc_SeqrClass::enable()
{
	ADC->ADC_MR |= ADC_MR_FREERUN;
}

void Adc_SeqrClass::disable()
{
	ADC->ADC_MR &= ~ADC_MR_FREERUN;
}

  //interrupt handler
void ADC_Handler()
{
    Adc_SeqrClass::ADCHandler();
}

float Adc_SeqrClass::ADC_sampleRate()
{
	return static_cast<float>(ADC_Clock_f()) / numChannels / CLOCK_CYCLE_PER_CONVERSION;
}

uint32_t Adc_SeqrClass::ADC_Startup_Clk()
{
	return ( ADC->ADC_MR & ADC_MR_STARTUP_Msk )  >> ADC_MR_STARTUP_Pos;
}

uint32_t Adc_SeqrClass::ADC_Tracktim_Clk()
{
	return ADC_MR_TRACKTIM_VALUE +1;
}

uint32_t Adc_SeqrClass::ADC_Clock_f()
{
	return VARIANT_MCK / ((ADC_MR_PRESCALER_VALUE +1) *2);
}

uint32_t Adc_SeqrClass::ADC_Transfer_Clk()
{
	return ADC_MR_TRANSFER_VALUE *2 +3;
}

Adc_SeqrClass Adc_Seqr;

float internalTemp()
{ // convert the sam3x internal sensor temp
	if ( !(ADC->ADC_CHSR & ADC_CHSR_CH15) ) return 0;  // return 0 if internal temps ch is disable
	float tVolt = Adc_Seqr.read(INTERNAL_TEMP) * 3300. / 4095;
	return (tVolt-INTERNAL_MV_OFFSET) / INTERNAL_TEMP_MV_PER_C + INTERNAL_TEMP_OFFSET;
}

