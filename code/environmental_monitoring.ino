#include <WiFi.h>
#include "DHT.h"
#include <HTTPClient.h>

// Sensor configuration
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 34

DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingSpeak configuration
String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
String server = "https://api.thingspeak.com/update";

void setup() {
  Serial.begin(115200);

  // Initialize DHT11 sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void loop() {
  // Read temperature and humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Read MQ135 sensor value
  int mq = analogRead(MQ135_PIN);

  // Print sensor readings
  Serial.printf(
    "Temp: %.2f, Hum: %.2f, MQ135: %d\n",
    t,
    h,
    mq
  );

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = server;
    url += "?api_key=" + apiKey;
    url += "&field1=" + String(t);
    url += "&field2=" + String(h);
    url += "&field3=" + String(mq);

    http.begin(url);

    int httpCode = http.GET();

    Serial.println("HTTP Code: " + String(httpCode));

    if (httpCode > 0) {
      Serial.println("ThingSpeak update success!");
    } else {
      Serial.println("ThingSpeak update failed!");
    }

    http.end();
  }

  // ThingSpeak update interval
  delay(20000);
}
