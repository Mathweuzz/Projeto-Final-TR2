#include <SPI.h>
int reset_lora = 9;

void setup() {
  pinMode(reset_lora, OUTPUT);

  digitalWrite(reset_lora, LOW);
  delay(100);
  digitalWrite(reset_lora, HIGH);
  Serial.begin(9600);
}

void loop() {
  Serial.println("teste");
}
