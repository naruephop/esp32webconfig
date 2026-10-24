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

#define ESP32WC_FEATURE_CONFIG_BUTTON 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
