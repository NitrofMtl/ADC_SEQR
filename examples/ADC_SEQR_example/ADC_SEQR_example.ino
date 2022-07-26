#include <ADC_SEQR.h>

void printAdcFcy();

void setup() {
  Serial.begin(115200);
  Serial.available();
  delay(1000);

//initiate sequencer, have to be before 'analogReadResolution' call to not reset it to 10
  //Adc_Seqr::begin(); //enable all ADC channels A0 to A11, pin 52,  and INTERNAL_TEMP sensor
  //Adc_Seqr::begin(ALL_ANALOG_CHANNEL); //enable all A0 to A12 ordinary DUE analog
  Adc_Seqr.begin(A0, A1, 3, A7, 52, 20, INTERNAL_TEMP); //initialate sequencer on any number of pin needed, A0 to A11, 52, 20(SDA0), 21(SCL) and INTERNAL_TEMP, order do not matter

////////// CAUTION !!! if you enable analog pin 20 or 21, I2C0 will be disale
////////// CAUTION !!! Also, pin 20 and 21 have harware 1k5 pullup resistor, it have to be in design consideration.


  analogReadResolution(12);  //10 or 12, default 12

  //To set the sampling frequency, set the prescaler
  //Adc_Seqr.prescaler(ADC_MR_PRESCALER_MAX); //maximum prescaler is 255
  //Or use set frequency function, must be call after begin
  Adc_Seqr.setFrequency(1000);
}

bool disabled = 0;
int counter = 0;

void loop() {
  int reading[12];
  for (int i = 0; i < 12; i++){
    reading[i] = Adc_Seqr.read(i); //disable pin return -1
    Serial.print("reading ");
    Serial.print(i); Serial.print(" = "); 
    Serial.println(reading[i]);
  }
  Serial.print("reading "); 
  Serial.print(52); Serial.print(" = "); 
  Serial.println(Adc_Seqr.read(52));

  Serial.print("internal temps ="); 
  Serial.println(Adc_Seqr.read(INTERNAL_TEMP)); 
  Serial.print(internalTemp()); 
  Serial.println("celcius");
  printAdcFcy();

  if (counter > 4) {
    if (!disabled) {
      disabled = 1;
      Adc_Seqr.disable();
      Serial.println("disable");

    } else {
      disabled = 0;
      Adc_Seqr.enable();
      Serial.println("enable");

    }
    counter = 0;
  }
  counter++;

  delay(1000);  
}



//estimated frequency
void printAdcFcy(){
  float f = Adc_Seqr.ADC_sampleRate();
  Serial.print("Adc sample frequency: ");
  if (f >= 1000) {
    f/=1000;
    Serial.print(f);
    Serial.println("KHz");
  }
  else {
    Serial.print(f);
    Serial.println("Hz");
  }  
}