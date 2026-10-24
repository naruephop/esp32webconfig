/*
 * Ethernet / LAN enabled example
 *
 * Intended for original ESP32 boards using an RMII LAN8720 PHY with the defaults.
 * ESP32-S2/S3/C3/C6 targets do not have the RMII EMAC; use the W5500 example
 * for those boards.
 * - MDC: GPIO 23
 * - MDIO: GPIO 18
 * - Clock: GPIO 0 input
 *
 * You can override the pins with:
 *   #define ESP32WC_ETH_PHY_TYPE <phy type>
 *   #define ESP32WC_ETH_PHY_ADDR <address>
 *   #define ESP32WC_ETH_MDC_PIN <pin>
 *   #define ESP32WC_ETH_MDIO_PIN <pin>
 *   #define ESP32WC_ETH_POWER_PIN <pin or -1>
 *   #define ESP32WC_ETH_CLOCK_MODE <clock mode>
 * before including ESP32WebConfigApp.h
 */

#define ESP32WC_SECTION_LAN 1
#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_MQTT 0
#define ESP32WC_SECTION_SERIAL 0
#define ESP32WC_SECTION_MODBUS 0
#define ESP32WC_SECTION_CANBUS 0
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 1
#define ESP32WC_FEATURE_PRINT_CONFIG 0
#define ESP32WC_FEATURE_FILE_MANAGER 1
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 0

// LAN8720 RMII defaults for the original ESP32. Override when wiring differs.
#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_LAN8720
#define ESP32WC_ETH_PHY_TYPE ETH_PHY_LAN8720
#define ESP32WC_ETH_PHY_ADDR 0
#define ESP32WC_ETH_MDC_PIN 23
#define ESP32WC_ETH_MDIO_PIN 18
#define ESP32WC_ETH_POWER_PIN -1
#define ESP32WC_ETH_CLOCK_MODE ETH_CLOCK_GPIO0_IN

#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.deviceName = "ESP32_LAN_Config";
  settings.firmwareVersion = "1.0.0";
  settings.apSsid = "ESP32_LAN_Config";
  settings.apPass = "12345678";

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
