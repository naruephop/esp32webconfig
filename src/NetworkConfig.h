#pragma once

#include <WiFi.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <esp_mac.h>
#include "ESP32WebConfig.h"

inline WiFiClient wifiClient;
inline EthernetClient ethClient;
inline PubSubClient mqtt;

inline uint8_t ETHMac[6];
inline bool eth_connect = false;

#ifndef ESP32WC_W5500_CS_PIN
#define ESP32WC_W5500_CS_PIN 15
#endif

#ifndef ESP32WC_W5500_RST_PIN
#define ESP32WC_W5500_RST_PIN 27
#endif

inline bool ethernetFeatureEnabled() {
  return showLanSettings;
}

inline void stopEthernet() {
  eth_connect = false;
  Ethernet.init(ESP32WC_W5500_CS_PIN);
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

inline void wizReset() {
  pinMode(ESP32WC_W5500_RST_PIN, OUTPUT);
  digitalWrite(ESP32WC_W5500_RST_PIN, HIGH);
  delay(250);
  digitalWrite(ESP32WC_W5500_RST_PIN, LOW);
  delay(50);
  digitalWrite(ESP32WC_W5500_RST_PIN, HIGH);
  delay(350);
}

inline bool startEthernet() {
  if (!ethernetFeatureEnabled()) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Ethernet disabled by configuration.");
#endif
    stopEthernet();
    return false;
  }

  wizReset();
  Ethernet.init(ESP32WC_W5500_CS_PIN);

#if ESP32WC_FEATURE_SERIAL_LOG
  Serial.println("Starting Ethernet...");
#endif

  esp_efuse_mac_get_default(ETHMac);

  if (Config.lan.dhcp) {
    Ethernet.begin(ETHMac, 10000);
  } else {
    IPAddress localIP;
    IPAddress defaultGateway;
    IPAddress subnetMask;
    IPAddress primaryDns;
    localIP.fromString(Config.lan.ip.c_str());
    defaultGateway.fromString(Config.lan.gateway.c_str());
    subnetMask.fromString(Config.lan.subnet.c_str());
    primaryDns.fromString(Config.lan.dns.c_str());
    Ethernet.begin(ETHMac, localIP, primaryDns, defaultGateway, subnetMask);
  }

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Ethernet shield was not found.");
#endif
    return false;
  }

  if (Ethernet.linkStatus() == LinkOFF) {
#if ESP32WC_FEATURE_SERIAL_LOG
    Serial.println("Ethernet cable is not connected.");
#endif
    return false;
  }

  char macStr[18] = {0};
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           ETHMac[0], ETHMac[1], ETHMac[2], ETHMac[3], ETHMac[4], ETHMac[5]);

  Config.lan.ip = ipAddressToString(Ethernet.localIP());
  Config.lan.gateway = ipAddressToString(Ethernet.gatewayIP());
  Config.lan.subnet = ipAddressToString(Ethernet.subnetMask());
  Config.lan.dns = ipAddressToString(Ethernet.dnsServerIP());
  Config.lan.mac = String(macStr);

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

  const bool ethAvailable = (Ethernet.linkStatus() == LinkON);
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
