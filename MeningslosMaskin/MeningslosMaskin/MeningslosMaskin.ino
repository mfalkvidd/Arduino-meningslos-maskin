// Inspirerad av "Moody useless machine" av Arvid Mortensen, http://www.lamja.com/?p=451
#include "Konfiguration.h"
#include <avr/sleep.h>
boolean aktiv = false;
byte variant = 0;

#define MYCKET_SNABB 1
#define SNABB 500
#define MEDEL 1000
#define LANGSAM 3000
#define MYCKET_LANGSAM 6000

#define LUCKA_STANGD 0 // Position när luckan är stängd
#define LUCKA_PA_GLANT 45 // Position när luckan är på glänt
#define LUCKA_1_3_OPPEN 60 // Position när luckan är ca 1/3 öppen
#define LUCKA_2_3_OPPEN 75 // Position när luckan är ca 2/3 öppen
#define LUCKA_HELT_OPPEN 100 // Position när luckan är helt öppen

#define ARM_NERE 150 // Armen är nere i lådan
#define ARM_SYNLIG 100 // Armen sticker precis upp
#define ARM_NASTAN_FRAMME 60 // Armen är nästan framme vid spaken
#define ARM_FRAMME 38 // Armen har tryckt på spaken

const byte SERVO_TID[] = { 1500 / 180, 2000 / 180 }; // Tid det tar för servot att svepa 1 grad

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(A7));
  pinMode(MOSFET_PIN, OUTPUT);
  pinMode(SPAK_PIN, INPUT);
  servos[ARM].attach(SERVO_PINS[ARM]);
  servos[LUCKA].attach(SERVO_PINS[LUCKA]);
  digitalWrite(SPAK_PIN, HIGH); // Aktivera pull-up
  servos[LUCKA].write(LUCKA_STANGD);
  servos[ARM].write(ARM_NERE);
  digitalWrite(MOSFET_PIN, HIGH);
  delay(2000); // Ge servona tid att ställa in sig

  digitalWrite(MOSFET_PIN, LOW); // Stäng av servona för att spara ström
  attachInterrupt(digitalPinToInterrupt(SPAK_PIN), hanteraInterrupt, CHANGE);
}


void loop()
{
  if (aktiv)
  {
    digitalWrite(MOSFET_PIN, HIGH);
    variant = int(random(0, 10));
    Serial.println(variant);
    delay(500);
    sekvens(variant);
  }

  aktiv = digitalRead(SPAK_PIN);
  Serial.println("Somnar");
  delay(500);
  if (!aktiv) {
    digitalWrite(MOSFET_PIN, LOW); // Stäng av servona för att spara batteri
    sov();
    Serial.println("Vaknar");
    delay(200);
    aktiv = digitalRead(SPAK_PIN); // När processorn vaknar läser vi status på knappen
  }
}

void svep(byte servo, byte stopp_pos, int milli_sek)
{
  byte start_pos = servos[servo].read();
  Serial.print(servo);
  Serial.print(" ");
  Serial.print(start_pos);
  Serial.print(" ");
  Serial.print(stopp_pos);
  Serial.print(" ");
  byte distans = max(start_pos, stopp_pos) - min(start_pos, stopp_pos);
  long sovtid = max(milli_sek / distans, SERVO_TID[servo]); // Försök aldrig gå snabbare än vad servot klarar av
  Serial.println(sovtid * distans);
  short riktning = start_pos <= stopp_pos ? 1 : -1;
  for (byte pos = start_pos; pos != stopp_pos; pos += riktning)
  {
    servos[servo].write(pos);
    delay(sovtid);
  }
  Serial.print("Stannar vid ");
  Serial.println(servos[servo].read());
}

void hanteraInterrupt()
{
}

