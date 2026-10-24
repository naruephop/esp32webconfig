/*
 * Basic example for ESP32WebConfigApp
 *
 * Uses the default library behavior:
 * - AP SSID: ESP32_MQTT_Config
 * - AP password: 12345678
 * - Login: admin / admin123
 * - Status LED: GPIO 2
 *
 * The config button is disabled in this example. Assign a safe GPIO for your
 * board before enabling it; GPIO26 is unavailable on many ESP32-S3 boards.
 */

// Full Wi-Fi configuration portal. LAN is disabled because this example does
// not select Ethernet hardware; use an ethernet_* example for LAN support.
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 1
#define ESP32WC_FEATURE_PRINT_CONFIG 1
#define ESP32WC_FEATURE_FILE_MANAGER 1
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 1

#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_MQTT 1
#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_SERIAL 1
#define ESP32WC_SECTION_MODBUS 1
#define ESP32WC_SECTION_CANBUS 1

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
