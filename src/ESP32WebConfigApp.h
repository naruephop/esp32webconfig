#pragma once

#include <WiFi.h>
#include <SPI.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <FS.h>
#include <SPIFFS.h>
#include "Button.h"
#include "ESP32WebConfig.h"
#include "WebHandlers.h"
#include "WebHandlersImpl.h"
#include "NetworkConfig.h"

inline const char* apSSID = "ESP32_MQTT_Config";
inline const char* apPassword = "12345678";
inline const char* USERNAME = "admin";
inline const char* PASSWORD = "admin123";
inline const char* deviceversion = "1.0.0";
inline const char* devicename = "ESP32_MQTT_Config";

inline WebServer server(80);
inline Configsettings Config;
inline String myActiveToken = "";
inline bool configPortalEnabled = true;
inline bool showDeviceInfo = ESP32WC_SECTION_DEVICE_INFO;
inline bool showMQTTConfig = ESP32WC_SECTION_MQTT;
inline bool showLanSettings = ESP32WC_SECTION_LAN;
inline bool showWlanSettings = ESP32WC_SECTION_WLAN;
inline bool showSerialSettings = ESP32WC_SECTION_SERIAL;
inline bool showModbusSettings = ESP32WC_SECTION_MODBUS;
inline bool showCanbusSettings = ESP32WC_SECTION_CANBUS;

struct ESP32WebConfigAppSettings {
  const char* apSsid = "ESP32_MQTT_Config";
  const char* apPass = "12345678";
  const char* username = "admin";
  const char* password = "admin123";
  const char* firmwareVersion = "1.0.0";
  const char* deviceName = "ESP32_MQTT_Config";
  uint8_t configButtonPin = 26;
  uint8_t statusLedPin = 23;
  bool configButtonUsePullup = true;
  uint8_t statusLedActiveLevel = HIGH;
  unsigned long configLedFastBlinkMs = 150;
  unsigned long configLedSlowBlinkMs = 700;
  unsigned long printConfigIntervalMs = 15000;
  uint16_t buttonDebounceMs = 30;
  uint16_t buttonClickMs = 350;
  uint16_t buttonLongPress1Ms = 2000;
  uint16_t buttonLongPress2Ms = 5000;
  uint16_t buttonLongPress3Ms = 10000;
};

class ESP32WebConfigApp {
public:
  explicit ESP32WebConfigApp(const ESP32WebConfigAppSettings& settings = ESP32WebConfigAppSettings())
      : settings_(settings)
#if ESP32WC_FEATURE_CONFIG_BUTTON
      , configButton_({
          settings.configButtonPin,
          settings.configButtonUsePullup,
          settings.buttonDebounceMs,
          settings.buttonClickMs,
          settings.buttonLongPress1Ms,
          settings.buttonLongPress2Ms,
          settings.buttonLongPress3Ms
        })
#endif
  {
    applySettings();
  }

  void setup() {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.begin(115200);
#endif
#if ESP32WC_FEATURE_STATUS_LED
    pinMode(settings_.statusLedPin, OUTPUT);
#endif
    applyStatusLed(false);
#if ESP32WC_FEATURE_CONFIG_BUTTON
    configButton_.begin();
#endif

    loadSettings();
    if (configPortalEnabled) {
      startConfigPortal();
    } else {
      stopConfigPortal();
    }

    const bool ethConnected = ::startEthernet();
    if (!ethConnected) {
      ::initWiFi();
    }
    setupRoutes();
    server.begin();
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("HTTP server started");
#endif
  }

  void loop() {
    const unsigned long now = millis();

    server.handleClient();
    handleConfigButton();
    updateStatusLed();
    ::network_switching();

#if ESP32WC_FEATURE_PRINT_CONFIG
    if (now - lastPrintConfigMs_ >= settings_.printConfigIntervalMs) {
      lastPrintConfigMs_ = now;
      printConfig();
    }
#endif

    delay(10);
  }

private:
  void applySettings() {
    apSSID = settings_.apSsid;
    apPassword = settings_.apPass;
    USERNAME = settings_.username;
    PASSWORD = settings_.password;
    deviceversion = settings_.firmwareVersion;
    devicename = settings_.deviceName;
  }

