#define LCD_BL 14

void setup() {
  pinMode(LCD_BL, OUTPUT);
  Serial.begin(115200);
  delay(500);
  Serial.println("BOOT OK");

  // Blink backlight 5 times as visual confirmation
  for (int i = 0; i < 5; i++) {
    digitalWrite(LCD_BL, HIGH);
    delay(100);
    digitalWrite(LCD_BL, LOW);
    delay(100);
  }
  digitalWrite(LCD_BL, HIGH);
  Serial.println("BLINK DONE");
}

void loop() {
  Serial.println("ALIVE");
  digitalWrite(LCD_BL, !digitalRead(LCD_BL));
  delay(1000);
}
