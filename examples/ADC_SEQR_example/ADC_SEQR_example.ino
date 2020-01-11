#include <ADC_SEQR.h>


void setup() {
  Serial.begin(115200);
  Serial.available(); 

  //Adc_Seqr::begin(); //initiate sequencer, have to be before 'analogReadResolution' call to not reset it to 10
  Adc_Seqr::begin(A0, A9, A7, A10); //initialate sequencer on any number of pin needed, A0 to A11, 52 and INTERNAL_TEMP, order do not matter
  analogReadResolution(12);
  Adc_Seqr::printSetup();
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
  delay(1000);  

}

