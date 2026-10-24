#pragma once
#include <WebServer.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

#ifndef ESP32WC_FEATURE_CONFIG_BUTTON
#define ESP32WC_FEATURE_CONFIG_BUTTON 1
#endif

#ifndef ESP32WC_FEATURE_STATUS_LED
#define ESP32WC_FEATURE_STATUS_LED 1
#endif

#ifndef ESP32WC_FEATURE_SERIAL_LOG
#define ESP32WC_FEATURE_SERIAL_LOG 1
#endif

#ifndef ESP32WC_FEATURE_PRINT_CONFIG
#define ESP32WC_FEATURE_PRINT_CONFIG 1
#endif

#ifndef ESP32WC_FEATURE_WIFI_SCAN
#define ESP32WC_FEATURE_WIFI_SCAN 1
#endif

#ifndef ESP32WC_FEATURE_OTA
#define ESP32WC_FEATURE_OTA 1
#endif

#ifndef ESP32WC_FEATURE_SPIFFS_UPDATE
#define ESP32WC_FEATURE_SPIFFS_UPDATE 1
#endif

#ifndef ESP32WC_FEATURE_FILE_MANAGER
#define ESP32WC_FEATURE_FILE_MANAGER 1
#endif

#ifndef ESP32WC_SECTION_DEVICE_INFO
#define ESP32WC_SECTION_DEVICE_INFO 1
#endif

#ifndef ESP32WC_SECTION_MQTT
#define ESP32WC_SECTION_MQTT 1
#endif

#ifndef ESP32WC_SECTION_LAN
#define ESP32WC_SECTION_LAN 0
#endif

#ifndef ESP32WC_SECTION_WLAN
#define ESP32WC_SECTION_WLAN 1
#endif

#ifndef ESP32WC_SECTION_SERIAL
#define ESP32WC_SECTION_SERIAL 1
#endif

#ifndef ESP32WC_SECTION_MODBUS
#define ESP32WC_SECTION_MODBUS 1
#endif

#ifndef ESP32WC_SECTION_CANBUS
#define ESP32WC_SECTION_CANBUS 1
#endif

// ===== External Declarations =====
extern const char* USERNAME;
extern const char* PASSWORD;
extern const char* devicename;
extern const char* deviceversion;

// ===== Configuration Structures =====
struct MQTTConfig {
  char server[40] = "";
  int port = 1883;
  char clientId[40] = "";
  char user[20] = "";
  char password[20] = "";
  char publishTopic[40] = "";
  char subscribeTopic[40] = "";
  int samplingtime = 0;
  bool enableMQTT = false;
};

struct MySerialConfig {
  int baud = 9600;
  int dataBits = 8;
  char parity[8] = "none";
  int stopBits = 1;
};

struct ModbusConfig {
  bool tcpClient = false;
  char server_ip[20] = "";
  int server_port = 502;
  bool rtuMaster = false;
  int rtuAddress = 1;
};

struct DeviceInfo {
  char deviceName[32] = "";
  char firmwareVersion[16] = "";
};

struct NetworkConfig {
  String mac;
  String ip;
  String subnet;
  String gateway;
  String dns;
  bool dhcp = true;
};

struct WLANConfig {
  String mac;
  String ssid;
  String password;
  String ip;
  String subnet;
  String gateway;
  String dns;
  bool dhcp = true;
};

struct CanbusConfig {
  bool canbusMaster = false;
  String canbusId;
  String canbusBaud;
};

struct Configsettings {
  DeviceInfo device;
  MQTTConfig mqtt;
  NetworkConfig lan;
  WLANConfig wlan;
  MySerialConfig serial;
  ModbusConfig modbus;
  CanbusConfig canbus;
};

// ===== Global Variables =====
extern Configsettings Config;
extern WebServer server;
extern String myActiveToken;
extern bool configPortalEnabled;
extern bool showDeviceInfo;
extern bool showMQTTConfig;
extern bool showLanSettings;
extern bool showWlanSettings;
extern bool showSerialSettings;
extern bool showModbusSettings;
extern bool showCanbusSettings;

// ===== Handler Functions (Declarations) =====
void handleRoot();
void handleLoginForm();
void handleLogin();
void handleLogout();
void handleLoad();
void handleNetworkStatus();
void handleSave();
void handleUpdate();
void handleUpdateForm();
void handleSpiffsUpdateForm();
void handleSpiffsUpdate();
void handleFileManager();
void handleFileList();
void handleDownloadFile();
void handleDeleteFile();
void handleFileUploadPage();
void handleFileUpload();
void handleScanWiFi();
void handleCheckToken();

// ===== Settings Management (Declarations) =====
void saveSettings(bool restart = true);
void loadSettings();

// Note: The implementation of these handler functions should be included 
// in your main sketch file (.ino) or in a separate implementation file.
// See the examples/esp32webconfig/esp32webconfig.ino for reference implementations.