  String ipAddressToString(const IPAddress& ipAddress) const {
    return String(ipAddress[0]) + "." +
           String(ipAddress[1]) + "." +
           String(ipAddress[2]) + "." +
           String(ipAddress[3]);
  }

  void initWiFi() {
    if (Config.wlan.ssid == "") {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("Undefined SSID");
#endif
      return;
    }

    IPAddress localIP;
    IPAddress defaultGateway;
    IPAddress subnetMask;
    IPAddress primaryDns;

    localIP.fromString(Config.wlan.ip.c_str());
    defaultGateway.fromString(Config.wlan.gateway.c_str());
    subnetMask.fromString(Config.wlan.subnet.c_str());
    primaryDns.fromString(Config.wlan.dns.c_str());

    if (!Config.wlan.dhcp) {
      if (!WiFi.config(localIP, defaultGateway, subnetMask, primaryDns)) {
#if ESP32WC_FEATURE_SERIAL_LOG
        Serial.println("STA Failed to configure");
#endif
      }
    }

    WiFi.begin(Config.wlan.ssid, Config.wlan.password);

    uint8_t cnt = 0;
    while (WiFi.status() != WL_CONNECTED && cnt < 15) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.print(".");
#endif
      delay(400);
      cnt++;
    }

    if (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      return;
    }

#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.print("WiFi IP address: ");
    Serial.println(WiFi.localIP());
#endif

    Config.wlan.ip = ipAddressToString(WiFi.localIP());
    Config.wlan.gateway = ipAddressToString(WiFi.gatewayIP());
    Config.wlan.subnet = ipAddressToString(WiFi.subnetMask());
    Config.wlan.dns = ipAddressToString(WiFi.dnsIP());
    Config.wlan.mac = WiFi.macAddress();

    delay(200);
    saveSettings(false);
  }

  void setupRoutes() {
    server.on("/", handleRoot);
    server.on("/login", HTTP_GET, handleLoginForm);
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_POST, handleLogout);
    server.on("/load", HTTP_GET, handleLoad);
    server.on("/save", HTTP_POST, handleSave);
#if ESP32WC_FEATURE_OTA
    server.on("/update", HTTP_GET, handleUpdateForm);
    server.on("/update", HTTP_POST, []() {}, handleUpdate);
#endif
#if ESP32WC_FEATURE_SPIFFS_UPDATE
    server.on("/update-spiffs", HTTP_GET, handleSpiffsUpdateForm);
    server.on("/update-spiffs", HTTP_POST, []() {}, handleSpiffsUpdate);
#endif
#if ESP32WC_FEATURE_FILE_MANAGER
    server.on("/file-manager", HTTP_GET, handleFileManager);
    server.on("/file-list", HTTP_GET, handleFileList);
    server.on("/download-file", HTTP_GET, handleDownloadFile);
    server.on("/delete-file", HTTP_GET, handleDeleteFile);
    server.on("/upload", HTTP_GET, handleFileUploadPage);
    server.on("/upload", HTTP_POST, []() {}, handleFileUpload);
#endif
#if ESP32WC_FEATURE_WIFI_SCAN
    server.on("/scan_wifi", HTTP_GET, handleScanWiFi);
