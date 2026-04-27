/*
 * Custom settings example for ESP32WebConfigApp
 *
 * Demonstrates:
 * - overriding default credentials
 * - changing button and LED pins
 * - disabling optional features at compile time
 * - adjusting blink and print timing
 */

#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 0
#define ESP32WC_SECTION_LAN 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.apSsid = "MyESP32Config";
  settings.apPass = "87654321";
  settings.username = "admin";
  settings.password = "secret123";
  settings.firmwareVersion = "2.0.0";
  settings.deviceName = "ESP32_Custom_Config";
  settings.configButtonPin = 0;
  settings.statusLedPin = 2;
  settings.statusLedActiveLevel = LOW;
  settings.configButtonUsePullup = true;
  settings.configLedFastBlinkMs = 120;
  settings.configLedSlowBlinkMs = 600;
  settings.printConfigIntervalMs = 10000;
  settings.buttonDebounceMs = 30;
  settings.buttonClickMs = 350;
  settings.buttonLongPress1Ms = 2000;
  settings.buttonLongPress2Ms = 5000;
  settings.buttonLongPress3Ms = 10000;

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
