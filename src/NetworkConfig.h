#pragma once

#include <WiFi.h>
#include <ETH.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "ESP32WebConfig.h"

inline WiFiClient wifiClient;
inline NetworkClient ethClient;
inline PubSubClient mqtt;

inline bool eth_connect = false;
inline bool ethStarted = false;
inline bool ethHasIp = false;

// Select the Ethernet controller before including ESP32WebConfigApp.h.
#define ESP32WC_ETHERNET_LAN8720 1
#define ESP32WC_ETHERNET_W5500   2

#ifndef ESP32WC_ETHERNET_TYPE
#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_LAN8720
#endif

#if ESP32WC_ETHERNET_TYPE != ESP32WC_ETHERNET_LAN8720 && \
    ESP32WC_ETHERNET_TYPE != ESP32WC_ETHERNET_W5500
#error "ESP32WC_ETHERNET_TYPE must be ESP32WC_ETHERNET_LAN8720 or ESP32WC_ETHERNET_W5500"
#endif

// LAN8720 RMII defaults. Override these macros before including
// ESP32WebConfigApp.h when your board uses a different LAN8720 pinout.
#ifndef ESP32WC_ETH_PHY_TYPE
#define ESP32WC_ETH_PHY_TYPE ETH_PHY_LAN8720
#endif

#ifndef ESP32WC_ETH_PHY_ADDR
#define ESP32WC_ETH_PHY_ADDR 0
#endif

#ifndef ESP32WC_ETH_MDC_PIN
#define ESP32WC_ETH_MDC_PIN 23
#endif

#ifndef ESP32WC_ETH_MDIO_PIN
#define ESP32WC_ETH_MDIO_PIN 18
#endif

#ifndef ESP32WC_ETH_POWER_PIN
#define ESP32WC_ETH_POWER_PIN -1
#endif

#ifndef ESP32WC_ETH_CLOCK_MODE
#define ESP32WC_ETH_CLOCK_MODE ETH_CLOCK_GPIO0_IN
#endif

// W5500 SPI defaults for a typical ESP32 DevKit. Override these macros before
// including ESP32WebConfigApp.h to match your wiring.
#ifndef ESP32WC_W5500_PHY_ADDR
#define ESP32WC_W5500_PHY_ADDR 1
#endif

#ifndef ESP32WC_W5500_SCK_PIN
#define ESP32WC_W5500_SCK_PIN 18
#endif

#ifndef ESP32WC_W5500_MISO_PIN
#define ESP32WC_W5500_MISO_PIN 19
#endif

#ifndef ESP32WC_W5500_MOSI_PIN
#define ESP32WC_W5500_MOSI_PIN 23
#endif

#ifndef ESP32WC_W5500_CS_PIN
#define ESP32WC_W5500_CS_PIN 5
#endif

#ifndef ESP32WC_W5500_INT_PIN
#define ESP32WC_W5500_INT_PIN -1
#endif

#ifndef ESP32WC_W5500_RST_PIN
#define ESP32WC_W5500_RST_PIN -1
#endif

#ifndef ESP32WC_W5500_SPI_FREQ_MHZ
#define ESP32WC_W5500_SPI_FREQ_MHZ 20
#endif

inline bool ethernetFeatureEnabled() {
  return showLanSettings;
}

inline void onNetworkEvent(arduino_event_id_t event, arduino_event_info_t info) {
  (void)info;

  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      ETH.setHostname("esp32webconfig");
      ethHasIp = false;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      ethHasIp = true;
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
    case ARDUINO_EVENT_ETH_DISCONNECTED:
    case ARDUINO_EVENT_ETH_STOP:
      ethHasIp = false;
      eth_connect = false;
      break;
    default:
      break;
  }
}

inline void stopEthernet() {
  eth_connect = false;
  ethHasIp = false;
  if (ethStarted) {
    ETH.end();
    ethStarted = false;
  }
}

inline void mqttCallback(char* topic, byte* payload, unsigned int length) {
#if ESP32WC_FEATURE_SERIAL_LOG
  String json;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");
  for (unsigned int i = 0; i < length; i++) {
    json += static_cast<char>(payload[i]);
  }
  Serial.println(json);
  Serial.println("=====================================");
#else
  (void)topic;
  (void)payload;
  (void)length;
#endif
}

inline String ipAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + "." +
         String(ipAddress[1]) + "." +
         String(ipAddress[2]) + "." +
         String(ipAddress[3]);
}

