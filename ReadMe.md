# Arduino DUE ADC_SEQR

## ADC_SEQUR enable the Sam3x adc sequencer PDC


# Feature

-Faster analog reading: no wake up time, ADC stay on all the time.  
-Free up MCU: micro controller do not wait for conversion, it is done in background and result conversion data is render trough interrupt routine.  
-Fast sample rate: conversion are time with ADC clock set by prescaler, clock going from 164KHz and 48Mhz.  
-Enable the usage of pin 52, 20 and 21 for another analog input.  
-Enable internal temp sensor  
-Could activate any wanted number of analog pin.  

# Limitation
-Final conversion sample time not known. Some test have been made with micros() chronos to give an estimation of the sample rate but the result given by the ADC_sampleRate() function are for reference only, not for critical timing use. 
-Disable and re-enable function are not fully tested yet.  
-Temp sensor is not accurate: it is more to be use for alarm for extreme temp (-40, +60C ).  A calibration routine could also be added.  

# Version 3.0
-Add preinstantioated object
-Add suppot to use SAM3x AD08, AD09 (SDA0 pin 20, SCL0 pin 21 )
-Fix conlict with I2C0 and analog pin 8 and 9
-Fix begin with only one argument

# Version 2.1
-Fix begin(...) arguments cast compile error when mixing Ax analog pin and integer 
-Add function to return approximative ADC sample rate

# Usage

ADC_SEQR is a rapper of static member variable and function so user could or not create an instance or use it with scope operator


~~-The user could create an instance if wanted:~~

```
~~Adc_Seqr adc_seqr;~~
```

~~If an instance of the library is created, any function could be call with the "." operator. Without, use scope "::" operator:~~
A preinstantiated object is already been set:

```
Adc_Seqr.begin();

```

## example:

-First include header:
```
#include <ADC_SEQR.h>
```

-Then, setup the sequencer, two begin function are available:
  -without argument: enable all adc channel A0 to A11, pin 52, pin 20, pin 21 and internal temp sensor.

**ATTENTION** 

*-if you enable analog pin 20 or 21, I2C0 will be disale*

*-Also, pin 20 and 21 have harware 1k5 pullup resistor, it have to be in design consideration.*

```
void setup(){
	Serial.begin(9600);
	Adc_Seqr.begin();
}
```

 -Or, active only the pin wanted, begin function could take any number of argument from 1 pin to all of them. order do not matter.  
 Argument could be in analog pin name or integer. pin 52 is only take integer argument. Internal temp sensor have a dedicated constant.
```
void setup(){
	Serial.begin(9600);
	Adc_Seqr.begin(A0, 8, 4, A6, INTERNAL_TEMP);
//other configs to come before closing setup
```

 -Other configuration could be set, analog resolution, and prescaler to aduste ADC clock:
 ```
 analogReadResolution(10); //default is 12
 prescaler(127); //0-255
 // ADC clock = (Master clock 84MHz) / 2(prescaler+1)
 ```
 
 -And proximative analog sample rate can be given in Hz:
 ```
 int adcSampleRate = Adc_Seqr.ADC_sampleRate();
 ```
 
 -If wanted, ADC config and some register info could be printed:
 ```
 	Adc_Seqr.printSetup();
 } //now void setup() is ready to be close
 ```
 
 -To read an input call read(pin) function, support the same input than begin function:
 ```
 int a = Adc_Seqr.read(0);
 int b = Adc_Seqr.read(A6);
 int b = Adc_Seqr.read(52);
 ```
 
 -get the internal temp, it is a global function, no scope operator needed:
 ```
 int internalTemperature = internalTemp();
 ```
 
 -disale and re-enable the sequencer
 ```
 Adc_Seqr.disable();
 Adc_Seqr.enable();
 ```