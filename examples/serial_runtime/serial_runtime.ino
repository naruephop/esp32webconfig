/*
 * Serial runtime example
 *
 * Reads Serial settings from the web config and applies them to Serial2.
 * Adjust RX/TX pins below to match your board wiring.
 */

#define ESP32WC_FEATURE_CONFIG_BUTTON 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;
HardwareSerial userSerial(2);

constexpr int USER_SERIAL_RX_PIN = 16;
constexpr int USER_SERIAL_TX_PIN = 17;

uint32_t serialModeFromConfig() {
  const bool even = strcmp(Config.serial.parity, "even") == 0;
  const bool odd = strcmp(Config.serial.parity, "odd") == 0;

  if (Config.serial.dataBits == 7) {
    if (even && Config.serial.stopBits == 1) return SERIAL_7E1;
    if (odd && Config.serial.stopBits == 1) return SERIAL_7O1;
  }

  if (Config.serial.dataBits == 8) {
    if (even && Config.serial.stopBits == 1) return SERIAL_8E1;
    if (odd && Config.serial.stopBits == 1) return SERIAL_8O1;
    if (!even && !odd && Config.serial.stopBits == 2) return SERIAL_8N2;
  }

  return SERIAL_8N1;
}

void beginConfiguredSerial() {
  userSerial.begin(
    Config.serial.baud,
    serialModeFromConfig(),
    USER_SERIAL_RX_PIN,
    USER_SERIAL_TX_PIN
  );

  Serial.println("Configured Serial2 from web settings");
  Serial.print("Baud: ");
  Serial.println(Config.serial.baud);
}

void setup() {
  app.setup();
  beginConfiguredSerial();
}

void loop() {
  app.loop();

  if (userSerial.available()) {
    while (userSerial.available()) {
      Serial.write(userSerial.read());
    }
  }

  if (Serial.available()) {
    while (Serial.available()) {
      userSerial.write(Serial.read());
    }
  }
}
