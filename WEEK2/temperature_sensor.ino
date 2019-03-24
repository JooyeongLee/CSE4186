// include ThingSpeak and ESP8266 WiFi Libraries
#include "ThingSpeak.h"

#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>

#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#include <OneWire.h>
#include <DallasTemperature.h>



OneWire ds(D3);

// set Router user name and passward
char ssid[] = "H204";
char pass[] = "12l98j63y";

int status = WL_IDLE_STATUS;
DallasTemperature sensors(&ds);
WiFiClient  client;
// MySQL
IPAddress server_addr(52,195,5,247);   // MySQL SERVER
char user[] = "me"; // MySQL USERNAME
char password[] = "password"; // MySQL PASSWORD
char INSERT_DATA[] = "INSERT INTO sensors.room (value, created) VALUES (%s, NOW() + INTERVAL 1 HOUR)";
char query[128];
char temperature[10];

MySQL_Connection conn((Client *)&client);




int sensorValue; // variable to save analog reading

//channel number and API key of ThingSpeak websie to transmit web
unsigned long myChannelNumber = 727943;
const char * myWriteAPIKey = "5EFJ86HKBSOOGAK5";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass); 
  ThingSpeak.begin(client);
  sensors.begin();
  
 
  Serial.println("DB - Connecting...");
  //Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
  conn.close();
}

int set=0;
void saveTempData() {
            Serial.print("while mysql. . .");
            sensors.requestTemperatures();
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
            Serial.print("Temp:");
            Serial.println(sensors.getTempCByIndex(0));
            Serial.print("Query\n");
            set=0;
            /*
            dtostrf(sensors.getTempCByIndex(0), 2, 2, temperature);
            sprintf(query, INSERT_DATA, temperature);
            cur_mem->execute(query);
            Serial.println(query);
            delete cur_mem;
            Serial.println("Data stored!");
            delay(1000*60);
            */
 }

void _ThingSpeak(){
  byte i, present=0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  Serial.print("while ThingSpeak . . .\n");
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
      set=1;
      }

void loop() {
  
  while(1){
    if(set==0) _ThingSpeak();
    else saveTempData();
  }
}
