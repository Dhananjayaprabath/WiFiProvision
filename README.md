# WiFiProvision

WiFiProvision is a library for WiFi provisioning on ESP32 using a web server in AP mode.

## Installation

1. Download the library as a zip file.
2. Open the Arduino IDE.
3. Go to `Sketch` > `Include Library` > `Add .ZIP Library`.
4. Select the downloaded zip file.

## Usage

Here is a basic example of how to use the library:

```cpp
#include <WiFiProvision.h>

const char* ap_ssid = "ESP32-AP";
const char* ap_password = "password123";
const int buttonPin = 0;

WiFiProvision wifiProvision(ap_ssid, ap_password, buttonPin, 512);

void setup() {
  wifiProvision.begin();
}

void loop() {
  wifiProvision.handleClient();
}
