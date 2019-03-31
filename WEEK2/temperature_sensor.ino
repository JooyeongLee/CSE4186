//delete thingspeak inst., using only mysql

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
           
            dtostrf(sensors.getTempCByIndex(0), 2, 2, temperature);
            sprintf(query, INSERT_DATA, temperature);
            cur_mem->execute(query);
            Serial.println(query);
            delete cur_mem;
            Serial.println("Data stored!");
            delay(1000*60);
           
 }

void loop() {
  
  saveTempData();
  }
}
