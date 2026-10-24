/*
 * Modbus TCP runtime example
 *
 * Modbus TCP is now built into ESP32WebConfigApp. Enable TCP Client and set
 * the server IP, port, Unit ID and holding-register range from the web UI.
 * This configuration targets a W5500 on a typical ESP32 DevKit. Change the
 * SPI pins below to match your board. Wi-Fi is used if Ethernet has no IP.
 */

// Portal features used by this example.
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 1
#define ESP32WC_FEATURE_PRINT_CONFIG 0
#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 0

// Configuration pages used by this example.
#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_MQTT 0
#define ESP32WC_SECTION_LAN 1
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_SERIAL 0
#define ESP32WC_SECTION_MODBUS 1
#define ESP32WC_SECTION_CANBUS 0

// W5500 wiring: change every pin to match the connected Ethernet module.
#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_W5500
#define ESP32WC_W5500_PHY_ADDR 1
#define ESP32WC_W5500_SCK_PIN 18
#define ESP32WC_W5500_MISO_PIN 19
#define ESP32WC_W5500_MOSI_PIN 23
#define ESP32WC_W5500_CS_PIN 5
#define ESP32WC_W5500_INT_PIN -1
#define ESP32WC_W5500_RST_PIN -1
#define ESP32WC_W5500_SPI_FREQ_MHZ 20

// Runtime buffer capacity. The UI Register Count must not exceed this value.
#define ESP32WC_MODBUS_TCP_MAX_REGISTERS 16

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