void sekvens(byte variant) {
  switch (variant) {
    case 0:
      svep(LUCKA, LUCKA_2_3_OPPEN, MEDEL);
      delay(1000);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(1000);
      svep(LUCKA, LUCKA_HELT_OPPEN, SNABB);
      svep(ARM, ARM_SYNLIG, SNABB);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(200);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      break;
    case 1:
      delay(800);
      svep(LUCKA, LUCKA_PA_GLANT, MEDEL);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(120);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(120);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(120);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(120);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(120);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(120);
      svep(LUCKA, LUCKA_STANGD, MEDEL);
      svep(LUCKA, LUCKA_2_3_OPPEN, MEDEL);
      delay(1000);
      svep(LUCKA, LUCKA_HELT_OPPEN, SNABB);
      svep(ARM, ARM_SYNLIG, SNABB);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(100);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      break;
    case 2:
      svep(LUCKA, LUCKA_HELT_OPPEN, MYCKET_SNABB);
      delay(100);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(1000);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      delay(500);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(500);
      break;
    case 3:
      delay(500);
      svep(LUCKA, LUCKA_HELT_OPPEN, MYCKET_SNABB);
      delay(1);
      svep(ARM, ARM_NASTAN_FRAMME, MYCKET_SNABB);
      delay(450);
      svep(ARM, ARM_FRAMME, MYCKET_LANGSAM);
      delay(500);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      delay(200);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(400);
      break;
    case 4:
      delay(1000);
      svep(LUCKA, LUCKA_HELT_OPPEN, MYCKET_SNABB);
      delay(1);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(450);
      svep(ARM, ARM_NASTAN_FRAMME, MYCKET_SNABB);
      delay(110);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(110);
      svep(ARM, ARM_NASTAN_FRAMME, MYCKET_SNABB);
      delay(110);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(110);
      svep(ARM, ARM_NASTAN_FRAMME, MYCKET_SNABB);
      delay(110);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(110);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      delay(200);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(400);
      break;
    case 5:
      delay(1500);
      svep(LUCKA, LUCKA_HELT_OPPEN, MYCKET_SNABB);
      delay(1);
      svep(ARM, ARM_NASTAN_FRAMME, MYCKET_SNABB);
      delay(450);
      svep(LUCKA, LUCKA_2_3_OPPEN, SNABB);
      delay(2000);
      svep(LUCKA, LUCKA_HELT_OPPEN, SNABB);
      delay(2000);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(100);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      delay(200);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(400);
      break;
    case 6:
      svep(LUCKA, LUCKA_2_3_OPPEN, SNABB);
      delay(200);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_2_3_OPPEN, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_2_3_OPPEN, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_STANGD, SNABB);
      delay(200);
      svep(LUCKA, LUCKA_HELT_OPPEN, SNABB);
      delay(1);
      svep(ARM, ARM_FRAMME, SNABB);
      delay(450);
      svep(ARM, ARM_NERE, SNABB);
      delay(200);
      svep(LUCKA, LUCKA_STANGD, SNABB);
      delay(400);
      break;
    case 7:
      delay(200);
      svep(LUCKA, LUCKA_2_3_OPPEN, SNABB);
      delay(200);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_2_3_OPPEN, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(100);
      svep(LUCKA, LUCKA_1_3_OPPEN, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(50);
      svep(LUCKA, LUCKA_STANGD, SNABB);
      delay(200);
      svep(LUCKA, LUCKA_HELT_OPPEN, SNABB);
      delay(1);
      svep(ARM, ARM_FRAMME, SNABB);
      delay(450);
      svep(ARM, ARM_NERE, SNABB);
      delay(200);
      svep(LUCKA, LUCKA_STANGD, SNABB);
      delay(400);
      break;
    case 8:
      delay(1000);
      svep(LUCKA, LUCKA_2_3_OPPEN, MEDEL);
      delay(500);
      svep(LUCKA, LUCKA_PA_GLANT, SNABB);
      delay(1);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_1_3_OPPEN, MYCKET_SNABB);
      delay(50);
      svep(LUCKA, LUCKA_PA_GLANT, MYCKET_SNABB);
      delay(500);
      svep(LUCKA, LUCKA_2_3_OPPEN, LANGSAM);
      delay(1);
      svep(LUCKA, LUCKA_HELT_OPPEN, SNABB);
      delay(1);
      svep(ARM, ARM_FRAMME, MYCKET_SNABB);
      delay(750);
      svep(ARM, ARM_NERE, MYCKET_SNABB);
      delay(750);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(400);
      break;
    case 9:
      delay(800);
      svep(LUCKA, LUCKA_HELT_OPPEN, MYCKET_LANGSAM);
      delay(1);
      svep(ARM, ARM_FRAMME, MEDEL);
      delay(1);
      svep(ARM, ARM_NERE, MEDEL);
      delay(1);
      svep(LUCKA, LUCKA_2_3_OPPEN, MYCKET_LANGSAM);
      delay(1);
      svep(LUCKA, LUCKA_STANGD, MYCKET_SNABB);
      delay(300);
  }
}

void sov() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  PRR = 255;
  MCUCR |= (1 << BODS) | (1 << BODSE);
  MCUCR &= ~(1 << BODSE);
  EIMSK = 1;
  sleep_mode();
  // ZZZzzz...
  sleep_disable(); // Vaken igen
  PRR = 0;
}

