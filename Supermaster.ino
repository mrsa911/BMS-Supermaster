#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

#include <Arduino.h>
#include "FSVUB_CAN.h"

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

//Timing variables
int t_start;
int t_stop;

double ExternalTemps[16];
double tempMaxBatt = 0;
double tempMaxExt = 0;
double battState = 0;

void setup() {
  Serial.begin(115200);
  sensors.begin();
  sensors.setResolution(11);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(10, OUTPUT); // set the SS pin as an output
  SPI.begin();         // initialize the SPI library

  t_start = millis();
}

void loop() {
  
  GetExternaltemps();
  //UARTMasterIn();
  SendCan();
  
}

void GetExternaltemps(){
  sensors.requestTemperatures();
  
  for(int i = 0; i<sizeof(ExternalTemps);i++){
    ExternalTemps[i] = sensors.getTempCByIndex(i);
  }

  for (int i = 0; i < (sizeof(ExternalTemps) / sizeof(ExternalTemps[0])); i++) {
      if (ExternalTemps[i] > tempMaxExt) {
         tempMaxExt = ExternalTemps[i];
      }
  }
}
//void UARTMasterIn(){
//  char tempstor = Serial.read(); //len aanpassen naar wens
//  double fromMaster[4];
//  for(int i = 0; i<sizeof(fromMaster);i++){
//    fromMaster[i] = tempstor[i]- '0';
//  }
//  tempMaxBatt = fromMaster[0];
//}

void SendCan(){
  t_stop = millis();
  //double ToSend[] = {tempMaxBatt,tempMaxExt,battState}
  double ToSend= 8;
  int dif = t_stop - t_start;
  if(dif > 1000) // every second data is sent on the CAN-bus or on the Serial interface
  {
    digitalWrite(10, LOW);            // set the SS pin to LOW
  
    for(byte wiper_value = 0; wiper_value <= 128; wiper_value++) {

      SPI.transfer(0x00);             // send a write command to the MCP4131 to write at registry address 0x00
      SPI.transfer(ToSend);      // send a new wiper value
    }

    digitalWrite(10, HIGH);           // set the SS pin HIGH
    
    
    t_start = millis();
  }
}
