#include <Arduino.h>
#define LCD_BL 14
void setup() {
  pinMode(LCD_BL, OUTPUT);
  for (int i = 0; i < 10; i++) {
    digitalWrite(LCD_BL, HIGH); delay(100);
    digitalWrite(LCD_BL, LOW); delay(100);
  }
  digitalWrite(LCD_BL, HIGH);
  Serial.begin(115200);
  Serial.println("PIO OK!");
}
void loop() {
  Serial.println("A");
  digitalWrite(LCD_BL, !digitalRead(LCD_BL));
  delay(1000);
}
