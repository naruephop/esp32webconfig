/*
 * Main sketch with inline configuration.
 *
 * Edit the values below to match your board and desired portal behavior.
 */

// GPIO26 is used by the ESP32-S3 OPI flash/PSRAM on this board, so do not
// configure it as a button. Enable this only after assigning a safe GPIO.
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_STATUS_LED 1
#define ESP32WC_FEATURE_SERIAL_LOG 0
#define ESP32WC_FEATURE_PRINT_CONFIG 1
#define ESP32WC_FEATURE_WIFI_SCAN 1
#define ESP32WC_FEATURE_OTA 1
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#define ESP32WC_FEATURE_FILE_MANAGER 1

#define ESP32WC_SECTION_DEVICE_INFO 1
#define ESP32WC_SECTION_MQTT 1
#define ESP32WC_SECTION_LAN 1
#define ESP32WC_SECTION_WLAN 1
#define ESP32WC_SECTION_SERIAL 1
#define ESP32WC_SECTION_MODBUS 1
#define ESP32WC_SECTION_CANBUS 1

// ESP32-S3-ETH onboard W5500 pinout.
// W5500: MOSI=11, MISO=12, SCK=13, CS=14, INT=10, RST=9.
#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_W5500
#define ESP32WC_W5500_SCK_PIN 13
#define ESP32WC_W5500_MISO_PIN 12
#define ESP32WC_W5500_MOSI_PIN 11
#define ESP32WC_W5500_CS_PIN 14
#define ESP32WC_W5500_INT_PIN 10
#define ESP32WC_W5500_RST_PIN 9
#define ESP32WC_W5500_SPI_FREQ_MHZ 20

#include "src/ESP32WebConfigApp.h"

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void configureApp() {
  settings.apSsid = "ESP32_MQTT_Config";
  settings.apPass = "12345678";
  settings.username = "admin";
  settings.password = "admin123";
  settings.deviceName = "ESP32_MQTT_Config";
  settings.firmwareVersion = "1.0.0";

  settings.configButtonPin = 46;
  settings.statusLedPin = 2;
  settings.configButtonUsePullup = true;
  settings.statusLedActiveLevel = HIGH;

  settings.configLedFastBlinkMs = 150;
  settings.configLedSlowBlinkMs = 700;
  settings.printConfigIntervalMs = 15000;

  settings.buttonDebounceMs = 30;
  settings.buttonClickMs = 350;
  settings.buttonLongPress1Ms = 2000;
  settings.buttonLongPress2Ms = 5000;
  settings.buttonLongPress3Ms = 10000;
}

void setup() {
  configureApp();

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
