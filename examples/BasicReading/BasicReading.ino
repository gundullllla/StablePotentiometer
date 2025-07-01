#include <StablePotentiometer.h>

#define POT_PIN 15
Potentiometer pot(POT_PIN);

void setup() {
  Serial.begin(115200);
  pot.configureADC();
  Serial.println("Basic Potentiometer Reading");
}

void loop() {
  Serial.print("Raw: ");
  Serial.print(pot.raw());
  Serial.print(" | Filtered: ");
  Serial.println(pot.read());
  delay(200);
}