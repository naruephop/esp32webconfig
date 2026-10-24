/*
 * Status LED disabled example
 *
 * Use this when your hardware has no status LED
 * or you do not want the library to blink any pin.
 */

#define ESP32WC_FEATURE_STATUS_LED 0
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
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
  settings.deviceName = "ESP32_No_Status_LED";
  settings.firmwareVersion = "1.0.0";

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
