#pragma once

#include <ETH.h>
#include <WiFi.h>
#include "ESP32WebConfig.h"

enum class ESP32WCModbusNetwork : uint8_t { None, Ethernet, WiFi };

inline NetworkClient modbusTcpClient;
inline ESP32WCModbusNetwork modbusTcpActiveNetwork = ESP32WCModbusNetwork::None;
inline unsigned long modbusTcpLastPollMs = 0;
inline unsigned long modbusTcpLastSuccessMs = 0;
inline uint16_t modbusTcpTransactionId = 1;
inline uint16_t modbusTcpRegisters[ESP32WC_MODBUS_TCP_MAX_REGISTERS] = {};
inline uint8_t modbusTcpAvailableRegisterCount = 0;
inline bool modbusTcpDataAvailable = false;

// Read values fetched by the most recent successful Modbus TCP poll.
// Index 0 maps to Config.modbus.tcpStartRegister.
inline bool modbusTcpReadRegister(uint8_t index, uint16_t &value) {
  if (!modbusTcpDataAvailable || index >= modbusTcpAvailableRegisterCount) return false;
  value = modbusTcpRegisters[index];
  return true;
}

inline uint8_t modbusTcpRegisterCount() {
  return modbusTcpDataAvailable ? modbusTcpAvailableRegisterCount : 0;
}

inline bool modbusTcpHasData() {
  return modbusTcpDataAvailable;
}

inline void modbusTcpClearData() {
  modbusTcpAvailableRegisterCount = 0;
  modbusTcpDataAvailable = false;
}

inline uint8_t modbusTcpRequestedRegisterCount() {
  uint8_t registerCount = Config.modbus.tcpRegisterCount;
  if (registerCount < 1 || registerCount > ESP32WC_MODBUS_TCP_MAX_REGISTERS) registerCount = 2;
  return registerCount;
}

inline bool modbusTcpSelectNetwork() {
  ESP32WCModbusNetwork nextNetwork = ESP32WCModbusNetwork::None;

  // Ethernet has priority whenever the W5500/RMII interface has an IP lease.
  if (ETH.hasIP()) {
    ETH.setDefault();
    nextNetwork = ESP32WCModbusNetwork::Ethernet;
  } else if (WiFi.status() == WL_CONNECTED) {
    WiFi.STA.setDefault();
    nextNetwork = ESP32WCModbusNetwork::WiFi;
  }

  if (nextNetwork != modbusTcpActiveNetwork) {
    modbusTcpClient.stop();
    modbusTcpActiveNetwork = nextNetwork;
#if ESP32WC_FEATURE_SERIAL_LOG
    if (modbusTcpActiveNetwork == ESP32WCModbusNetwork::Ethernet) {
      Serial.println("Modbus TCP network: Ethernet");
    } else if (modbusTcpActiveNetwork == ESP32WCModbusNetwork::WiFi) {
      Serial.println("Modbus TCP network: WiFi");
    }
#endif
  }

  return modbusTcpActiveNetwork != ESP32WCModbusNetwork::None;
}

inline bool modbusTcpConnect() {
  if (!Config.modbus.tcpClient || !modbusTcpSelectNetwork()) return false;
  if (modbusTcpClient.connected()) return true;

  IPAddress serverIp;
  if (!serverIp.fromString(Config.modbus.server_ip)) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Invalid Modbus TCP IP");
#endif
    return false;
  }

  return modbusTcpClient.connect(serverIp, Config.modbus.server_port);
}

inline void modbusTcpPoll() {
  if (!modbusTcpConnect()) return;

  const uint16_t startRegister = Config.modbus.tcpStartRegister;
  const uint8_t registerCount = modbusTcpRequestedRegisterCount();

  uint8_t request[12];
  request[0] = highByte(modbusTcpTransactionId);
  request[1] = lowByte(modbusTcpTransactionId);
  request[2] = 0x00;
  request[3] = 0x00;
  request[4] = 0x00;
  request[5] = 0x06;
  request[6] = static_cast<uint8_t>(Config.modbus.tcpUnitId);
  request[7] = 0x03;  // Read Holding Registers
  request[8] = highByte(startRegister);
  request[9] = lowByte(startRegister);
  request[10] = highByte(registerCount);
  request[11] = lowByte(registerCount);

  if (modbusTcpClient.write(request, sizeof(request)) != sizeof(request)) {
    modbusTcpClient.stop();
    return;
  }
  modbusTcpTransactionId++;

  uint8_t response[9 + (ESP32WC_MODBUS_TCP_MAX_REGISTERS * 2)];
  size_t count = 0;
  const unsigned long startMs = millis();
  while ((millis() - startMs) < 500 && count < sizeof(response)) {
    if (modbusTcpClient.available()) {
      response[count++] = static_cast<uint8_t>(modbusTcpClient.read());
      if (count >= 9 && count >= static_cast<size_t>(9 + response[8])) break;
    }
    delay(1);
  }

  if (count < 9 || response[7] != 0x03 || response[8] != registerCount * 2 ||
      count < static_cast<size_t>(9 + response[8])) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println(count < 9 ? "Modbus TCP response timeout" : "Invalid Modbus TCP response");
#endif
    modbusTcpClient.stop();
    modbusTcpClearData();
    return;
  }

  for (uint8_t index = 0; index < registerCount; index++) {
    const size_t responseIndex = 9 + (index * 2);
    modbusTcpRegisters[index] = static_cast<uint16_t>(response[responseIndex] << 8) | response[responseIndex + 1];
  }
  modbusTcpAvailableRegisterCount = registerCount;
  modbusTcpDataAvailable = true;
  modbusTcpLastSuccessMs = millis();

#if ESP32WC_FEATURE_SERIAL_LOG
  for (uint8_t index = 0; index < registerCount; index++) {
    Serial.print("TCP Reg ");
    Serial.print(startRegister + index);
    Serial.print(": ");
    Serial.println(modbusTcpRegisters[index]);
  }
#endif
}

inline void modbusTcpRuntimeLoop() {
  if (!Config.modbus.tcpClient) {
    if (modbusTcpClient.connected()) modbusTcpClient.stop();
    modbusTcpActiveNetwork = ESP32WCModbusNetwork::None;
    modbusTcpClearData();
    return;
  }

  const unsigned long now = millis();
  const unsigned long pollInterval = Config.modbus.tcpPollIntervalMs < 100 ? 100 : Config.modbus.tcpPollIntervalMs;
  if (now - modbusTcpLastPollMs >= pollInterval) {
    modbusTcpLastPollMs = now;
    modbusTcpPoll();
  }
}
