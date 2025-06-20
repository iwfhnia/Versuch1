// Bibliotheken einbinden
#include <LiquidCrystal.h>
#include <DHT.h>

// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

// DHT-Sensor
#define DHTPIN      A0
#define DHTTYPE     DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pins definieren
const int lichtPin    = A1;
const int tasterPin   = 12;
const int piezoPin    = 13;
const int ledGruen    = A3;
const int ledGelb     = A4;
const int ledRot      = A5;

// Zeitsteuerung in Minuten
unsigned long startZeitMin;
const unsigned long transportDauerMin = 360;   // 6 Stunden = 360 Minuten
const unsigned long messIntervallMin  = 10;    // 10 Minuten
unsigned long letzteMessungMin        = 0;
int messungen                        = 0;

// Zuletzt gemessene Werte
float temp       = 0.0;
float hum        = 0.0;
int lichtWert    = 0;

void setup() {

  // LCD initialisieren
  lcd.begin(16, 2);
  lcd.print("System Start...");
  delay(2000);
  lcd.clear();

  // Sensor initialisieren
  dht.begin();

  // Pin-Modi setzen
  pinMode(tasterPin, INPUT_PULLUP);
  pinMode(piezoPin, OUTPUT);
  pinMode(ledGruen, OUTPUT);
  pinMode(ledGelb, OUTPUT);
  pinMode(ledRot, OUTPUT);

  // Zeitbasis setzen
  startZeitMin      = millis() / 60000;
  letzteMessungMin  = startZeitMin - messIntervallMin;  // erste Messung sofort
}

void loop() {
  unsigned long jetztMin    = millis() / 60000;             // Minuten seit Start
  unsigned long vergangenMin = jetztMin - startZeitMin;     // Minuten seit Beginn
  long restMinuten           = transportDauerMin - vergangenMin;

  // Alle 10 Minuten eine neue Messung durchführen (36x im Zeitraum)
  if ((vergangenMin - letzteMessungMin >= messIntervallMin) && (messungen < 36)) {
    letzteMessungMin = jetztMin;
    messungen++;

    // Sensorwerte einlesen
    temp      = dht.readTemperature();
    hum       = dht.readHumidity();
    lichtWert = analogRead(lichtPin);
  }

  // LCD-Ausgabe
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(temp,1) + "oC "); lcd.print("H:" + String(hum,0) + "%");

  lcd.setCursor(0, 1);
  lcd.print("L:" + String(lichtWert) + "lx");lcd.print(" Z:" + (restMinuten > 0 ? String(restMinuten) + "min" : "Ende "));

  // LED-Statuslogik
  bool kritisch  = false;
  bool grenzwert = false;

  if      (temp > 17.0)       kritisch = true;
  else if (temp > 10.0)       grenzwert = true;

  if      (hum  > 40.0)       kritisch = true;
  else if (hum  > 30.0)       grenzwert = true;

  if      (lichtWert > 100)   kritisch = true;
  else if (lichtWert > 70)   grenzwert = true;

  digitalWrite(ledGruen, (!kritisch && !grenzwert));
  digitalWrite(ledGelb,  (grenzwert && !kritisch));
  digitalWrite(ledRot,   kritisch);

  // Türüberwachung über Taster
  bool tasterGedrueckt = (digitalRead(tasterPin) == LOW);
  if (!tasterGedrueckt) {
    tone(piezoPin, 1000, 100);  // Alarmsignal bei offenem Behälter
  } else {
    noTone(piezoPin);
  }

  delay(1000); 
}
