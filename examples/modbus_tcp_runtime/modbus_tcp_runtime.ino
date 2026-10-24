/*
 * Modbus TCP runtime example
 *
 * Modbus TCP is now built into ESP32WebConfigApp. Enable TCP Client and set
 * the server IP, port, Unit ID and holding-register range from the web UI.
 * Ethernet is preferred;
 * WiFi is used when Ethernet has no IP address.
 */

#define ESP32WC_FEATURE_CONFIG_BUTTON 0

#include <ESP32WebConfigApp.h>
ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();

  // Index 0 is the Start Register configured on the web page.
  // Replace this comment with the action required by your application.
  uint16_t firstRegister;
  if (modbusTcpReadRegister(0, firstRegister)) {
    // Example: use firstRegister to control your application.
  }
}
