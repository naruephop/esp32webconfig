/*
 * Custom button and LED example
 *
 * Use this when your board wiring is different from the defaults.
 */

// This sketch uses the physical configuration button and status LED below.
#define ESP32WC_FEATURE_CONFIG_BUTTON 1
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 1
#define ESP32WC_FEATURE_PRINT_CONFIG 0
#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 0

#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_MQTT 0
#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_SERIAL 0
#define ESP32WC_SECTION_MODBUS 0
#define ESP32WC_SECTION_CANBUS 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.deviceName = "ESP32_Custom_IO";
  settings.firmwareVersion = "1.0.0";

  // Example wiring:
  // - button on GPIO 4, active LOW with pull-up
  // - LED on GPIO 2, active LOW
  settings.configButtonPin = 4;
  settings.configButtonUsePullup = true;
  settings.statusLedPin = 2;
  settings.statusLedActiveLevel = LOW;

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
