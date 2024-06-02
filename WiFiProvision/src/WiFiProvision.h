#ifndef WIFIPROVISION_H
#define WIFIPROVISION_H

#if defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif

#include <EEPROM.h>

class WiFiProvision {
public:
    WiFiProvision(const char* ap_ssid, const char* ap_password, int buttonPin, int eepromSize, int ledPin);
    void begin();
    void handleClient();
    void handleLED();

private:
    void startAPMode();
    void handleProvision();
    String readEEPROMString(int start, int length);
    void writeEEPROMString(int start, const String &value, int maxLength);

    const char* _ap_ssid;
    const char* _ap_password;
    int _buttonPin;
    int _eepromSize;
    int _ledPin;
    bool _apMode;

#if defined(ESP32)
    WebServer server;
#elif defined(ESP8266)
    ESP8266WebServer server;
#endif
};

#endif
