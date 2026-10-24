/*
 * MQTT + Serial focused example
 *
 * Keeps MQTT, WiFi, Serial and Modbus sections visible,
 * while hiding LAN and CAN Bus sections.
 */

// Portal features used by this example.
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 1
#define ESP32WC_FEATURE_PRINT_CONFIG 1
#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 0

#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_MQTT 1
#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_SERIAL 1
#define ESP32WC_SECTION_MODBUS 1
#define ESP32WC_SECTION_CANBUS 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.deviceName = "ESP32_MQTT_Serial";
  settings.firmwareVersion = "1.0.0";
  settings.printConfigIntervalMs = 5000;

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
