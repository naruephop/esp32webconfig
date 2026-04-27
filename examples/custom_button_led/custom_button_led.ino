/*
 * Custom button and LED example
 *
 * Use this when your board wiring is different from the defaults.
 */

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
