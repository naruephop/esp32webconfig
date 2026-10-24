/*
 * W5500 SPI Ethernet example for a typical ESP32 DevKit.
 *
 * W5500 SCK  -> GPIO 18
 * W5500 MISO -> GPIO 19
 * W5500 MOSI -> GPIO 23
 * W5500 CS   -> GPIO 5
 * W5500 INT  -> not connected (set a GPIO here if used)
 * W5500 RST  -> not connected (set a GPIO here if used)
 */

#define ESP32WC_SECTION_LAN 1
#define ESP32WC_FEATURE_CONFIG_BUTTON 0

#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_W5500
#define ESP32WC_W5500_SCK_PIN 18
#define ESP32WC_W5500_MISO_PIN 19
#define ESP32WC_W5500_MOSI_PIN 23
#define ESP32WC_W5500_CS_PIN 5
#define ESP32WC_W5500_INT_PIN -1
#define ESP32WC_W5500_RST_PIN -1
#define ESP32WC_W5500_SPI_FREQ_MHZ 20

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
