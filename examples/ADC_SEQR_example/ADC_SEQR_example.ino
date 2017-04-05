#include <Streaming.h>
#include <ADC_SEQR.h>

void setup() {
  Serial.begin(9600); 
  Adc_Seqr::begin(1); //initiate sequencer, have to be before 'analogReadResolution' call to not reset it to 10
  analogReadResolution(12);
  Adc_Seqr::printSetup();
}

void loop() {
  unsigned int reading[12];
  for (int i = 0; i < 12; i++){
    reading[i] = Adc_Seqr::read(i);
    Serial << "reading " << i  << " = " << reading[i] << endl;
  }
 // Serial << endl;
    delay(500);
}
