/*
 * Minimal features example
 *
 * Recommended when you only need the config portal
 * and want to keep the web UI small.
 */

#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 0
#define ESP32WC_FEATURE_OTA 0
#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_CANBUS 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
