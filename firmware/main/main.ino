#include "MAX30105.h"
#include "heartRate.h"
#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>


MAX30105 sensor;
WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);

unsigned long lastBeatTime = 0;
float bpm = 0;
float normalizedBpm = 0;
unsigned long fingerNotDetectedTime = 0;
unsigned long pulseNotDetectedTime = 0;
bool hasPreviousBeat = false;

const char* MQTT_ID = "MQTT_ID";
const char* MQTT_BROKER = "192.168.2.154"; 
const int MQTT_BROKER_PORT = 1883;
const char* MQTT_TOPIC =  "sensor";
const char* DEVICE_ID = "01a160c0-7517-745b-8241-e7f8d680449f";

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
    Serial.println("Trying to connecting to network");
    delay(500);
  }
  Serial.print("Connected to network: ");
  Serial.println(WiFi.localIP());

   MQTT.setServer(MQTT_BROKER, MQTT_BROKER_PORT); 

  while (!MQTT.connected()){
    if(MQTT.connect(MQTT_ID)){
      MQTT.subscribe(MQTT_TOPIC);
      Serial.print("Connected to MQTT broker: ");
      Serial.println(MQTT_BROKER);
    }
  }
  configTime(0, 0, "pool.ntp.org", "a.st1.ntp.br");
}

void loop() {
  long value = sensor.getIR();
    //TODO: creting consts for values like 50000, 30.0, 200.0
    if(value > 50000){
      if(checkForBeat(value)){
        if(hasPreviousBeat){
          long delta = millis() - lastBeatTime;
          bpm = 60 / (delta / 1000.0);
          lastBeatTime = millis();
          if(bpm >= 25 && bpm <= 250){
            if(normalizedBpm == 0){
              normalizedBpm = bpm;
            }else{
              normalizedBpm = (normalizedBpm * 0.70) + (bpm * 0.30);
            }
            publishMessage("MEASURING", normalizedBpm);
            Serial.print("BPM = ");
            Serial.println(normalizedBpm);
          }
        }else{
          lastBeatTime = millis();
          hasPreviousBeat = true;
          publishMessage("CALIBRATING", 0);
          Serial.println("Calibrating...");
        }
        pulseNotDetectedTime = millis();
      }else {
        if(millis() - pulseNotDetectedTime > 5000){
          pulseNotDetectedTime = millis();
          hasPreviousBeat = false;
          normalizedBpm = 0;
          publishMessage("WEAK_SIGNAL", 0);
          Serial.println("Finger not detected correctly! Position your finger again!");
        }
      }
    }else{
      lastBeatTime = millis();
      pulseNotDetectedTime = millis();
      hasPreviousBeat = false;
      normalizedBpm = 0;
      if(millis() - fingerNotDetectedTime > 5000){
        publishMessage("IDLE", 0);
        Serial.println("Finger not detected! Position your finger!");
        fingerNotDetectedTime = millis();
      }
    }
  MQTT.loop();
}

void publishMessage(const char* state, const float bpm){
  char payload[256];
  snprintf(payload, sizeof(payload), "{\"device_id\":\"%s\",\"state\":\"%s\",\"bpm\":%.1f,\"time\":%lld}", DEVICE_ID, state, bpm, (long long)time(nullptr));
  MQTT.publish(MQTT_TOPIC, payload);
}