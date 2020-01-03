#include "Arduino.h"
#include "ADC_SEQR.h"

  
uint16_t Adc_Seqr::global_ADCounts_Array[] = {0};

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

void Adc_Seqr::begin(){
  pmc_enable_periph_clk(ID_ADC);   //power management controller told to turn on adc
  ADC->ADC_CR |=1; //reset the adc
  ADC->ADC_MR= 0x9038FF00;  //default setting for  ADC_MR register :
  //USEQ = 1;
  //TRANSFER = 1;
  //TRACKTIM = 0;
  //ANACH = 0;
  //SETTLING = 3;
  //STARTUP = 8;
  //PRESCAL = 255;
  //FREERUN = 0;
  //FWUP = 0;
  //SLEEP = 0;
  //LOWRES = 0;
  //TRGSEL = 0;
  //TRGEN = 0;

  ADC->ADC_CHDR=0xFFFFFFFF;   // disable all channels   
  ADC->ADC_CHER=0x3cff;       //   use channels A0 to A11 //

  ADC->ADC_MR |=0x80000000;   //USEQ bit set, saying use the sequence

  ADC->ADC_SEQR1=0x01234567;  // use A0 to A7 in order into array 
  ADC->ADC_SEQR2=0x00dcba00;      //use A8 to A11 following in order into array
  
  NVIC_EnableIRQ(ADC_IRQn); // interrupt controller set to enable adc.
  ADC->ADC_IDR=~((1<<27)); // interrupt disable register, disables all interrupts but ENDRX
  ADC->ADC_IER=(1<<27);   // interrupt enable register, enables only ENDRX
 
 // following are the DMA controller registers for this peripheral
 // "receive buffer address" 
	ADC->ADC_RPR=(uint32_t) global_ADCounts_Array;   // DMA receive pointer register  points to beginning of global_ADCount
	// "receive count" 
	ADC->ADC_RCR=NUM_CHANNELS;  //  receive counter set to 12
	// "next-buffer address"
	ADC->ADC_RNPR=(uint32_t)global_ADCounts_Array; // next receive pointer register DMA global_ADCounts_Arrayfer  points to second set of data 
	// and "next count"
	ADC->ADC_RNCR=NUM_CHANNELS;   //  and next counter is set to 12
	// "transmit control register"
	ADC->ADC_PTCR=1;  // transfer control register for the DMA is set to enable receiver channel requests
	// now that all things are set up, it is safe to start the ADC.....
	ADC->ADC_MR |=0x80; // mode register of adc bit seven, free run, set to free running. starts ADC
	delay(2000); //for stability
}

void Adc_Seqr::printSetup(){
	Serial.print("mode register =            "); Serial.println(_HEX(REG_ADC_MR));
	Serial.print("channel enabled register = "); Serial.println(_HEX(REG_ADC_CHSR));
	Serial.print("sequence register1 =       "); Serial.println(_HEX(REG_ADC_SEQR1));
	Serial.print("sequence register2 =       "); Serial.println(_HEX(REG_ADC_SEQR2));
	Serial.print("interrupts =               "); Serial.println(_HEX(REG_ADC_IMR));
}

uint16_t Adc_Seqr::read(uint8_t pin){
	switch (pin) { //handle 0-11 and A0 to A11 pin, if invalid return 0
		case 0:;
		case 1:;
		case 2:;
		case 3:;
		case 4:;
		case 5:;
		case 6:;
		case 7:;
		case 8:;
		case 9:;
		case 10:;
		case 11: break;
		case 54:;
		case 55:;
		case 56:;
		case 58:;
		case 59:;
		case 60:;
		case 61:;
		case 62:;
		case 63:;
		case 64:;
		case 65:;
		case 66 : pin = pin-54; break;
		default : return 0;
	}

  	uint16_t result = 0;
	result = global_ADCounts_Array[pin];
	return result;
}

void Adc_Seqr::ADCHandler(){     // for the ATOD: re-initialize DMA pointers and count
	
	int f=ADC->ADC_ISR;  //   read the interrupt status register 
	if (f & (1<<27)){ /// check the bit "endrx"  in the status register
	/// set up the "next pointer register" 
	ADC->ADC_RNPR=(uint32_t)global_ADCounts_Array;  // "receive next pointer" register set to global_ADCounts_Array 
	// set up the "next count"
	ADC->ADC_RNCR=NUM_CHANNELS;  // "receive next" counter set to 12
 }
}

void Adc_Seqr::prescaler(uint32_t prsc){
	ADC->ADC_MR &=0xFFFF00FF;   //mode register "prescale" zeroed out. 
	ADC->ADC_MR |= prsc << 8;  //setup the prescale frequency
}

void Adc_Seqr::enable(){
	ADC->ADC_MR |= 0x80000000; //turn on bit 32, sequencer enable
}

void Adc_Seqr::disable(){
	ADC->ADC_MR &= 0x7FFFFFFF; //turn off bit 32, sequencer disable
}

  //interrupt handler
void ADC_Handler(){
    Adc_Seqr::ADCHandler();
}
