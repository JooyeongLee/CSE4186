
// include ThingSpeak and ESP8266 WiFi Libraries
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <OneWire.h>
OneWire ds(D3);

// set Router user name and passward
char ssid[] = "Jooyeong Lee의 iPhone";
char pass[] = "12986300";


int status = WL_IDLE_STATUS;
WiFiClient  client;

int sensorValue; // variable to save analog reading

//channel number and API key of ThingSpeak websie to transmit web
unsigned long myChannelNumber = 727943;
const char * myWriteAPIKey = "5EFJ86HKBSOOGAK5";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass); 
  ThingSpeak.begin(client);
}

void loop() {
  byte i, present=0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if(!ds.search(addr)){
    ds.reset_search();
    delay(250*60);
    return;
    }
    if(OneWire::crc8(addr, 7)!=addr[7]){
      Serial.println("CRC is not valid!");
      return ;
      }
      if(addr[0] !=0x28){
        Serial.println("Not DS18B20! ");
        return ;}
      ds.reset();
      ds.select(addr);
      ds.write(0x44,1);
      delay(750*60);

      present=ds.reset();
      ds.select(addr);
      ds.write(0xBE);
      for(i=0; i<9; i++) data[i]=ds.read();
      

      int16_t raw=(data[1]<<8) | data[0];
      celsius=(float)raw/16.0;
      Serial.print("  Temperature = ");
      Serial.println(celsius);
      ThingSpeak.writeField(myChannelNumber, 1, celsius, myWriteAPIKey);
  
  
}
