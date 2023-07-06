#include <Arduino.h>
#include <Wire.h>
#include <DHTesp.h>
#include <ThingsBoard.h>
#include <WiFi.h>
#include <BH1750.h>
#include <Ticker.h>

#define UPDATE_DATA_INTERVAL 5000
#define wifi_ssid "Wifi Orang Ganteng_EXT"
#define wifi_password "30150003"
#define thingsboard_server "demo.thingsboard.io"
#define thingsboard_access_token "Y3wAsOZLmke3TfVqHgon"

#define DHTPIN 18
#define PIN_SDA 21
#define PIN_SCL 22

WiFiClient espClient;
ThingsBoard tb(espClient);
Ticker timerSendData;
DHTesp sensorDHT;
BH1750 pengukurcahaya;



void WifiConnect();
void onSendSensor();

void setup() {
  Serial.begin(9600);
  sensorDHT.setup(DHTPIN, DHTesp::DHT11);
  Wire.begin(21, 22);
  pengukurcahaya.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire);
  
  WifiConnect();
  timerSendData.attach_ms(UPDATE_DATA_INTERVAL, onSendSensor);  // Fix the ticker error
}

void loop() {
  if (!tb.connected()) {
    if (tb.connect(thingsboard_server, thingsboard_access_token)) {
      Serial.println("Masuk ke Thingsboard");
    } else {
      Serial.println("Gagal Masuk ke Thingsboard");
    }
  }
  
  tb.loop();
}

void onSendSensor() {
  float temperature = sensorDHT.getTemperature();
  float humidity = sensorDHT.getHumidity();
  float cahaya = pengukurcahaya.readLightLevel();
  if (sensorDHT.getStatusString() != "OK") {
    Serial.println("Gagal membaca sensor DHT");
    return;
  }
  
  Serial.println("Mengirim data ke Thingsboard");
  Serial.printf("Suhu: %.2f C, Kelembaban: %.2f %% , Cahaya: %.2f lux\n", temperature, humidity, cahaya);
  
  if (tb.connected()) {
    tb.sendTelemetryFloat("temperature", temperature);
    tb.sendTelemetryFloat("humidity", humidity);
    tb.sendTelemetryFloat("cahaya", cahaya);
  } else {
    Serial.println("Gagal mengirim data ke Thingsboard");
  }
}

void WifiConnect() {
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Gagal Untuk Connect! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("System connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
}