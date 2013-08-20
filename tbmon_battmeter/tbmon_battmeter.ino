#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

int vinPin = 23; //Teensy 3.0 A9
int voltsD = 0; //ADC voltage value
int adcRes = 16; //adcResolution
float voltsA = 0; //Calculated analog voltage value
float voltsPC = 3.3 / 65536; //(2^adcRes); //Volts per ADC count. vRef / max ADC resolution
float voltScale = 25.3 / 3.3; //Max voltage for divider to = vRef / vRef

void setup() {
  Serial.begin(9600);
    delay(5000);
 
  // Setup pins / SPI.
  Mirf.cePin = 9;
  Mirf.csnPin = 10;

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  
 
  // Configure reciving address. 
  Mirf.setRADDR((byte *)"volt1");

  Mirf.payload = sizeof(unsigned long);

  /*
   * To change channel:
   * 
   * Mirf.channel = 10;
   *
   * NB: Make sure channel is legal in your area.
   */
   
  Mirf.config();
  
  //config the ADC
  //ADC0_SC2 = ADC_SC2_REFSEL(0); // select the internal reference
  ADC0_SC3 =  1024;  // setup averaging
  analogReadRes(adcRes); //set ADC resolution
  
  Serial.println("Beginning ... "); 

}

void loop() {

  voltsD = analogRead(vinPin);
  voltsA = voltsD * voltsPC * voltScale;
  
  Serial.print("Digital voltage value: ");
  Serial.println(voltsD);
  
  Serial.print("Analog voltage value: ");
  Serial.println(voltsA); 
  
  unsigned long time = millis();
  
  Mirf.setTADDR((byte *)"tbmn1"); // set target
  Mirf.send((byte *)&voltsA); // send data
  
  // wait for data to finish sending
  while(Mirf.isSending()){
  }
  Serial.println("Finished sending");
  
  // wait for ack
  delay(10);
  while(!Mirf.dataReady()){
    Serial.println("Waiting");
    if ( ( millis() - time ) > 1000 ) {
      Serial.println("Timeout on response from server!");
      return;
    }
  }  

  // get ack and print
  Mirf.getData((byte *) &voltsA);
  Serial.println(voltsA);
  
  delay(10000);
}

