/*
 * MQTT + Serial focused example
 *
 * Keeps MQTT, WiFi, Serial and Modbus sections visible,
 * while hiding LAN and CAN Bus sections.
 */

#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_CANBUS 0
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_FILE_MANAGER 0

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
