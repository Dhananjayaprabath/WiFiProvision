#include "WiFiProvision.h"

WiFiProvision::WiFiProvision(const char* ap_ssid, const char* ap_password, int buttonPin, int eepromSize, int ledPin)
    : _ap_ssid(ap_ssid), _ap_password(ap_password), _buttonPin(buttonPin), _eepromSize(eepromSize), _ledPin(ledPin), _apMode(false), server(80) {
}

void WiFiProvision::begin() {
    Serial.begin(115200);
    EEPROM.begin(_eepromSize);
    pinMode(_buttonPin, INPUT_PULLUP);
    pinMode(_ledPin, OUTPUT);

    if (digitalRead(_buttonPin) == LOW) {
        _apMode = true;
        Serial.println("Button pressed, entering AP mode...");
        startAPMode();
    } else {
        String stored_ssid = readEEPROMString(0, 32);
        String stored_password = readEEPROMString(32, 64);

        if (stored_ssid.length() > 0 && stored_password.length() > 0) {
            WiFi.begin(stored_ssid.c_str(), stored_password.c_str());
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                Serial.print(".");
                attempts++;
            }

            if (WiFi.status() == WL_CONNECTED) {
                Serial.println();
                Serial.print("Connected to Wi-Fi network with IP: ");
                Serial.println(WiFi.localIP());
                return;
            }
        }
        startAPMode();
    }
}

void WiFiProvision::handleClient() {
    server.handleClient();
    handleLED();
}

void WiFiProvision::handleLED() {
    static unsigned long lastMillis = 0;
    unsigned long currentMillis = millis();

    if (_apMode) {
        // Blink LED for AP mode
        if (currentMillis - lastMillis >= 500) {
            digitalWrite(_ledPin, !digitalRead(_ledPin));
            lastMillis = currentMillis;
        }
    } else {
        // Solid LED for connected mode
        digitalWrite(_ledPin, HIGH);
    }
}

void WiFiProvision::startAPMode() {
    WiFi.softAP(_ap_ssid, _ap_password);
    Serial.print("Access Point IP Address: ");
    Serial.println(WiFi.softAPIP());
    server.on("/provision", HTTP_POST, std::bind(&WiFiProvision::handleProvision, this));
    server.begin();
}

void WiFiProvision::handleProvision() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
        String ssid = server.arg("ssid");
        String password = server.arg("password");

        writeEEPROMString(0, ssid, 32);
        writeEEPROMString(32, password, 32);
        EEPROM.commit();

        WiFi.begin(ssid.c_str(), password.c_str());

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            server.send(200, "text/plain", "Wi-Fi Provisioning Successful. Rebooting...");
            delay(1000);
            ESP.restart();
        } else {
            server.send(500, "text/plain", "Failed to Connect to Wi-Fi Network");
        }
    } else {
        server.send(400, "text/plain", "Missing SSID or Password");
    }
}

String WiFiProvision::readEEPROMString(int start, int length) {
    String value;
    for (int i = start; i < start + length; ++i) {
        value += char(EEPROM.read(i));
    }
    value.trim();
    return value;
}

void WiFiProvision::writeEEPROMString(int start, const String &value, int maxLength) {
    for (int i = 0; i < value.length() && i < maxLength; ++i) {
        EEPROM.write(start + i, value[i]);
    }
    for (int i = value.length(); i < maxLength; ++i) {
        EEPROM.write(start + i, 0);
    }
}

