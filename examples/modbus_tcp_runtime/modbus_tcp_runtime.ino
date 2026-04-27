/*
 * Modbus TCP runtime example
 *
 * Uses the Modbus TCP settings from the web config and sends a raw
 * Read Holding Registers request over WiFi.
 */

#include <ESP32WebConfigApp.h>
#include <WiFi.h>

ESP32WebConfigApp app;
WiFiClient modbusTcpClient;

constexpr uint16_t MODBUS_START_REGISTER = 0x0000;
constexpr uint16_t MODBUS_REGISTER_COUNT = 2;

unsigned long lastPollMs = 0;
uint16_t transactionId = 1;

bool connectModbusTcp() {
  if (!Config.modbus.tcpClient) return false;
  if (WiFi.status() != WL_CONNECTED) return false;

  if (modbusTcpClient.connected()) return true;

  IPAddress ip;
  if (!ip.fromString(Config.modbus.server_ip)) {
    Serial.println("Invalid Modbus TCP IP");
    return false;
  }

  return modbusTcpClient.connect(ip, Config.modbus.server_port);
}

void pollModbusTcp() {
  if (!connectModbusTcp()) return;

  uint8_t request[12];
  request[0] = highByte(transactionId);
  request[1] = lowByte(transactionId);
  request[2] = 0x00;
  request[3] = 0x00;
  request[4] = 0x00;
  request[5] = 0x06;
  request[6] = static_cast<uint8_t>(Config.modbus.rtuAddress);
  request[7] = 0x03;
  request[8] = highByte(MODBUS_START_REGISTER);
  request[9] = lowByte(MODBUS_START_REGISTER);
  request[10] = highByte(MODBUS_REGISTER_COUNT);
  request[11] = lowByte(MODBUS_REGISTER_COUNT);

  modbusTcpClient.write(request, sizeof(request));
  transactionId++;

  uint8_t response[32];
  size_t count = 0;
  const unsigned long startMs = millis();
  while ((millis() - startMs) < 500 && count < sizeof(response)) {
    if (modbusTcpClient.available()) {
      response[count++] = static_cast<uint8_t>(modbusTcpClient.read());
      if (count >= 9 && count >= static_cast<size_t>(9 + response[8])) {
        break;
      }
    }
  }

  if (count < 9) {
    Serial.println("Modbus TCP response timeout");
    modbusTcpClient.stop();
    return;
  }

  if (response[7] != 0x03) {
    Serial.println("Unexpected Modbus TCP function code");
    return;
  }

  for (uint16_t i = 0; i < MODBUS_REGISTER_COUNT; i++) {
    const size_t idx = 9 + (i * 2);
    if (idx + 1 >= count) break;
    const uint16_t reg = static_cast<uint16_t>(response[idx] << 8) | response[idx + 1];
    Serial.print("TCP Reg ");
    Serial.print(MODBUS_START_REGISTER + i);
    Serial.print(": ");
    Serial.println(reg);
  }
}

void setup() {
  app.setup();
}

void loop() {
  app.loop();

  if (Config.modbus.tcpClient && millis() - lastPollMs >= 1000) {
    lastPollMs = millis();
    pollModbusTcp();
  }
}
