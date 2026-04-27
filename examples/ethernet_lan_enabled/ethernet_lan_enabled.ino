/*
 * Ethernet / LAN enabled example
 *
 * Intended for boards using W5500 with the library defaults:
 * - CS  pin: GPIO 15
 * - RST pin: GPIO 27
 *
 * You can override the pins with:
 *   #define ESP32WC_W5500_CS_PIN <pin>
 *   #define ESP32WC_W5500_RST_PIN <pin>
 * before including ESP32WebConfigApp.h
 */

#define ESP32WC_SECTION_LAN 1
#define ESP32WC_FEATURE_FILE_MANAGER 1
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#define ESP32WC_FEATURE_WIFI_SCAN 1

#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.deviceName = "ESP32_LAN_Config";
  settings.firmwareVersion = "1.0.0";
  settings.apSsid = "ESP32_LAN_Config";
  settings.apPass = "12345678";

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
