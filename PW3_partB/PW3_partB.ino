//(1)-Include Library
#include <WiFiClientSecure.h>
#include <ESP32_WiFiManager.h>  //https://github.com/kingdiaw/ESP32_WiFiManager

//(2)-Define Constant Value
const char* host = "api.thingspeak.com";
const int port = 443;
String apikey = "HE2QBIOR16QPLQQR";
const float adcToLux = 0.29625;           //Constant Convert ADC to LUX value

//(3)-Object Mapping
WiFiClientSecure client;
ESP32_WiFiManager wm("ESP32Wifi-AP", 2, 15); //WiFi AP = "ESP32Wifi-AP", LED at Pin 2, Trigger at Pin 15

//(4)-I/O Mapping
const byte ldrPin35 = 35;
const byte sw1Pin34 = 34;

//(5)-Global Variable Declaration
float temperature = 0;
int ldrValue = 0;

//Internal data from ESP32
extern "C" {
  uint8_t temprature_sens_read();
}

//(6) User Define Function
//6.2 Function - update_suhu()
void update_field1(float dd) {
  Serial.println("\nStarting connection to server...");
  client.setInsecure();
  if (!client.connect(host, port))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    //Prepare HTTP header
    String header = "";
    header = "GET /update?api_key=" + apikey + "&field1=" + (String(dd)) + " HTTP/1.1\n";
    header += "Host: api.thingspeak.com\n";
    header += "Connection: close\n\n";
    Serial.print(header);
    // Make a HTTP request:
    client.print(header);

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
    client.stop();
  }
}

void setup() {
  //Initialize serial and wait for port to open:
  pinMode(sw1Pin34, INPUT);
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  wm.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  wm.running();
  
  //get internal temp of ESP32
  uint8_t temp_farenheit = temprature_sens_read();
  //convert farenheit to celcius
  temperature = ( temp_farenheit - 32 ) / 1.8;

  //get LDR value
  ldrValue = analogRead(ldrPin35);
  //convert ADC to Lux
  float luxValue = ldrValue * adcToLux;

  //Read state button sw1
  byte button1State = digitalRead(sw1Pin34);

  if (button1State == HIGH) {
    delay(500);
    Serial.print("Temperature:");
    Serial.println(temperature);
    update_field1(temperature);
    Serial.println();
    Serial.println("Wait 15sec for next time");
  }
}
