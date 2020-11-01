//#include <ESP32HTTPUpdateServer.h>
#include <EspMQTTClient.h>

#include "DHTesp.h"


void onConnectionEstablishedClient2();
unsigned long lastpublishhome = 0;
unsigned long lastpublishadafruit = 0;
int publishfreq = 60000; // 30 second delay

// The client #1 will handle wifi connection (connecting, retrying, etc) and MQTT connection to 192.168.1.100
EspMQTTClient client1(
  "BTHub6-C7HZ_EXT",
  "g9eTHRniGraM",
  "192.168.1.48"
);

// The client #2 will handle MQTT connection to 192.168.1.101.
EspMQTTClient client2(
  "io.adafruit.com",
  1883,
  "andysm", 
  "01354beb55f94795ae599d0a7094c114"
);

DHTesp dht;

void setup()
{
  
Serial.begin(115200);

client1.enableDebuggingMessages();
client2.enableDebuggingMessages();

//Initialise the DHT sensor
  dht.setup(4, DHTesp::DHT11); // Connect DHT sensor to GPIO 17

//while(!client1.isWifiConnected()){ // Return true if WiFi is connected.
//    Serial.println("Wifi connection failed"); 
//    Serial.print("please wait.");
//    Serial.print("."); 
//    delay(5000);   
//    }
//    Serial.println("Wifi connected"); 

//while(!client1.isMqttConnected()){ // Return true if WiFi is connected.
//    Serial.println("MQTT connection failed"); 
//    Serial.print("please wait.");
//    Serial.print("."); 
//    delay(5000);   
//    }
//    Serial.println("MQTT connected"); 



// We redirect the connection established callback of client2 to onConnectionEstablishedClient2.
// This will prevent the two client from calling the same callback (default to onConnectionEstablished)

 client2.setOnConnectionEstablishedCallback(onConnectionEstablishedClient2); 

lastpublishhome = millis();
lastpublishadafruit = millis();


}

// For client1
void onConnectionEstablished()
{
  client1.subscribe("mytopic/test", [](const String & payload) {
    Serial.println(payload);
  });
  client1.publish("events", "Connection established to client 1");
}

// For client2
void onConnectionEstablishedClient2()
{
  client2.subscribe("mytopic/test", [](const String & payload) {
    Serial.println(payload);
  });
  client1.publish("events", "Connection established to client 2");
}

char temp_char[8]; // Buffer big enough for 7-character float
char humidity_char[8]; // Buffer big enough for 7-character float


void loop()
{  
  client1.loop();
  client2.loop();

  if (client1.isMqttConnected()){ // Return true if MQTT is connected.

      if (millis() >=  lastpublishhome + publishfreq){
        lastpublishhome += publishfreq;
        publishdatahome();
      }
  }

  if (client2.isMqttConnected()){ // Return true if MQTT is connected.

      if (millis() >=  lastpublishadafruit + publishfreq){
        lastpublishadafruit += publishfreq;
        publishdataadafruit();
      }
  }

}
void publishdatahome(){

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  dtostrf(humidity, 6, 0, humidity_char); // Leave room for too large numbers!
  dtostrf(temperature, 6, 0, temp_char); // Leave room for too large numbers!
  
//  Serial.print("temperature/workshop - ");
//  Serial.println(temp_char);
  client1.publish("temperature/workshop", temp_char);
  
//  Serial.print("humidity/workshop - ");
//  Serial.println(humidity_char);
  client1.publish("humidity/workshop", humidity_char);
  
}

void publishdataadafruit(){

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  dtostrf(humidity, 6, 0, humidity_char); // Leave room for too large numbers!
  dtostrf(temperature, 6, 0, temp_char); // Leave room for too large numbers!
  
//  Serial.print("temperature/workshop - ");
//  Serial.println(temp_char);
  client2.publish("/feeds/workshop-monitor.air-temperature", temp_char);
  
//  Serial.print("humidity/workshop - ");
//  Serial.println(humidity_char);
  client2.publish("/feeds/workshop-monitor.air-humidity", humidity_char);
  
}