inline void initWiFi() {
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

inline bool startEthernet() {
  if (!ethernetFeatureEnabled()) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Ethernet disabled by configuration.");
#endif
    stopEthernet();
    return false;
  }

  if (!ethStarted) {
    // ETH acquires a DHCP address asynchronously. Register this once before
    // starting the driver, just as in ETH_W5500_Arduino_SPI.
    static bool networkEventRegistered = false;
    if (!networkEventRegistered) {
      Network.onEvent(onNetworkEvent);
      networkEventRegistered = true;
    }
#if ESP32WC_ETHERNET_TYPE == ESP32WC_ETHERNET_LAN8720
#if CONFIG_ETH_USE_ESP32_EMAC
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Starting LAN8720 Ethernet...");
#endif
    if (!ETH.begin(
          ESP32WC_ETH_PHY_TYPE,
          ESP32WC_ETH_PHY_ADDR,
          ESP32WC_ETH_MDC_PIN,
          ESP32WC_ETH_MDIO_PIN,
          ESP32WC_ETH_POWER_PIN,
          ESP32WC_ETH_CLOCK_MODE)) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("Failed to start Ethernet PHY.");
#endif
      return false;
    }
#else
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("LAN8720 is not supported by this ESP32 target.");
#endif
    return false;
#endif
#elif ESP32WC_ETHERNET_TYPE == ESP32WC_ETHERNET_W5500
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Starting W5500 Ethernet...");
#endif
    // Match the Arduino-ESP32 ETH_W5500_Arduino_SPI example: initialize
    // Arduino's default SPI bus, then pass that SPI instance to ETH.
    SPI.begin(ESP32WC_W5500_SCK_PIN,
              ESP32WC_W5500_MISO_PIN,
              ESP32WC_W5500_MOSI_PIN);
    if (!ETH.begin(
          ETH_PHY_W5500,
          ESP32WC_W5500_PHY_ADDR,
          ESP32WC_W5500_CS_PIN,
          ESP32WC_W5500_INT_PIN,
          ESP32WC_W5500_RST_PIN,
          SPI,
          ESP32WC_W5500_SPI_FREQ_MHZ)) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("Failed to start W5500 Ethernet.");
#endif
      return false;
    }
#endif
    ethStarted = true;
  }

  if (!Config.lan.dhcp) {
    IPAddress localIP;
    IPAddress defaultGateway;
    IPAddress subnetMask;
    IPAddress primaryDns;
    localIP.fromString(Config.lan.ip.c_str());
    defaultGateway.fromString(Config.lan.gateway.c_str());
    subnetMask.fromString(Config.lan.subnet.c_str());
    primaryDns.fromString(Config.lan.dns.c_str());

    if (!ETH.config(localIP, defaultGateway, subnetMask, primaryDns)) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("Failed to configure static Ethernet IP.");
#endif
      return false;
    }
  }

  // A physical link can come up before DHCP completes. Do not publish or
  // persist a 0.0.0.0 address; the next loop will retry once an IP arrives.
  if (!ETH.hasIP()) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Waiting for Ethernet IP address...");
#endif
    return false;
  }

  Config.lan.ip = ipAddressToString(ETH.localIP());
  Config.lan.gateway = ipAddressToString(ETH.gatewayIP());
  Config.lan.subnet = ipAddressToString(ETH.subnetMask());
  Config.lan.dns = ipAddressToString(ETH.dnsIP());
  Config.lan.mac = ETH.macAddress();

  delay(200);
  saveSettings(false);

  eth_connect = true;
  return true;
}

inline void reconnectMQTT() {
  if (!Config.mqtt.enableMQTT) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("MQTT is disabled in the configuration.");
#endif
    return;
  }

  mqtt.setBufferSize(2048);
  mqtt.setServer(Config.mqtt.server, Config.mqtt.port);
  mqtt.setCallback(mqttCallback);

#if ESP32WC_FEATURE_SERIAL_LOG
  Serial.print("Attempting MQTT connection...");
#endif

  if (mqtt.connect(Config.mqtt.clientId, Config.mqtt.user, Config.mqtt.password)) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("connected");
#endif
    mqtt.subscribe(Config.mqtt.subscribeTopic);
  } else {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.print("failed, rc=");
    Serial.print(mqtt.state());
    Serial.println(" try again in 5 seconds");
#endif
    delay(500);
  }
}

inline void network_switching() {
  static bool lastNetworkIsEth = false;
  static bool mqttUsingEth = false;

  if (!ethernetFeatureEnabled()) {
    if (lastNetworkIsEth || eth_connect) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("[NET] Ethernet disabled, switching to WiFi only...");
#endif
      stopEthernet();
    }

    lastNetworkIsEth = false;
    mqttUsingEth = false;

    if (WiFi.status() != WL_CONNECTED) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("[NET] Trying to connect WiFi...");
#endif
      initWiFi();
    }

    if (WiFi.status() == WL_CONNECTED) {
      if (!mqtt.connected()) {
        mqtt.setClient(wifiClient);
        reconnectMQTT();
      }
      mqtt.loop();
    }
    return;
  }

  const bool ethAvailable = ethStarted && ETH.hasIP();
  const bool wifiAvailable = (WiFi.status() == WL_CONNECTED);

  if (ethAvailable) {
    if (!lastNetworkIsEth) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("[NET] Switching to Ethernet...");
#endif
      WiFi.disconnect();
      startEthernet();
    }

    eth_connect = true;
    lastNetworkIsEth = true;

    if (!mqtt.connected() || !mqttUsingEth) {
      mqtt.setClient(ethClient);
      reconnectMQTT();
      mqttUsingEth = true;
    }

    mqtt.loop();
    return;
  }

  if (wifiAvailable) {
    if (lastNetworkIsEth) {
#if ESP32WC_FEATURE_SERIAL_LOG
      Serial.println("[NET] Switching to WiFi...");
#endif
    }

    eth_connect = false;
    lastNetworkIsEth = false;

    if (!mqtt.connected() || mqttUsingEth) {
      mqtt.setClient(wifiClient);
      reconnectMQTT();
      mqttUsingEth = false;
    }

    mqtt.loop();
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("[NET] Trying to connect WiFi...");
#endif
    initWiFi();
  }

  lastNetworkIsEth = false;
  eth_connect = false;

#if ESP32WC_FEATURE_SERIAL_LOG
  Serial.println("[NET] No network connection");
#endif
  delay(500);
}
