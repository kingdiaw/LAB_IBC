#include <WiFi.h>
#include <ArduinoJson.h>  //ArduinoJson by Benoit Blanchon Version 6.13.0
#include <MQTT.h>         //MQTT by Joel Gaehwiler Version 2.4.7

// Replace the next variables with your SSID/Password combination
const char* ssid = "xxxxxxxx";
const char* password = "xxxxxxxx";
WiFiClient client;

//MQTT setting
const char* broker ="xxx.xxx.xxx.xxx";  //Mosquitto
const char* unique_id = "xxxx";
String topic_subscribe = "xxxxxxxx"; 
    
MQTTClient mqtt(1024);

// LED Pin
const byte ledPin = 2;
const byte led1Pin22 = 22;

//Global Variable
unsigned long ledTick=0;

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
  Serial.print(topic_subscribe);
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
   digitalWrite(led1Pin22,HIGH); 
   Serial.println("LED on");
  }
  else if(value == "off"){
    digitalWrite(led1Pin22,LOW);  
    Serial.println("LED off");
  } 
}
//------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  //dht.setup(dhtPin,DHTesp::DHT11);
  pinMode(ledPin, OUTPUT);
  pinMode(led1Pin22,OUTPUT);
  mqtt.begin(broker,1883,client);
  mqtt.onMessage(messageReveived);
  setup_wifi();
  setup_mqtt();
}

void loop() {
  
  mqtt.loop();
  delay(10);
  if(!mqtt.connected()){
    setup_mqtt();
  }

  if(millis()>ledTick){
    ledTick = millis()+200;
    digitalWrite(ledPin,digitalRead(ledPin)^1);
  }
}
