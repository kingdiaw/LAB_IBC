#include <WiFi.h>
#include "DHTesp.h"       //DHTesp by beegee_tokyo Version 1.17.0
#include <ArduinoJson.h>  //ArduinoJson by Benoit Blanchon Version 6.13.0
#include <MQTT.h>         //MQTT by Joel Gaehwiler Version 2.4.7

// Replace the next variables with your SSID/Password combination
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxx";
WiFiClient client;

//MQTT setting
const char* broker ="xxx.xxx.xxx.xxx";  //Server PTSS
const char* unique_id = "espXX";
String topic_subscribe = "espXX/room1/led1"; 
String topic_publish = "espXX/humidity";     
MQTTClient mqtt(1024);

//DHT Pin
const byte dhtPin = 25;
DHTesp dht;

// LED Pin
const byte ledPin = 2;
const byte led1 = 22;

//Global Variable
unsigned long ledTick=0;
unsigned long sysTick=0;
float temperature=0.0,humidity=0.0;
const unsigned int TIME_INTERVAL = 15000; //15 Sec

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

//User Function - setup_mqtt()
//------------------------------------------------
void setup_mqtt(){
  while(!mqtt.connect(unique_id)){
    Serial.print(".");
    delay(500);
  }
  Serial.println("MQTT connected.");
  mqtt.subscribe(topic_subscribe);
}
//------------------------------------------------

//CALL THIS FUNCTION WHEN RECEIVED MESSAGE FROM MQTT
//------------------------------------------------------------- 
void messageReveived(String &topic_subscribe, String &payload){
  Serial.print("Incoming Topic:");
  Serial.println(topic_subscribe);
  Serial.print(", Payload");
  Serial.println(payload);

//Decode DATA from JSON format
//e.g.: {"value":"on"}
  DynamicJsonDocument doc(1024);
  deserializeJson (doc,payload);
  String value = doc["value"];

  Serial.println(value);

//Application according to JSON Data  
  if(value == "on"){
   digitalWrite(led1,HIGH); 
   Serial.println("LED on");
  }
  else if(value == "off"){
    digitalWrite(led1,LOW);  
    Serial.println("LED off");
  }
}
//------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  dht.setup(dhtPin,DHTesp::DHT22);
  pinMode(ledPin, OUTPUT);
  pinMode(led1,OUTPUT);
  mqtt.begin(broker,1883,client);
  mqtt.onMessage(messageReveived);
  setup_wifi();
  setup_mqtt();
}

void loop() {
  //MQTT Running
  mqtt.loop();
  if(!mqtt.connected()){
    setup_mqtt();
  }

  //Publish Payload to MQTT every 15 Sec
  if(millis()>sysTick){
    sysTick = millis()+TIME_INTERVAL;
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(temperature, 1);
    Serial.print("\t\t");
    Serial.print(humidity, 1);
    Serial.println();

    StaticJsonDocument<200>data;
    data["temperature"]=temperature;
    data["humidity"]=humidity;
    
    //====Debuging Message=====
    serializeJson(data,Serial);
    Serial.println();
    serializeJsonPretty(data,Serial);
    Serial.println();
    //=========================
    
    String payload;
    serializeJson(data,payload);    

    mqtt.publish(topic_publish,payload);                        
  }

  //Blinking LED onboard
  if(millis()>ledTick){
    ledTick = millis()+300;
    digitalWrite(ledPin,digitalRead(ledPin)^1);
  }
}
