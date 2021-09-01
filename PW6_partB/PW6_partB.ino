#include "ThingSpeak.h"   //ThingSpeak by MathWorks Version 1.5.0
#include "DHTesp.h"       //DHTesp by beegee_tokyo Version 1.17.0
#include <WiFi.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxx";

//Mapping Library
  WiFiClient client;
  DHTesp dht;

 //ThingSpeak Channel details
 unsigned long channel_ID = xxxxx;  //Replace with your Channel ID
 const char* readAPIKey = "xxxx";   //Replace with your API KEY
 const char* writeAPIKey = "xxxx";  //Replace with your API KEY
 unsigned int fieldLedState = 1;
 unsigned int fieldTemperature = 2;
 unsigned int fieldHumidity = 3;

 //Mapping GPIO<-->DHT Pin
 const byte dhtPin = 25;

 //Mapping GPIO<-->LED Pin
  const byte ledPin = 2;
  const byte led1 = 23;

//Global Variable
  unsigned long task1Tick=0;
  unsigned long task2Tick=0;
  unsigned long ledTick=0;
  float temperature=0.0,humidity=0.0;
  const unsigned int TIME_INTERVAL_TASK1 = 15000; //15 Sec 
  const unsigned int TIME_INTERVAL_TASK2 = 10000; //10 sec 

//User Function - setup_wifi()
//------------------------------------------------
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//------------------------------------------------ 

void setup() {
  Serial.begin(115200);  //Initialize serial 
  pinMode(ledPin,OUTPUT);
  pinMode(led1,OUTPUT);
  setup_wifi(); 
  dht.setup(dhtPin,DHTesp::DHT22);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  int statusCode = 0;

  //Task 1
  //=================================================================
  if(millis()>task1Tick){
    task1Tick = millis()+ TIME_INTERVAL_TASK1;
  unsigned int dataTS = ThingSpeak.readIntField(channel_ID, fieldLedState, readAPIKey);
   // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
    if(statusCode == 200){
      Serial.println("Data From MIT: " + String(dataTS));
    }
    else{
      Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    }
  //Application on ESP32 board
    if(dataTS == 0){
      Serial.println("LED1 OFF");
      digitalWrite(led1,LOW);      
    }
    else if(dataTS == 1){
      Serial.println("LED1 ON");
      digitalWrite(led1,HIGH);
    }
  }
  //END TASK 1

  //Task 2
//=================================================================
  if(millis()>task2Tick){
    task2Tick = millis()+ TIME_INTERVAL_TASK2;
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(temperature, 1);
    Serial.print("\t\t");
    Serial.print(humidity, 1);
    Serial.println(); 
    
    ThingSpeak.setField(fieldTemperature, temperature);
    ThingSpeak.setField(fieldHumidity, humidity);    

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(channel_ID, writeAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }       
  }
  //END TASK 2

  //Blinking On board LED at Pin 2
  if(millis()>ledTick){
    ledTick = millis()+300;
    digitalWrite(ledPin,digitalRead(ledPin)^1);
  } 
}
