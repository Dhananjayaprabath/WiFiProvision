#include <WiFiProvision.h>

const char* ap_ssid = "ESP32-AP";
const char* ap_password = "password123";
const int buttonPin = 27;
const int ledPin = 12; // Change this to your desired LED pin

WiFiProvision wifiProvision(ap_ssid, ap_password, buttonPin, 512, ledPin);

void setup() {
    wifiProvision.begin();
}

void loop() {
    wifiProvision.handleClient();
}
