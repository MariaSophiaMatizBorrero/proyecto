#include <LiquidCrystal.h>

// ---------- Pines ----------
LiquidCrystal lcd(13, 12, 6, 5, 4, 3);  // RS, E, D4, D5, D6, D7

const int pinSonido = A0;                // salida analógica
const int pinPico = 2;                   // salida digital (D0)
const int leds[5] = {7, 8, 9, 10, 11};

// ---------- Ajustes ----------
const int ESCALA = 15;       // más bajo = más sensible
const int RUIDO_MIN = 2;     // amplitud mínima para encender el primer LED
const int NIVEL_PICO = LOW;  // cambia a HIGH si D0 funciona al revés

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(pinPico, INPUT);
  for (int i = 0; i < 5; i++) {
    pinMode(leds[i], OUTPUT);
  }

  lcd.print("Medidor de dB");
  // Secuencia de arranque para comprobar los LEDs
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], HIGH);
    delay(200);
  }
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], LOW);
  }
  lcd.clear();
}

void loop() {
  // 1. Medir amplitud durante 50 ms
  unsigned long inicio = millis();
  int minimo = 1023;
  int maximo = 0;
  while (millis() - inicio < 50) {
    int lectura = analogRead(pinSonido);
    if (lectura < minimo) minimo = lectura;
    if (lectura > maximo) maximo = lectura;
  }
  int amplitud = maximo - minimo;

  bool pico = (digitalRead(pinPico) == NIVEL_PICO);

  // 2. Convertir a dB aproximados (30 = silencio, 100 = muy fuerte)
  int ampLimitada = constrain(amplitud, 0, ESCALA);
  int db = map(ampLimitada, 0, ESCALA, 30, 100);

  // 3. Pantalla
  lcd.setCursor(0, 0);
  lcd.print("dB:");
  lcd.print(db);
  lcd.print(" Amp:");
  lcd.print(amplitud);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  if (db < 50) {
    lcd.print("Bajo          ");
  } else if (db < 75) {
    lcd.print("Normal        ");
  } else {
    lcd.print("Muy alto!     ");
  }

  lcd.setCursor(15, 1);
  lcd.print(pico ? "!" : " ");

  // 4. LEDs (barra de 0 a 5)
  int cantidadLeds = map(ampLimitada, 0, ESCALA, 0, 5);
  if (amplitud >= RUIDO_MIN && cantidadLeds < 1) {
    cantidadLeds = 1;
  }
  if (pico) {
    cantidadLeds = 5;
  }
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], i < cantidadLeds ? HIGH : LOW);
  }

  Serial.print("Amp: ");
  Serial.print(amplitud);
  Serial.print("  D0: ");
  Serial.println(pico);
}