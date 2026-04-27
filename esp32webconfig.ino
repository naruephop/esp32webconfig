/*
 * Main sketch with inline configuration.
 *
 * Edit the values below to match your board and desired portal behavior.
 */

#define ESP32WC_FEATURE_CONFIG_BUTTON 1
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 1
#define ESP32WC_FEATURE_PRINT_CONFIG 1
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 1
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#define ESP32WC_FEATURE_FILE_MANAGER 1

#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_MQTT 1
#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_SERIAL 1
#define ESP32WC_SECTION_MODBUS 1
#define ESP32WC_SECTION_CANBUS 1

#include "src/ESP32WebConfigApp.h"

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void configureApp() {
  settings.apSsid = "ESP32_MQTT_Config";
  settings.apPass = "12345678";
  settings.username = "admin";
  settings.password = "admin123";
  settings.deviceName = "ESP32_MQTT_Config";
  settings.firmwareVersion = "1.0.0";

  settings.configButtonPin = 26;
  settings.statusLedPin = 23;
  settings.configButtonUsePullup = true;
  settings.statusLedActiveLevel = HIGH;

  settings.configLedFastBlinkMs = 150;
  settings.configLedSlowBlinkMs = 700;
  settings.printConfigIntervalMs = 15000;

  settings.buttonDebounceMs = 30;
  settings.buttonClickMs = 350;
  settings.buttonLongPress1Ms = 2000;
  settings.buttonLongPress2Ms = 5000;
  settings.buttonLongPress3Ms = 10000;
}

void setup() {
  configureApp();

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