#endif
    server.on("/check_token", HTTP_GET, handleCheckToken);
  }

  void applyStatusLed(bool ledOn) {
#if ESP32WC_FEATURE_STATUS_LED
    statusLedState_ = ledOn;
    digitalWrite(settings_.statusLedPin, ledOn ? settings_.statusLedActiveLevel : statusLedInactiveLevel());
#else
    (void)ledOn;
#endif
  }

  void updateStatusLed() {
#if ESP32WC_FEATURE_STATUS_LED
    const unsigned long now = millis();
    const bool wifiConnected = (WiFi.status() == WL_CONNECTED);

    if (wifiConnected && !configPortalEnabled) {
      applyStatusLed(true);
      return;
    }

    if (configPortalEnabled && wifiConnected) {
      if (now - lastStatusLedToggleMs_ >= settings_.configLedSlowBlinkMs) {
        lastStatusLedToggleMs_ = now;
        applyStatusLed(!statusLedState_);
      }
      return;
    }

    if (configPortalEnabled && !wifiConnected) {
      if (now - lastStatusLedToggleMs_ >= settings_.configLedFastBlinkMs) {
        lastStatusLedToggleMs_ = now;
        applyStatusLed(!statusLedState_);
      }
      return;
    }

    applyStatusLed(false);
#endif
  }

  void startConfigPortal() {
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAP(apSSID, apPassword);
    configPortalEnabled = true;
#if ESP32WC_FEATURE_STATUS_LED
    lastStatusLedToggleMs_ = millis();
#endif
    applyStatusLed(false);
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println();
    Serial.print("Config portal enabled. AP IP: ");
    Serial.println(WiFi.softAPIP());
#endif
  }

  void stopConfigPortal() {
    configPortalEnabled = false;
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_MODE_STA);
    applyStatusLed(false);
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Config portal disabled");
#endif
  }

  void blinkStatusLed(uint8_t times, uint16_t intervalMs) {
#if ESP32WC_FEATURE_STATUS_LED
    for (uint8_t i = 0; i < times; i++) {
      applyStatusLed(true);
      delay(intervalMs);
      applyStatusLed(false);
      delay(intervalMs);
    }
    updateStatusLed();
#else
    (void)times;
    (void)intervalMs;
#endif
  }

  void resetConfigToDefaults() {
    Config = Configsettings();
    myActiveToken = "";
    configPortalEnabled = true;

    strlcpy(Config.device.deviceName, devicename, sizeof(Config.device.deviceName));
    strlcpy(Config.device.firmwareVersion, deviceversion, sizeof(Config.device.firmwareVersion));
    Config.mqtt.port = 1883;
    Config.serial.baud = 9600;
    Config.serial.dataBits = 8;
    strlcpy(Config.serial.parity, "none", sizeof(Config.serial.parity));
    Config.serial.stopBits = 1;
    Config.modbus.server_port = 502;
    Config.modbus.rtuAddress = 1;
    Config.lan.dhcp = true;
    Config.wlan.dhcp = true;
    Config.canbus.canbusBaud = "100";

    saveSettings(false);
    blinkStatusLed(6, 100);
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Configuration reset to defaults. Restarting...");
#endif
    ESP.restart();
  }

  void handleConfigButton() {
#if ESP32WC_FEATURE_CONFIG_BUTTON
    const Button::Event event = configButton_.update();

    if (event == Button::CLICK_1) {
      if (configPortalEnabled) {
        stopConfigPortal();
      } else {
        startConfigPortal();
      }
      saveSettings(false);
    }

    if (event == Button::LONG_2) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("Reset button long press detected");
#endif
      resetConfigToDefaults();
    }
