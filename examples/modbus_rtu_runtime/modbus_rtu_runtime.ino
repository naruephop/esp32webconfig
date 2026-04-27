/*
 * Modbus RTU runtime example
 *
 * Uses Serial2 and the values from the web config to send a raw
 * Modbus RTU Read Holding Registers request.
 *
 * Adjust RX/TX pins and RS485 direction pin to match your hardware.
 */

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;
HardwareSerial rs485Serial(2);

constexpr int RS485_RX_PIN = 16;
constexpr int RS485_TX_PIN = 17;
constexpr int RS485_DE_RE_PIN = 4;
constexpr uint16_t MODBUS_START_REGISTER = 0x0000;
constexpr uint16_t MODBUS_REGISTER_COUNT = 2;

unsigned long lastPollMs = 0;

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

uint16_t modbusCrc(const uint8_t* data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t pos = 0; pos < len; pos++) {
    crc ^= static_cast<uint16_t>(data[pos]);
    for (int i = 0; i < 8; i++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void beginRs485Serial() {
  pinMode(RS485_DE_RE_PIN, OUTPUT);
  digitalWrite(RS485_DE_RE_PIN, LOW);

  rs485Serial.begin(
    Config.serial.baud,
    serialModeFromConfig(),
    RS485_RX_PIN,
    RS485_TX_PIN
  );
}

void sendReadHoldingRegisters() {
  if (!Config.modbus.rtuMaster) return;

  uint8_t request[8];
  request[0] = static_cast<uint8_t>(Config.modbus.rtuAddress);
  request[1] = 0x03;
  request[2] = highByte(MODBUS_START_REGISTER);
  request[3] = lowByte(MODBUS_START_REGISTER);
  request[4] = highByte(MODBUS_REGISTER_COUNT);
  request[5] = lowByte(MODBUS_REGISTER_COUNT);

  const uint16_t crc = modbusCrc(request, 6);
  request[6] = lowByte(crc);
  request[7] = highByte(crc);

  digitalWrite(RS485_DE_RE_PIN, HIGH);
  rs485Serial.write(request, sizeof(request));
  rs485Serial.flush();
  digitalWrite(RS485_DE_RE_PIN, LOW);
}

void readResponse() {
  const size_t expectedLength = 5 + (MODBUS_REGISTER_COUNT * 2);
  uint8_t response[16];
  size_t count = 0;

  const unsigned long startMs = millis();
  while ((millis() - startMs) < 300 && count < expectedLength) {
    if (rs485Serial.available()) {
      response[count++] = static_cast<uint8_t>(rs485Serial.read());
    }
  }

  if (count != expectedLength) {
    Serial.println("Modbus RTU response timeout");
    return;
  }

  const uint16_t rxCrc = static_cast<uint16_t>(response[count - 1] << 8) | response[count - 2];
  const uint16_t calcCrc = modbusCrc(response, count - 2);
  if (rxCrc != calcCrc) {
    Serial.println("Modbus RTU CRC error");
    return;
  }

  for (uint16_t i = 0; i < MODBUS_REGISTER_COUNT; i++) {
    const uint16_t reg = static_cast<uint16_t>(response[3 + (i * 2)] << 8) | response[4 + (i * 2)];
    Serial.print("RTU Reg ");
    Serial.print(MODBUS_START_REGISTER + i);
    Serial.print(": ");
    Serial.println(reg);
  }
}

void setup() {
  app.setup();
  beginRs485Serial();
}

void loop() {
  app.loop();

  if (Config.modbus.rtuMaster && millis() - lastPollMs >= 1000) {
    lastPollMs = millis();
    sendReadHoldingRegisters();
    readResponse();
  }
}
