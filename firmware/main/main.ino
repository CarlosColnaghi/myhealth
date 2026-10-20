#include "MAX30105.h"
#include "heartRate.h"
#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>


MAX30105 sensor;
WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);

unsigned long lastBeatTime = 0;
float bpm = 0;
float normalizedBpm = 0;
long fingerNotDetectedTime = 0;
bool hasPreviousBeat = false;

const char* MQTT_ID = "MQTT_ID";
const char* MQTT_BROKER = "192.168.2.154"; 
const int MQTT_BROKER_PORT = 1883;
const char* MQTT_TOPIC =  "sensor";

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(8, 9);
  if(!sensor.begin(Wire, I2C_SPEED_FAST)){
    //TODO: send a mqtt message about this state
    Serial.println("Sensor was not found!");
    delay(1000);
    while(true);
  }
  sensor.setup();
  sensor.setPulseAmplitudeRed(0x24);
  sensor.setPulseAmplitudeGreen(0);
  
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Trying to connecting to WiFi");
    delay(500);
  }
  Serial.print("Connected to: ");
  Serial.println(WiFi.localIP());

   MQTT.setServer(MQTT_BROKER, MQTT_BROKER_PORT); 

  while (!MQTT.connected()){
    if(MQTT.connect(MQTT_ID)){
      MQTT.subscribe(MQTT_TOPIC);
      Serial.print("Connected to MQTT broker: ");
      Serial.println(MQTT_BROKER);
    }
  }
}

void loop() {
  long value = sensor.getIR();
    //TODO: creting consts for values like 50000, 30.0, 200.0
    if(value > 50000){
      //TODO: sends a warning when finger is detected, but pulse not detected
      if(checkForBeat(value)){
        if(hasPreviousBeat){
          long delta = millis() - lastBeatTime;
          bpm = 60 / (delta / 1000.0);
          lastBeatTime = millis();
          if(bpm >= 30.0 && bpm <= 200.0){
            if(normalizedBpm == 0){
              normalizedBpm = bpm;
            }else{
              normalizedBpm = (normalizedBpm * 0.70) + (bpm * 0.30);
            }
            //TODO: send a mqtt message about bpm value and time
            Serial.print("bpm = ");
            Serial.println(normalizedBpm);
          }
        }else{
          lastBeatTime = millis();
          hasPreviousBeat = true;
        }
      }
    }else{
      lastBeatTime = millis();
      hasPreviousBeat = false;
      normalizedBpm = 0;
      if(fingerNotDetectedTime == 0 || (millis() - fingerNotDetectedTime > 5000)){
        //TODO: send a mqtt message about this state
        Serial.println("Finger not detected! Position you finger!");
        fingerNotDetectedTime = millis();
      }
    }
  
}