#endif
  }

  void printConfig() {
#if ESP32WC_FEATURE_PRINT_CONFIG && ESP32WC_FEATURE_SERIAL_LOG
    if (showDeviceInfo) {
      Serial.println("=== Device Info ===");
      Serial.print("Device Name: ");        Serial.println(Config.device.deviceName);
      Serial.print("Firmware Version: ");   Serial.println(Config.device.firmwareVersion);
    }
    if (showMQTTConfig) {
      Serial.println("\n=== MQTT Config ===");
      Serial.print("Server: ");             Serial.println(Config.mqtt.server);
      Serial.print("Port: ");               Serial.println(Config.mqtt.port);
      Serial.print("User: ");               Serial.println(Config.mqtt.user);
      Serial.println("Password: [hidden]");
      Serial.print("Publish Topic: ");      Serial.println(Config.mqtt.publishTopic);
      Serial.print("Subscribe Topic: ");    Serial.println(Config.mqtt.subscribeTopic);
      Serial.print("Sampling Time: ");      Serial.println(Config.mqtt.samplingtime);
      Serial.print("Enable MQTT: ");        Serial.println(Config.mqtt.enableMQTT ? "true" : "false");
    }
    if (showLanSettings) {
      Serial.println("\n=== LAN Settings ===");
      Serial.print("DHCP: ");               Serial.println(Config.lan.dhcp ? "true" : "false");
      Serial.print("MAC: ");                Serial.println(Config.lan.mac);
      Serial.print("IP: ");                 Serial.println(Config.lan.ip);
      Serial.print("Subnet: ");             Serial.println(Config.lan.subnet);
      Serial.print("Gateway: ");            Serial.println(Config.lan.gateway);
      Serial.print("DNS: ");                Serial.println(Config.lan.dns);
    }
    if (showWlanSettings) {
      Serial.println("\n=== WLAN Settings ===");
      Serial.print("DHCP: ");               Serial.println(Config.wlan.dhcp ? "true" : "false");
      Serial.print("MAC: ");                Serial.println(Config.wlan.mac);
      Serial.print("SSID: ");               Serial.println(Config.wlan.ssid);
      Serial.println("Password: [hidden]");
      Serial.print("IP: ");                 Serial.println(Config.wlan.ip);
      Serial.print("Subnet: ");             Serial.println(Config.wlan.subnet);
      Serial.print("Gateway: ");            Serial.println(Config.wlan.gateway);
      Serial.print("DNS: ");                Serial.println(Config.wlan.dns);
    }
    if (showSerialSettings) {
      Serial.println("\n=== Serial Settings ===");
      Serial.print("Baud: ");               Serial.println(Config.serial.baud);
      Serial.print("Data Bits: ");          Serial.println(Config.serial.dataBits);
      Serial.print("Parity: ");             Serial.println(Config.serial.parity);
      Serial.print("Stop Bits: ");          Serial.println(Config.serial.stopBits);
    }
    if (showModbusSettings) {
      Serial.println("\n=== Modbus Settings ===");
      Serial.print("TCP Client: ");         Serial.println(Config.modbus.tcpClient ? "true" : "false");
      Serial.print("Server IP: ");          Serial.println(Config.modbus.server_ip);
      Serial.print("Server Port: ");        Serial.println(Config.modbus.server_port);
      Serial.print("RTU Master: ");         Serial.println(Config.modbus.rtuMaster ? "true" : "false");
      Serial.print("RTU Address: ");        Serial.println(Config.modbus.rtuAddress);
    }
    if (showCanbusSettings) {
      Serial.println("\n=== CAN Bus Settings ===");
      Serial.print("CAN Bus Master: ");     Serial.println(Config.canbus.canbusMaster ? "true" : "false");
      Serial.print("CAN Bus ID: ");         Serial.println(Config.canbus.canbusId);
      Serial.print("CAN Bus Baud: ");       Serial.println(Config.canbus.canbusBaud);
    }
    Serial.println("====================\n");
#endif
  }

  uint8_t statusLedInactiveLevel() const {
    return settings_.statusLedActiveLevel == HIGH ? LOW : HIGH;
  }

  ESP32WebConfigAppSettings settings_;
#if ESP32WC_FEATURE_CONFIG_BUTTON
  Button configButton_;
#endif
#if ESP32WC_FEATURE_STATUS_LED
  bool statusLedState_ = false;
  unsigned long lastStatusLedToggleMs_ = 0;
#endif
  unsigned long lastPrintConfigMs_ = 0;
};
