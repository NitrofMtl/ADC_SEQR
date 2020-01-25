#include <ADC_SEQR.h>


void setup() {
  Serial.begin(115200);
  Serial.available(); Serial.println();
//initiate sequencer, have to be before 'analogReadResolution' call to not reset it to 10
  //Adc_Seqr::begin(); //enable all ADC channels A0 to A11, pin 52 and INTERNAL_TEMP sensor
  Adc_Seqr::begin(A0, A1, 3, A7, 52, INTERNAL_TEMP); //initialate sequencer on any number of pin needed, A0 to A11, 52 and INTERNAL_TEMP, order do not matter
  analogReadResolution(12);
  Adc_Seqr::prescaler(127);

}


void loop() {
  unsigned int reading[12];
  for (int i = 0; i < 12; i++){
    reading[i] = Adc_Seqr::read(i);
    Serial.print("reading "); 
    Serial.print(i); Serial.print(" = "); 
    Serial.println(reading[i]);
  }
  Serial.print("reading "); 
  Serial.print(52); Serial.print(" = "); 
  Serial.println(Adc_Seqr::read(52));

  Serial.print("internal temps ="); 
  Serial.println(Adc_Seqr::read(INTERNAL_TEMP)); 
  Serial.print(internalTemp()); 
  Serial.println("celcius");
  printAdcFcy();

  delay(1000);  
}



//estimated frequency
void printAdcFcy(){

  double f = Adc_Seqr::ADC_sampleRate();
  Serial.print("Adc sample frequency: ");
  if (f >= 1000) {
    f/=1000;
    Serial.print(f,3);
    Serial.println("KHz");
  }
  else {
    Serial.print(f,3);
    Serial.println("Hz");
  } 
}

