#include "Konfiguration.h"

void setup() {
  Serial.begin(9600);
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, HIGH);
  servos[LUCKA].attach(SERVO_PINS[LUCKA]);
  servos[ARM].attach(SERVO_PINS[ARM]);
  servos[LUCKA].write(0);
  servos[ARM].write(180);
  Serial.println(F("Kalibreringsläge"));
  Serial.println(F("Styr armen uppåt med Q och neråt med A"));
  Serial.println(F("Tryck på mellanslag för att byta servo"));
}

void loop() {
  for (byte servo = 0; servo <= 1; servo++) {
    bool klar = false;
    while (!klar) {
      while (Serial.available() > 0) {
        char c = Serial.read();
        Serial.print(c);
        switch (c) {
          case 'q':
          case 'Q':
            servos[servo].write(servos[servo].read() + 1);
            break;
          case 'a':
          case 'A':
            servos[servo].write(servos[servo].read() - 1);
            break;
          case ' ':
            klar = true;
        }
        Serial.println(servos[servo].read()); // Skriv ut nuvarande position
      }
    }
  }
}

