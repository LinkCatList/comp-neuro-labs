
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!ads.begin(0x49)) {
    Serial.println("ADS1115 NOT FOUND!");
    while (1) {
      delay(100);
    }
  }

  ads.setGain(GAIN_ONE);
}

void loop() {
  int16_t raw = ads.readADC_SingleEnded(0);

  float voltage = ads.computeVolts(raw);

  Serial.print(millis());
  Serial.print(",");
  Serial.print(raw);
  Serial.print(",");
  Serial.println(voltage, 4);

  delay(100);
}
