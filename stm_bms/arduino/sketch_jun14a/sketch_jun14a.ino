#include <SoftwareSerial.h>

#define ledPinA 4
#define ledPinB 5
#define ledPinC 6
#define ledPinD 7
#define ledPinE 8
#define ledPinF 9
#define ledPinG 10

#define dpin 11
#define upin 12

#define ADC_PIN A0

SoftwareSerial uart(2, 3); // RX, TX

const byte segments[7] = { ledPinA, ledPinB, ledPinC, ledPinD, ledPinE, ledPinF, ledPinG };

const byte chiffres[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

int adcValue = 0, vitesse = 0, derniereVitesse = -1;

void setup()
{
  for (int i = 0; i < 7; i++) {
    pinMode(segments[i], OUTPUT);
  }

  pinMode(dpin, OUTPUT);
  pinMode(upin, OUTPUT);
  pinMode(ADC_PIN, INPUT);

  Serial.begin(9600);
  uart.begin(9600);

  Serial.println("Demarrage...");
}

void loop()
{
  adcValue = analogRead(ADC_PIN);
  vitesse = map(adcValue, 0, 1023, 0, 50); 

  afficher_nombre(vitesse);

  if (vitesse != derniereVitesse) {
    derniereVitesse = vitesse;
    uart.write((uint8_t)vitesse); 

    Serial.print("vitesse = ");
    Serial.println(vitesse);
  }

  while (uart.available()) {
    char c = uart.read();
    Serial.print(c);
  }
}

void afficher(int chiffre) {
  if (chiffre < 0 || chiffre > 9) return;
  for (int i = 0; i < 7; i++) {
    digitalWrite(segments[i], chiffres[chiffre][i]);
  }
}

void afficher_nombre(unsigned int n) {
  int d = n / 10;
  int u = n % 10;

  for (int i = 0; i < 2; i++) {
    afficher(d);
    digitalWrite(dpin, LOW);
    delay(2);
    digitalWrite(dpin, HIGH);

    afficher(u);
    digitalWrite(upin, LOW);
    delay(2);
    digitalWrite(upin, HIGH);
  }
}