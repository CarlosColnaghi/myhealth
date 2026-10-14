#include "MAX30105.h"
#include "heartRate.h"


MAX30105 sensor;

long lastBeat = 0;
float bpm = 0;

void setup() {
  if(sensor.begin(Wire, I2C_SPEED_FAST)){
    //TODO: send a mqtt message about this state
    Serial.println("Sensor was not found!");
    while(true);
  }
  sensor.setup();
  sensor.setPulseAmplitudeRed(0x0A);
  sensor.setPulseAmplitudeGreen(0);
  //TODO: send a mqtt message about this state
}

void loop() {
  long value = sensor.getIR();
  if(checkForBeat(value)){
    long delta = millis() - lastBeat;
    lastBeat = millis();
    bpm = 60 / (delta / 1000.0);
    Serial.println("BPM = ", bpm);
  }

}
