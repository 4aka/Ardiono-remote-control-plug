#include "IRremote.h"

// Порт реле
const int relay = 3;
const unsigned long interval = 30UL * 60UL * 1000UL; // 30 хвилин в мілісекундах
volatile bool timerStarted = false;
unsigned long startTime = 0;
volatile unsigned long value = 0xFFFFFF;

// Налаштування IR
const int irReceiverPin = 2;
IRrecv ir(irReceiverPin);
decode_results rs;

void setup() {
  delay(2000);
  Serial.begin(9600);
  ir.enableIRIn();
  attachInterrupt(digitalPinToInterrupt(irReceiverPin), decodeIR, CHANGE);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
}

void loop() {
    unsigned long currentMillis = millis();
    
    if (timerStarted && (currentMillis - startTime >= interval)) {
        digitalWrite(relay, LOW); // Вимкнути реле після 30 хвилин
        timerStarted = false; // Зупинити таймер
    }

    switch (value) {
      case 0xFFA25D: // Вмикання реле
        digitalWrite(relay, HIGH);
        timerStarted = false; // Зупинити таймер, якщо був запущений
        value = 0xFFFFFF;
        break;
      case 0xFFE21D: // Вимкнення реле
        digitalWrite(relay, LOW);
        timerStarted = false; // Зупинити таймер
        value = 0xFFFFFF;
        break;
      case 0xFF629D: // Запуск таймера на 30 хвилин
        if (!timerStarted) {
          digitalWrite(relay, HIGH);
          startTime = millis(); // Запам'ятати час запуску
          timerStarted = true; // Запустити таймер
        }
        value = 0xFFFFFF;
        break;
    }
}

void decodeIR() {
  if (ir.decode(&rs)) {
    // Вивести значення HEX команди з пульта
    Serial.println(rs.value, HEX);
    value = rs.value;
    ir.resume();
  }
}
