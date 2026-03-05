
#include "HX711.h"

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
HX711 scale;
void setup() {
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  if(scale.is_ready()) {
    Serial.println("HX711 is ready.");
    scale.set_scale();
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done. Place a known weight on the scale...");
  } else {
    Serial.println("HX711 not found.");
  }
}

void loop() {

  if (scale.is_ready()) {
    delay(1000);
    Serial.print("Weight:");
    long reading = scale.get_units(10) * 0.007633*(-1);

    Serial.print(reading);
    Serial.println(" gm");
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}
