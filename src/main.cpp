#define BLYNK_TEMPLATE_ID "TMPL4SlVVAA8d"
#define BLYNK_TEMPLATE_NAME "System do monitorowania wilgotności gleby"

// Zdefiniowanie bibliotek
#define BLYNK_PRINT Serial
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "ps2ECPWncxkw6BoWMNAjcXKvdi1oj0D8";
char ssid[] = "HH71VM_2D72_2.4G";
char pass[] = "iva9i73n";

// Zdefiniowanie obiektu BlynkTimer
BlynkTimer timer;

// Zdefiniowanie pinu czujnika wilgotności gleby
#define sensor A0

// Zdefiniowanie pinu diody LED
#define LED_PIN D3

// Kalibracja - obecne wartości odczytów
const int dryValue = 1023;  // Wartość odczytu w suchym środowisku
const int currentWetValue = 512;  // Wartość odczytu przy 60%, którą teraz ustawimy na 100%

// Inicjalizacja adresu dla LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Deklaracja funkcji readSensor
void readSensor();

// Zmienna do przechowywania stanu diody
bool ledState = 0;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, readSensor); // Odświeżanie co 0,1 sekundy

  // Inicjalizacja LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Witaj!");
  for (int a = 0; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(300);
  }
  lcd.clear();
  
  // Ustawienie pinu diody jako wyjście
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Upewnij się, że dioda jest wyłączona na początku
}

void readSensor() {
  int sensorValue = analogRead(sensor);

  // Przeskalowanie wartości do przedziału 0-100%
  int percentValue = map(sensorValue, dryValue, currentWetValue, 0, 100);
  percentValue = constrain(percentValue, 0, 100); // Ensure values are within 0-100%

  // Wysłanie wartości do Blynk
  Blynk.virtualWrite(V0, percentValue);
  
  // Wysyłanie wartości na Serial Monitor
  Serial.print("Raw Moisture Value: ");
  Serial.print(sensorValue);
  Serial.print(", Percent Value: ");
  Serial.print(percentValue);
  Serial.println("%");

  // Wyświeetlanie wartości z czujnika na LCD
  lcd.setCursor(0, 0);
  lcd.print("Wilgotnosc :");
  lcd.print(percentValue);
  lcd.print("% ");
  delay(200);
}

// Funkcja do obsługi przycisku Blynk na pinie V1
BLYNK_WRITE(V1) {
  int buttonValue = param.asInt(); // Odczytaj stan przycisku
  ledState = (buttonValue > 0); // Ustaw stan diody LED (0 lub 1)
  digitalWrite(LED_PIN, ledState ? HIGH : LOW); // Ustaw stan diody LED
}

void loop() {
  Blynk.run();
  timer.run();
}
