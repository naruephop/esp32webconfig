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
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_FILE_MANAGER 1
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#define ESP32WC_FEATURE_WIFI_SCAN 1

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
