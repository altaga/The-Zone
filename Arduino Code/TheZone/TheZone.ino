/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <ArduinoJson.h>
#include "certs.h"
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <M5Core2.h>

#define PIN       25 // M5Stack NeoPixel Pin
#define NUMPIXELS 10 // M5 Neopixel Number
#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())

// The name of the device. This MUST match up with the name defined in the AWS console
#define DEVICE_NAME "M5Core"
#define AWS_IOT_TOPIC "/TheZone/M5Core"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);
DynamicJsonDocument doc(2048);
Adafruit_NeoPixel M5pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

String getAddress(String AD);

unsigned long inter;

int devicesDetected = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      doc[getAddress(advertisedDevice.toString().c_str())][0] = advertisedDevice.getRSSI(); //Filter Multiple Detections
    }
};

void setup() {
  inter = millis();
  M5.begin(false, false, false, false);
  M5pixels.begin();
  M5pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
    // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    M5pixels.setPixelColor(i, M5pixels.Color(255, 255, 255));
    M5pixels.show();   // Send the updated pixel colors to the hardware.
    delay(1); // Pause before next pass through loop
  }
  Serial.begin(115200);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  devicesDetected = doc.size();
  if (devicesDetected > 8) {
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      M5pixels.setPixelColor(i, M5pixels.Color(255, 0, 0));
      M5pixels.show();   // Send the updated pixel colors to the hardware.
      delay(1); // Pause before next pass through loop
    }
  }
  else if (devicesDetected < 8 && devicesDetected >= 4) {
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      M5pixels.setPixelColor(i, M5pixels.Color(255, 233, 0));
      M5pixels.show();   // Send the updated pixel colors to the hardware.
      delay(1); // Pause before next pass through loop
    }
  }
  else {
    for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
      // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      M5pixels.setPixelColor(i, M5pixels.Color(0, 255, 0));
      M5pixels.show();   // Send the updated pixel colors to the hardware.
      delay(1); // Pause before next pass through loop
    }
  }
  WiFi.begin(ssid, password);
  connect();
}

void loop() {
  client.loop();
  if ((millis() - inter) > 15000) {
    M5pixels.clear(); // Set all pixel colors to 'off'
    M5pixels.show();
    doc.clear();
    delay(1);
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    devicesDetected = doc.size();
    if (devicesDetected >= 8) {
      for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
        // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        M5pixels.setPixelColor(i, M5pixels.Color(255, 0, 0));
        M5pixels.show();   // Send the updated pixel colors to the hardware.
        delay(1); // Pause before next pass through loop
      }
    }
    else if (devicesDetected < 8 && devicesDetected >= 4) {
      for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
        // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        M5pixels.setPixelColor(i, M5pixels.Color(255, 233, 0));
        M5pixels.show();   // Send the updated pixel colors to the hardware.
        delay(1); // Pause before next pass through loop
      }
    }
    else {
      for (int i = 0; i < NUMPIXELS; i++) { // For each pixel...
        // M5pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        M5pixels.setPixelColor(i, M5pixels.Color(0, 255, 0));
        M5pixels.show();   // Send the updated pixel colors to the hardware.
        delay(1); // Pause before next pass through loop
      }
    }
    String output1 = "";
    serializeJson(doc, output1);
    client.publish(AWS_IOT_TOPIC, string2char(output1));
    inter = millis();
  }
  if (!client.connected()) {
    connect();
  }
}

String getAddress(String AD) {
  int place = AD.indexOf("Address");
  return AD.substring(place + 9, place + 26);
}

char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

void connectToWiFi() {
  int conCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    conCounter++;
    if (conCounter == 10) {
      ESP.restart();
    }
  }
}

void connectToAWS()
{
  // Configure WiFiClientSecure to use the AWS certificates we generated
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Try to connect to AWS and count how many times we retried.

  while (!client.connect(DEVICE_NAME)) {
    delay(100);
  }

  // Make sure that we did indeed successfully connect to the MQTT broker
  // If not we just end the function and wait for the next loop.
  if (!client.connected()) {
    connect();
  }

}

void connect() {
  connectToWiFi();
  Serial.println("WiFi OK");
  connectToAWS();
  Serial.println("AWS OK");
}
