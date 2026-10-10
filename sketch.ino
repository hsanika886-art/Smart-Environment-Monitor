#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Sensor and buzzer pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define LDRPIN 34
#define BUZZERPIN 25

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1
);

// Alert limits
const float TEMP_LIMIT = 30.0;
const int DARK_LIMIT = 1500;

// Buzzer beep timing
unsigned long previousBeepTime = 0;
bool buzzerState = false;

void setup() {
  Serial.begin(115200);

  pinMode(BUZZERPIN, OUTPUT);
  noTone(BUZZERPIN);

  dht.begin();
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true) {
      delay(10);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SMART ENVIRONMENT");
  display.println("MONITOR STARTING...");
  display.display();

  delay(2000);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int light = analogRead(LDRPIN);

  // Detect alert conditions
  bool hot = !isnan(temperature) &&
             temperature > TEMP_LIMIT;

  // Assumes lower LDR readings mean darker conditions.
  bool dark = light < DARK_LIMIT;

  bool alert = hot || dark;

  // Repeating buzzer sound without blocking sensor readings
  unsigned long currentTime = millis();

  if (alert) {
    if (currentTime - previousBeepTime >= 500) {
      previousBeepTime = currentTime;
      buzzerState = !buzzerState;

      if (buzzerState) {
        tone(BUZZERPIN, 1000);  // 1000 Hz beep
      } else {
        noTone(BUZZERPIN);      // Silence
      }
    }
  } else {
    noTone(BUZZERPIN);
    buzzerState = false;
  }

  // Serial Monitor output
  Serial.println("SMART ENVIRONMENT MONITOR");

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT22 reading error!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
  }

  Serial.print("Light reading: ");
  Serial.println(light);

  Serial.print("Alert: ");
  Serial.println(alert ? "ON" : "OFF");

  Serial.println("---------------------");

  // OLED output
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("SMART ENVIRONMENT");

  display.setCursor(0, 15);
  display.print("Temp: ");

  if (isnan(temperature)) {
    display.println("Sensor error");
  } else {
    display.print(temperature, 1);
    display.println(" C");
  }

  display.setCursor(0, 27);
  display.print("Humidity: ");

  if (isnan(humidity)) {
    display.println("Error");
  } else {
    display.print(humidity, 1);
    display.println(" %");
  }

  display.setCursor(0, 39);
  display.print("Light: ");
  display.println(light);

  display.setCursor(0, 52);
  display.print("Status: ");
  display.println(alert ? "ALERT!" : "NORMAL");

  display.display();

  delay(100);
}
