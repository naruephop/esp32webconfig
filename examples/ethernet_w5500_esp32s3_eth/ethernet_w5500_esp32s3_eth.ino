/*
 * ESP32-S3-ETH onboard W5500 example.
 *
 * W5500: MOSI=11, MISO=12, SCK=13, CS=14, INT=10, RST=9.
 * Uses the current ETH.h API: SPI.begin(...) followed by ETH.begin(..., SPI).
 */

#define ESP32WC_SECTION_LAN 1
#define ESP32WC_FEATURE_CONFIG_BUTTON 0

#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_W5500
#define ESP32WC_W5500_SCK_PIN 13
#define ESP32WC_W5500_MISO_PIN 12
#define ESP32WC_W5500_MOSI_PIN 11
#define ESP32WC_W5500_CS_PIN 14
#define ESP32WC_W5500_INT_PIN 10
#define ESP32WC_W5500_RST_PIN 9
#define ESP32WC_W5500_SPI_FREQ_MHZ 20

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
