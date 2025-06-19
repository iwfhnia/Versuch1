// Bibliotheken einbinden
#include <LiquidCrystal.h>
#include <DHT.h>

// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// DHT-Sensor
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pins definieren
const int lichtPin = A0;
const int kippsensorPin = 6;
const int tasterPin = 4;
const int piezoPin = 5;
const int ledGruen = A3;
const int ledGelb  = A4;
const int ledRot   = A5;

void setup() {
  // LCD initialisieren
  lcd.begin(16, 2);
  lcd.print("System Start...");

  // Sensoren und Aktoren konfigurieren
  dht.begin();
  pinMode(kippsensorPin, INPUT);
  pinMode(tasterPin, INPUT_PULLUP);
  pinMode(piezoPin, OUTPUT);
  pinMode(ledGruen, OUTPUT);
  pinMode(ledGelb, OUTPUT);
  pinMode(ledRot, OUTPUT);
  delay(2000);
  lcd.clear();
}

void loop() {
  // Sensorwerte lesen
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  int lichtWert = analogRead(lichtPin);
  bool kippt = !digitalRead(kippsensorPin);
  bool tasterGedrueckt = digitalRead(tasterPin) == LOW;

  // LCD-Ausgabe
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temp, 1); lcd.print("C H:"); lcd.print(hum, 0); lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("L:"); lcd.print(lichtWert); lcd.print(" K:"); lcd.print(kippt ? "i.O." : "! ");

  // LED-Statuslogik
  bool kritisch = false;
  bool grenzwert = false;

  // Temperaturbewertung
  if (temp > 17) {
    kritisch = true;
  } else if (temp > 15) {
    grenzwert = true;
  }

  // Luftfeuchtigkeit
  if (hum > 40) {
    kritisch = true;
  } else if (hum > 35) {
    grenzwert = true;
  }

  // Lichtbewertung (einfaches Beispiel)
  if (lichtWert > 800) {
    kritisch = true;
  } else if (lichtWert > 600) {
    grenzwert = true;
  }

  // LEDs steuern
  digitalWrite(ledGruen, !kritisch && !grenzwert);
  digitalWrite(ledGelb, grenzwert && !kritisch);
  digitalWrite(ledRot,  kritisch);

  // Türüberwachung über Taster
  if (!tasterGedrueckt) {
    tone(piezoPin, 1000, 100); // leiser Piezo-Ton
  } else {
    noTone(piezoPin);
  }

  delay(1000);
}

