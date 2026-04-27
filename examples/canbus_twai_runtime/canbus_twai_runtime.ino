/*
 * CAN Bus TWAI runtime example
 *
 * Uses ESP32 built-in TWAI controller.
 * Adjust TX/RX pins to match your transceiver wiring.
 */

#include <ESP32WebConfigApp.h>
#include <driver/twai.h>

ESP32WebConfigApp app;

constexpr gpio_num_t CAN_TX_PIN = GPIO_NUM_5;
constexpr gpio_num_t CAN_RX_PIN = GPIO_NUM_4;

bool canStarted = false;
unsigned long lastSendMs = 0;

twai_timing_config_t timingFromConfig() {
  if (Config.canbus.canbusBaud == "100") return TWAI_TIMING_CONFIG_100KBITS();
  if (Config.canbus.canbusBaud == "125") return TWAI_TIMING_CONFIG_125KBITS();
  if (Config.canbus.canbusBaud == "250") return TWAI_TIMING_CONFIG_250KBITS();
  if (Config.canbus.canbusBaud == "500") return TWAI_TIMING_CONFIG_500KBITS();
  return TWAI_TIMING_CONFIG_1MBITS();
}

uint32_t canIdFromConfig() {
  if (Config.canbus.canbusId.length() == 0) return 0x123;
  return static_cast<uint32_t>(strtoul(Config.canbus.canbusId.c_str(), nullptr, 0));
}

void startCanBus() {
  if (!Config.canbus.canbusMaster || canStarted) return;

  const twai_general_config_t generalConfig =
    TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
  const twai_timing_config_t timingConfig = timingFromConfig();
  const twai_filter_config_t filterConfig = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&generalConfig, &timingConfig, &filterConfig) != ESP_OK) {
    Serial.println("TWAI install failed");
    return;
  }

  if (twai_start() != ESP_OK) {
    Serial.println("TWAI start failed");
    twai_driver_uninstall();
    return;
  }

  canStarted = true;
  Serial.println("CAN Bus started");
}

void sendCanFrame() {
  if (!canStarted || !Config.canbus.canbusMaster) return;

  twai_message_t message = {};
  message.identifier = canIdFromConfig();
  message.extd = 0;
  message.rtr = 0;
  message.data_length_code = 2;
  message.data[0] = 0x12;
  message.data[1] = 0x34;

  if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
    Serial.println("CAN frame sent");
  }
}

void receiveCanFrame() {
  if (!canStarted) return;

  twai_message_t message;
  if (twai_receive(&message, pdMS_TO_TICKS(10)) == ESP_OK) {
    Serial.print("CAN ID: 0x");
    Serial.print(message.identifier, HEX);
    Serial.print(" DLC: ");
    Serial.println(message.data_length_code);
  }
}

void setup() {
  app.setup();
  startCanBus();
}

void loop() {
  app.loop();

  if (!canStarted && Config.canbus.canbusMaster) {
    startCanBus();
  }

  if (canStarted && millis() - lastSendMs >= 1000) {
    lastSendMs = millis();
    sendCanFrame();
  }

  receiveCanFrame();
}
