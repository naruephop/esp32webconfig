/*
 * Status LED disabled example
 *
 * Use this when your hardware has no status LED
 * or you do not want the library to blink any pin.
 */

#define ESP32WC_FEATURE_STATUS_LED 0

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
