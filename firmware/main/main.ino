#include "MAX30105.h"
#include "heartRate.h"


MAX30105 sensor;

unsigned long lastBeatTime = 0;
float bpm = 0;
float normalizedBpm = 0;
long fingerNotDetectedTime = 0;
bool hasPreviousBeat = false;

void setup() {
  Serial.begin(115200);
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
  //TODO: send a mqtt message about this state
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
