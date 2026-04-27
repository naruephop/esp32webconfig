/*
 * Basic example for ESP32WebConfigApp
 *
 * Uses the default library behavior:
 * - AP SSID: ESP32_MQTT_Config
 * - AP password: 12345678
 * - Login: admin / admin123
 * - Config button: GPIO 26
 * - Status LED: GPIO 23
 */

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
