#pragma once
#include "WebHandlers.h"

// ===== AUTHENTICATION & LOGIN =====
#include <ETH.h>

void handleLoginForm(){
  if (!requireConfigPortalPage()) return;
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  server.send(200,"text/html", R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Control Center</title>
<style>
:root {
  --bg: #f3efe6;
  --panel: rgba(255,255,255,0.9);
  --panel-strong: #ffffff;
  --text: #1d2935;
  --muted: #667585;
  --line: rgba(29,41,53,0.12);
  --primary: #14532d;
  --primary-strong: #0f3f23;
  --accent: #c98b2c;
  --danger: #b42318;
  --shadow: 0 24px 80px rgba(24,39,52,0.16);
  --radius: 24px;
}
* { box-sizing: border-box; }
body {
  margin: 0;
  min-height: 100vh;
  font-family: "Segoe UI", Arial, sans-serif;
  color: var(--text);
  background:
    radial-gradient(circle at top left, rgba(201,139,44,0.18), transparent 28%),
    radial-gradient(circle at bottom right, rgba(20,83,45,0.14), transparent 30%),
    linear-gradient(135deg, #f7f2ea 0%, #eef3f0 100%);
  display: grid;
  place-items: center;
  padding: 24px;
}
.shell {
  width: 100%;
  max-width: 980px;
  display: grid;
  grid-template-columns: 1.15fr 0.85fr;
  border-radius: 32px;
  overflow: hidden;
  box-shadow: var(--shadow);
  background: var(--panel);
  border: 1px solid rgba(255,255,255,0.4);
  backdrop-filter: blur(10px);
}
.hero {
  padding: 48px;
  background:
    linear-gradient(160deg, rgba(20,83,45,0.97), rgba(15,63,35,0.9)),
    linear-gradient(135deg, #14532d, #0f3f23);
  color: #f6f7f5;
  position: relative;
}
.hero:before,
.hero:after {
  content: "";
  position: absolute;
  border-radius: 999px;
  background: rgba(255,255,255,0.08);
}
.hero:before {
  width: 240px;
  height: 240px;
  top: -80px;
  right: -40px;
}
.hero:after {
  width: 160px;
  height: 160px;
  left: -30px;
  bottom: -40px;
}
.eyebrow {
  display: inline-flex;
  align-items: center;
  gap: 10px;
  padding: 8px 14px;
  border-radius: 999px;
  background: rgba(255,255,255,0.12);
  border: 1px solid rgba(255,255,255,0.14);
  font-size: 13px;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}
.hero h1 {
  margin: 24px 0 12px;
  font-size: clamp(32px, 5vw, 52px);
  line-height: 1.02;
}
.hero p {
  margin: 0;
  max-width: 460px;
  color: rgba(246,247,245,0.82);
  font-size: 16px;
  line-height: 1.7;
}
.hero-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 14px;
  margin-top: 28px;
}
.metric {
  padding: 16px;
  border-radius: 18px;
  background: rgba(255,255,255,0.08);
  border: 1px solid rgba(255,255,255,0.12);
}
.metric strong {
  display: block;
  font-size: 22px;
  margin-bottom: 4px;
}
.metric span {
  color: rgba(246,247,245,0.74);
  font-size: 13px;
}
.panel {
  padding: 40px 34px;
  background: rgba(255,255,255,0.86);
}
.panel h2 {
  margin: 0 0 8px;
  font-size: 28px;
}
.panel p {
  margin: 0 0 28px;
  color: var(--muted);
  line-height: 1.6;
}
.field {
  margin-bottom: 16px;
}
.field label {
  display: block;
  margin-bottom: 8px;
  font-weight: 600;
  font-size: 14px;
}
.field input {
  width: 100%;
  padding: 14px 16px;
  border-radius: 16px;
  border: 1px solid var(--line);
  background: rgba(255,255,255,0.96);
  font-size: 15px;
  color: var(--text);
  transition: border-color 0.2s ease, box-shadow 0.2s ease;
}
.field input:focus {
  outline: none;
  border-color: rgba(20,83,45,0.48);
  box-shadow: 0 0 0 4px rgba(20,83,45,0.12);
}
button {
  width: 100%;
  border: none;
  border-radius: 16px;
  padding: 15px 18px;
  background: linear-gradient(135deg, var(--primary), #1c6b3a);
  color: #fff;
  font-size: 15px;
  font-weight: 700;
  cursor: pointer;
  transition: transform 0.18s ease, box-shadow 0.18s ease, background 0.18s ease;
  box-shadow: 0 16px 32px rgba(20,83,45,0.22);
}
button:hover {
  transform: translateY(-1px);
  background: linear-gradient(135deg, var(--primary-strong), var(--primary));
}
button:disabled {
  cursor: wait;
  opacity: 0.7;
}
.login-error,
.status {
  margin-top: 14px;
  padding: 12px 14px;
  border-radius: 14px;
  font-size: 14px;
  display: none;
}
.login-error {
  color: var(--danger);
  background: rgba(180,35,24,0.08);
  border: 1px solid rgba(180,35,24,0.18);
}
.status {
  color: var(--primary-strong);
  background: rgba(20,83,45,0.08);
  border: 1px solid rgba(20,83,45,0.14);
}
.footer-note {
  margin-top: 18px;
  color: var(--muted);
  font-size: 13px;
  line-height: 1.6;
}
@media (max-width: 860px) {
  .shell {
    grid-template-columns: 1fr;
  }
  .hero,
  .panel {
    padding: 32px 24px;
  }
  .hero-grid {
    grid-template-columns: 1fr;
  }
}
</style>
</head>
<body>
<div class="shell">
  <section class="hero">
    <div class="eyebrow">Embedded Device Management</div>
    <h1>ESP32 Control Center</h1>
    <p>Secure access to your on-device configuration portal for WiFi, MQTT, serial communication, industrial protocols, and maintenance tools.</p>
    <div class="hero-grid">
      <div class="metric">
        <strong>WiFi</strong>
        <span>Provision network settings from one place</span>
      </div>
      <div class="metric">
        <strong>OTA</strong>
        <span>Update firmware and filesystem in-browser</span>
      </div>
      <div class="metric">
        <strong>I/O</strong>
        <span>Manage MQTT, Modbus, CAN Bus, and Serial</span>
      </div>
    </div>
  </section>
  <section class="panel">
    <h2>Sign in</h2>
    <p>Use the device credentials configured in your firmware settings to continue.</p>
    <form id="loginForm" onsubmit="return loginSubmit(event)">
      <div class="field">
        <label for="loginUser">Username</label>
        <input type="text" id="loginUser" autocomplete="username" required>
      </div>
      <div class="field">
        <label for="loginPass">Password</label>
        <input type="password" id="loginPass" autocomplete="current-password" required>
      </div>
      <button id="loginButton" type="submit">Open Control Center</button>
      <div id="loginStatus" class="status">Checking credentials...</div>
      <div id="loginError" class="login-error">Invalid username or password. Please try again.</div>
    </form>
    <div class="footer-note">
      This page is served directly from the ESP32 device. Session access is stored in the current browser only.
    </div>
  </section>
</div>
<script>
function setBusy(isBusy) {
  document.getElementById('loginButton').disabled = isBusy;
  document.getElementById('loginStatus').style.display = isBusy ? 'block' : 'none';
}

async function loginSubmit(e) {
  e.preventDefault();
  document.getElementById('loginError').style.display = 'none';
  setBusy(true);
  const data = {
    user: document.getElementById('loginUser').value,
    pass: document.getElementById('loginPass').value
  };
  try {
    const response = await fetch('/login', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify(data)
    });
    const result = await response.json();
    if (result.success && result.token) {
      sessionStorage.setItem('esp32_token', result.token);
      window.location.href = '/';
      return false;
    }
  } catch (error) {
  }
  document.getElementById('loginError').style.display = 'block';
  setBusy(false);
  return false;
}

async function validateExistingToken() {
  const token = sessionStorage.getItem('esp32_token');
  if (!token) return;
  setBusy(true);
  try {
    const response = await fetch('/check_token', {
      headers: {'Authorization': 'Bearer ' + token}
    });
    if (response.ok) {
      window.location.href = '/';
      return;
    }
  } catch (error) {
  }
  sessionStorage.removeItem('esp32_token');
  setBusy(false);
}

validateExistingToken();
</script>
</body>
</html>)rawliteral");
}

void handleLogin() {
  if (!requireConfigPortalJson()) return;
  String body = server.arg("plain");
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);
  StaticJsonDocument<128> res;
  String resp;
  
  if (!error) {
    String user = doc["user"] | "";
    String pass = doc["pass"] | "";
    if (user == USERNAME && pass == PASSWORD) {
      String token = genToken(32);
      res["success"] = true;
      res["token"] = token;
      myActiveToken = token;
      saveSettings(false);
    } else {
      res["success"] = false;
    }
  } else {
    res["success"] = false;
  }
  serializeJson(res, resp);
  server.send(200, "application/json", resp);
}

void handleLogout() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;
  myActiveToken = "";
  saveSettings(false);
  server.send(200, "application/json", R"({"success": true})");
}

// ===== SETTINGS MANAGEMENT =====
void handleLoad() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;
  
  DynamicJsonDocument doc(2048);
  doc["showDeviceInfo"] = showDeviceInfo;
  doc["showMQTT"] = showMQTTConfig;
  doc["showLAN"] = showLanSettings;
  doc["showWLAN"] = showWlanSettings;
  doc["showSerial"] = showSerialSettings;
  doc["showModbus"] = showModbusSettings;
  doc["showCanBus"] = showCanbusSettings;
  doc["myActiveToken"] = myActiveToken;
  doc["configPortalEnabled"] = configPortalEnabled;
  
#if ESP32WC_SECTION_DEVICE_INFO
  doc["deviceName"] = Config.device.deviceName;
  doc["firmwareVersion"] = Config.device.firmwareVersion;
#endif
#if ESP32WC_SECTION_MQTT
  doc["enableMQTT"] = Config.mqtt.enableMQTT;
  doc["server"] = Config.mqtt.server;
  doc["port"] = Config.mqtt.port;
  doc["clientId"] = Config.mqtt.clientId;
  doc["user"] = Config.mqtt.user;
  doc["password"] = Config.mqtt.password;
  doc["publishTopic"] = Config.mqtt.publishTopic;
  doc["subscribeTopic"] = Config.mqtt.subscribeTopic;
  doc["samplingtime"] = Config.mqtt.samplingtime;
#endif
#if ESP32WC_SECTION_LAN
  doc["lanType"] = Config.lan.dhcp;
  doc["lanMac"] = Config.lan.mac;
  doc["lanIp"] = Config.lan.ip;
  doc["lanSubnet"] = Config.lan.subnet;
  doc["lanGateway"] = Config.lan.gateway;
  doc["lanDns"] = Config.lan.dns;
#endif
#if ESP32WC_SECTION_WLAN
  doc["wlanType"] = Config.wlan.dhcp;
  doc["wlanMac"] = Config.wlan.mac;
  doc["wlanSsid"] = Config.wlan.ssid;
  doc["wlanPassword"] = Config.wlan.password;
  doc["wlanIp"] = Config.wlan.ip;
  doc["wlanSubnet"] = Config.wlan.subnet;
  doc["wlanGateway"] = Config.wlan.gateway;
  doc["wlanDns"] = Config.wlan.dns;
#endif
#if ESP32WC_SECTION_SERIAL
  doc["serialBaud"] = Config.serial.baud;
  doc["serialDataBits"] = Config.serial.dataBits;
  doc["serialParity"] = Config.serial.parity;
  doc["serialStopBits"] = Config.serial.stopBits;
#endif
#if ESP32WC_SECTION_MODBUS
  doc["modbusTcp"] = Config.modbus.tcpClient;
  doc["modbusRtu"] = Config.modbus.rtuMaster;
  doc["rtuAddress"] = Config.modbus.rtuAddress;
  doc["server_ip"] = Config.modbus.server_ip;
  doc["server_port"] = Config.modbus.server_port;
#endif
#if ESP32WC_SECTION_CANBUS
  doc["canbusMaster"] = Config.canbus.canbusMaster;
  doc["canbusId"] = Config.canbus.canbusId;
  doc["canbusBaud"] = Config.canbus.canbusBaud;
#endif
  
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleNetworkStatus() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;

  DynamicJsonDocument doc(384);
  const bool lanConnected = ETH.hasIP();
  doc["lanConnected"] = lanConnected;
  doc["lanIp"] = lanConnected ? ETH.localIP().toString() : "";
  doc["lanSubnet"] = lanConnected ? ETH.subnetMask().toString() : "";
  doc["lanGateway"] = lanConnected ? ETH.gatewayIP().toString() : "";
  doc["lanDns"] = lanConnected ? ETH.dnsIP().toString() : "";

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleSave() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;
  
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }
  
  DynamicJsonDocument doc(2048);
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }
  
#if ESP32WC_SECTION_MQTT
  Config.mqtt.enableMQTT = doc["enableMQTT"] | false;
  strlcpy(Config.mqtt.server, doc["server"] | "", sizeof(Config.mqtt.server));
  Config.mqtt.port = doc["port"] | 1883;
  strlcpy(Config.mqtt.clientId, doc["clientId"] | "", sizeof(Config.mqtt.clientId));
  strlcpy(Config.mqtt.user, doc["user"] | "", sizeof(Config.mqtt.user));
  strlcpy(Config.mqtt.password, doc["password"] | "", sizeof(Config.mqtt.password));
  strlcpy(Config.mqtt.publishTopic, doc["publishTopic"] | "", sizeof(Config.mqtt.publishTopic));
  strlcpy(Config.mqtt.subscribeTopic, doc["subscribeTopic"] | "", sizeof(Config.mqtt.subscribeTopic));
  Config.mqtt.samplingtime = doc["samplingtime"] | 0;
#endif
#if ESP32WC_SECTION_LAN
  Config.lan.dhcp = doc["lanType"] | true;
  Config.lan.mac = doc["lanMac"] | "";
  Config.lan.ip = doc["lanIp"] | "";
  Config.lan.subnet = doc["lanSubnet"] | "";
  Config.lan.gateway = doc["lanGateway"] | "";
  Config.lan.dns = doc["lanDns"] | "";
#endif
#if ESP32WC_SECTION_WLAN
  Config.wlan.dhcp = doc["wlanType"] | true;
  Config.wlan.mac = doc["wlanMac"] | "";
  Config.wlan.ssid = doc["wlanSsid"] | "";
  Config.wlan.password = doc["wlanPassword"] | "";
  Config.wlan.ip = doc["wlanIp"] | "";
  Config.wlan.subnet = doc["wlanSubnet"] | "";
  Config.wlan.gateway = doc["wlanGateway"] | "";
  Config.wlan.dns = doc["wlanDns"] | "";
#endif
#if ESP32WC_SECTION_SERIAL
  Config.serial.baud = doc["serialBaud"].as<int>();
    if (Config.serial.baud == 0) Config.serial.baud = 9600;
    Config.serial.dataBits = doc["serialDataBits"].as<int>();
    if (Config.serial.dataBits == 0) Config.serial.dataBits = 8;
    strlcpy(Config.serial.parity, doc["serialParity"] | "none", sizeof(Config.serial.parity));
    Config.serial.stopBits = doc["serialStopBits"].as<int>();
    if (Config.serial.stopBits == 0) Config.serial.stopBits = 1;
#endif
#if ESP32WC_SECTION_MODBUS
  Config.modbus.tcpClient = doc["modbusTcp"] | false;
    Config.modbus.rtuMaster = doc["modbusRtu"] | false;
    Config.modbus.rtuAddress = doc["rtuAddress"].as<int>();
    if (Config.modbus.rtuAddress <= 0) Config.modbus.rtuAddress = 1;
    strlcpy(Config.modbus.server_ip, doc["server_ip"] | "", sizeof(Config.modbus.server_ip));
    Config.modbus.server_port = doc["server_port"].as<int>() | 502;
#endif
#if ESP32WC_SECTION_CANBUS
  Config.canbus.canbusMaster = doc["canbusMaster"] | false;
  Config.canbus.canbusId = doc["canbusId"] | "";
  Config.canbus.canbusBaud = String((const char*)(doc["canbusBaud"] | "100"));
#endif
  
  saveSettings(false);
  server.send(200, "text/plain", "Settings saved! Restarting...");
  delay(200);
  ESP.restart();
}

void saveSettings(bool restart) {
  DynamicJsonDocument doc(2048);
#if ESP32WC_SECTION_DEVICE_INFO
  doc["deviceName"] = Config.device.deviceName;
  doc["firmwareVersion"] = Config.device.firmwareVersion;
#endif
#if ESP32WC_SECTION_MQTT
  doc["enableMQTT"] = Config.mqtt.enableMQTT;
  doc["server"] = Config.mqtt.server;
  doc["port"] = Config.mqtt.port;
  doc["clientId"] = Config.mqtt.clientId;
  doc["user"] = Config.mqtt.user;
  doc["password"] = Config.mqtt.password;
  doc["publishTopic"] = Config.mqtt.publishTopic;
  doc["subscribeTopic"] = Config.mqtt.subscribeTopic;
  doc["samplingtime"] = Config.mqtt.samplingtime;
#endif
#if ESP32WC_SECTION_LAN
  doc["lanType"] = Config.lan.dhcp;
  doc["lanMac"] = Config.lan.mac;
  doc["lanIp"] = Config.lan.ip;
  doc["lanSubnet"] = Config.lan.subnet;
  doc["lanGateway"] = Config.lan.gateway;
  doc["lanDns"] = Config.lan.dns;
#endif
#if ESP32WC_SECTION_WLAN
  doc["wlanType"] = Config.wlan.dhcp;
  doc["wlanMac"] = Config.wlan.mac;
  doc["wlanSsid"] = Config.wlan.ssid;
  doc["wlanPassword"] = Config.wlan.password;
  doc["wlanIp"] = Config.wlan.ip;
  doc["wlanSubnet"] = Config.wlan.subnet;
  doc["wlanGateway"] = Config.wlan.gateway;
  doc["wlanDns"] = Config.wlan.dns;
#endif
#if ESP32WC_SECTION_SERIAL
  doc["serialBaud"] = Config.serial.baud;
  doc["serialDataBits"] = Config.serial.dataBits;
  doc["serialParity"] = Config.serial.parity;
  doc["serialStopBits"] = Config.serial.stopBits;
#endif
#if ESP32WC_SECTION_MODBUS
  doc["modbusTcp"] = Config.modbus.tcpClient;
  doc["modbusRtu"] = Config.modbus.rtuMaster;
  doc["rtuAddress"] = Config.modbus.rtuAddress;
  doc["server_ip"] = Config.modbus.server_ip;
  doc["server_port"] = Config.modbus.server_port;
#endif
#if ESP32WC_SECTION_CANBUS
  doc["canbusMaster"] = Config.canbus.canbusMaster;
  doc["canbusId"] = Config.canbus.canbusId;
  doc["canbusBaud"] = Config.canbus.canbusBaud;
#endif
  doc["myActiveToken"] = myActiveToken;
  doc["configPortalEnabled"] = configPortalEnabled;
  
  File file = SPIFFS.open("/config.json", "w");
  if (!file) {
    Serial.println("Failed to open config file");
    return;
  }
  serializeJson(doc, file);
  file.close();
  Serial.println("Configuration saved");
  if (restart) {
    delay(1000);
    ESP.restart();
  }
}

void loadSettings() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }
  
  strlcpy(Config.device.deviceName, devicename, sizeof(Config.device.deviceName));
  strlcpy(Config.device.firmwareVersion, deviceversion, sizeof(Config.device.firmwareVersion));
  configPortalEnabled = true;
  
  File file = SPIFFS.open("/config.json", "r");
  if (!file) {
    Serial.println("No configuration file found");
    return;
  }
  
  if (file.size() > 2048) {
    Serial.println("Configuration file too large");
    return;
  }
  
  DynamicJsonDocument doc(2048);
  if (deserializeJson(doc, file)) {
    Serial.println("Failed to parse config");
    return;
  }
  
#if ESP32WC_SECTION_MQTT
  Config.mqtt.enableMQTT = doc["enableMQTT"] | false;
  strlcpy(Config.mqtt.server, doc["server"] | "", sizeof(Config.mqtt.server));
  Config.mqtt.port = doc["port"] | 1883;
  strlcpy(Config.mqtt.clientId, doc["clientId"] | "", sizeof(Config.mqtt.clientId));
  strlcpy(Config.mqtt.user, doc["user"] | "", sizeof(Config.mqtt.user));
  strlcpy(Config.mqtt.password, doc["password"] | "", sizeof(Config.mqtt.password));
  strlcpy(Config.mqtt.publishTopic, doc["publishTopic"] | "", sizeof(Config.mqtt.publishTopic));
  strlcpy(Config.mqtt.subscribeTopic, doc["subscribeTopic"] | "", sizeof(Config.mqtt.subscribeTopic));
  Config.mqtt.samplingtime = doc["samplingtime"] | 0;
#endif
#if ESP32WC_SECTION_LAN
  Config.lan.dhcp = doc["lanType"] | true;
  Config.lan.mac = doc["lanMac"] | "";
  Config.lan.ip = doc["lanIp"] | "";
  Config.lan.subnet = doc["lanSubnet"] | "";
  Config.lan.gateway = doc["lanGateway"] | "";
  Config.lan.dns = doc["lanDns"] | "";
#endif
#if ESP32WC_SECTION_WLAN
  Config.wlan.dhcp = doc["wlanType"] | true;
  Config.wlan.mac = doc["wlanMac"] | "";
  Config.wlan.ssid = doc["wlanSsid"] | "";
  Config.wlan.password = doc["wlanPassword"] | "";
  Config.wlan.ip = doc["wlanIp"] | "";
  Config.wlan.subnet = doc["wlanSubnet"] | "";
  Config.wlan.gateway = doc["wlanGateway"] | "";
  Config.wlan.dns = doc["wlanDns"] | "";
#endif
#if ESP32WC_SECTION_SERIAL
  Config.serial.baud = doc["serialBaud"] | 9600;
  Config.serial.dataBits = doc["serialDataBits"] | 8;
  strlcpy(Config.serial.parity, doc["serialParity"] | "none", sizeof(Config.serial.parity));
  Config.serial.stopBits = doc["serialStopBits"] | 1;
#endif
#if ESP32WC_SECTION_MODBUS
  Config.modbus.tcpClient = doc["modbusTcp"] | false;
  Config.modbus.rtuMaster = doc["modbusRtu"] | false;
  Config.modbus.rtuAddress = doc["rtuAddress"] | 1;
  if (Config.modbus.rtuAddress <= 0) Config.modbus.rtuAddress = 1;
  strlcpy(Config.modbus.server_ip, doc["server_ip"] | "", sizeof(Config.modbus.server_ip));
  Config.modbus.server_port = doc["server_port"] | 502;
#endif
#if ESP32WC_SECTION_CANBUS
  Config.canbus.canbusMaster = doc["canbusMaster"] | false;
  Config.canbus.canbusId = doc["canbusId"] | "";
  Config.canbus.canbusBaud = String((const char*)(doc["canbusBaud"] | "100"));
#endif
  myActiveToken = doc["myActiveToken"] | "";
  configPortalEnabled = doc["configPortalEnabled"] | true;
  
  file.close();
  Serial.println("Configuration loaded");
}

// ===== OTA UPDATES =====
#if ESP32WC_FEATURE_OTA
void handleUpdateForm() {
  if (!requireConfigPortalPage()) return;
  server.send(200, "text/html", R"rawliteral(<!DOCTYPE html><html><head><title>OTA Update</title><style>body{font-family:Arial;text-align:center;margin:20px;}form{background:#fff;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);display:inline-block;}button{padding:10px 20px;background:#007bff;color:#fff;border:none;border-radius:5px;cursor:pointer;}button:hover{background:#0056b3;}</style></head><body><h2>Firmware Update</h2><form id="updateForm" method="POST" action="/update" enctype="multipart/form-data"><input type="file" name="firmware"><button type="submit">Upload</button></form><script>const token = sessionStorage.getItem("esp32_token"); if (!token) {window.location.href = "/login";} else {document.getElementById("updateForm").action = "/update?token=" + encodeURIComponent(token);}</script></body></html>)rawliteral");
}

void handleUpdate() {
  if (!requireConfigPortalText() || !requireAuthorizationText()) return;
  
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      server.send(200, "text/plain", "Update Success! Rebooting...");
      delay(500);
      ESP.restart();
    } else {
      Update.printError(Serial);
    }
  }
}
#endif

// ===== SPIFFS MANAGEMENT =====
#if ESP32WC_FEATURE_SPIFFS_UPDATE
void handleSpiffsUpdateForm() {
  if (!requireConfigPortalPage()) return;
  server.send(200, "text/html", R"rawliteral(<!DOCTYPE html><html><head><title>SPIFFS Update</title><style>body{font-family:Arial;text-align:center;margin:20px;}form{background:#fff;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);display:inline-block;}button{padding:10px 20px;background:#28a745;color:#fff;border:none;border-radius:5px;cursor:pointer;}button:hover{background:#218838;}</style></head><body><h2>SPIFFS Update</h2><form id="spiffsForm" method="POST" action="/update-spiffs" enctype="multipart/form-data"><input type="file" name="spiffs"><button type="submit">Upload</button></form><script>const token = sessionStorage.getItem("esp32_token"); if (!token) window.location.href = "/login"; else document.getElementById("spiffsForm").action = "/update-spiffs?token=" + encodeURIComponent(token);</script></body></html>)rawliteral");
}

void handleSpiffsUpdate() {
  if (!requireConfigPortalText() || !requireAuthorizationText()) return;
  
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      server.send(200, "text/plain", "SPIFFS Update Success! Rebooting...");
      delay(500);
      ESP.restart();
    } else {
      Update.printError(Serial);
    }
  }
}
#endif

// ===== FILE MANAGEMENT =====
#if ESP32WC_FEATURE_FILE_MANAGER
void handleFileManager() {
  if (!requireConfigPortalPage()) return;
  server.send(200, "text/html", R"rawliteral(<!DOCTYPE html><html><head><title>File Manager</title><style>body{font-family:Arial;margin:20px;}table{margin:20px auto;border-collapse:collapse;width:80%;}th,td{border:1px solid #ddd;padding:8px;text-align:left;}th{background-color:#007bff;color:white;}button{background-color:#dc3545;color:white;border:none;padding:5px 10px;cursor:pointer;}button:hover{background-color:#c82333;}</style></head><body><h2>File Manager</h2><table><thead><tr><th>Filename</th><th>Size</th><th>Actions</th></tr></thead><tbody id="fileTable"></tbody></table><form id="uploadForm" method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="file"><button type="submit">Upload</button></form><script>const token = sessionStorage.getItem("esp32_token"); if (!token) window.location.href = "/login"; else { document.getElementById("uploadForm").action = "/upload?token=" + encodeURIComponent(token); fetchFiles(); }async function fetchFiles() { const response = await fetch('/file-list', {headers: {'Authorization': 'Bearer ' + token}}); if (!response.ok) return; const files = await response.json(); const table = document.getElementById('fileTable'); table.innerHTML = files.map(file => `<tr><td>${file.name}</td><td>${file.size}</td><td><button onclick="deleteFile('${file.name}')">Delete</button></td></tr>`).join(''); }async function deleteFile(filename) { if (!confirm(`Delete ${filename}?`)) return; const response = await fetch(`/delete-file?filename=${encodeURIComponent(filename)}`, {method: 'GET', headers: {'Authorization': 'Bearer ' + token}}); alert(await response.text()); fetchFiles(); }</script></body></html>)rawliteral");
}

void handleFileList() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;
  
  String json = "[";
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    if (json != "[") json += ",";
    json += "{\"name\":\"" + String(file.name()) + "\",\"size\":\"" + formatBytes(file.size()) + "\"}";
    file = root.openNextFile();
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleDeleteFile() {
  if (!requireConfigPortalText() || !requireAuthorizationText()) return;
  
  if (!server.hasArg("filename")) {
    server.send(400, "text/plain", "No filename");
    return;
  }
  
  String filename = "/" + server.arg("filename");
  if (filename.indexOf("..") != -1 || !SPIFFS.exists(filename)) {
    server.send(400, "text/plain", "Invalid filename");
    return;
  }
  
  if (SPIFFS.remove(filename)) {
    server.send(200, "text/plain", "Deleted");
  } else {
    server.send(500, "text/plain", "Delete failed");
  }
}

void handleDownloadFile() {
  if (!requireConfigPortalText() || !requireAuthorizationText()) return;
  
  if (!server.hasArg("filename")) {
    server.send(400, "text/plain", "No filename");
    return;
  }
  
  String filename = "/" + server.arg("filename");
  if (filename.indexOf("..") != -1 || !SPIFFS.exists(filename)) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  
  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    server.send(500, "text/plain", "Open failed");
    return;
  }
  
  server.sendHeader("Content-Disposition", "attachment; filename=\"" + filename.substring(1) + "\"");
  server.streamFile(file, "application/octet-stream");
  file.close();
}

void handleFileUploadPage() {
  if (!requireConfigPortalPage()) return;
  server.send(200, "text/html", R"rawliteral(<!DOCTYPE html><html><head><title>Upload</title><style>body{font-family:Arial;text-align:center;margin:20px;}form{background:#fff;padding:20px;border-radius:10px;display:inline-block;}button{padding:10px 20px;background:#007bff;color:#fff;border:none;border-radius:5px;cursor:pointer;}button:hover{background:#0056b3;}</style></head><body><h2>Upload File</h2><form id="uploadPageForm" method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="file"><button type="submit">Upload</button></form><script>const token = sessionStorage.getItem("esp32_token"); if (!token) window.location.href = "/login"; else document.getElementById("uploadPageForm").action = "/upload?token=" + encodeURIComponent(token);</script></body></html>)rawliteral");
}

void handleFileUpload() {
  if (!requireConfigPortalText() || !requireAuthorizationText()) return;
  
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/" + upload.filename;
    if (!filename.endsWith(".bin") && !filename.endsWith(".json")) {
      server.send(400, "text/plain", "Invalid type");
      return;
    }
    File file = SPIFFS.open(filename, FILE_WRITE);
    if (!file) {
      server.send(500, "text/plain", "Create failed");
      return;
    }
    file.close();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    String filename = "/" + upload.filename;
    File file = SPIFFS.open(filename, FILE_APPEND);
    if (file) {
      file.write(upload.buf, upload.currentSize);
      file.close();
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    server.send(200, "text/plain", "Uploaded");
  }
}
#endif

// ===== WIFI & STATUS =====
#if ESP32WC_FEATURE_WIFI_SCAN
void handleScanWiFi() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;
  
  int n = WiFi.scanNetworks();
  DynamicJsonDocument doc(1024);
  JsonArray ssids = doc.to<JsonArray>();
  
  for (int i = 0; i < n; ++i) {
    ssids.add(WiFi.SSID(i));
  }
  
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}
#endif

void handleCheckToken() {
  if (!requireConfigPortalJson() || !requireAuthorizationJson()) return;
  server.send(200, "application/json", R"({"valid": true})");
}

// ===== ROOT HANDLER - Minimal inline HTML =====
void handleRoot() {
  if (!requireConfigPortalPage()) return;
  server.send(200, "text/html", R"rawliteral(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Control Center</title>
    <style>
      :root {
        --bg: #efe8db;
        --surface: rgba(255,255,255,0.92);
        --surface-strong: #ffffff;
        --surface-muted: #f6f1e8;
        --ink: #1f2933;
        --muted: #6b7785;
        --line: rgba(31,41,51,0.12);
        --primary: #14532d;
        --primary-strong: #0f3f23;
        --primary-soft: rgba(20,83,45,0.08);
        --accent: #c98b2c;
        --danger: #b42318;
        --shadow: 0 20px 60px rgba(25,39,53,0.12);
        --radius-lg: 28px;
        --radius-md: 18px;
        --radius-sm: 14px;
      }
      * {
        box-sizing: border-box;
      }
      body {
        font-family: "Segoe UI", Arial, sans-serif;
        margin: 0;
        min-height: 100vh;
        color: var(--ink);
        background:
          radial-gradient(circle at top left, rgba(201,139,44,0.18), transparent 24%),
          radial-gradient(circle at bottom right, rgba(20,83,45,0.14), transparent 30%),
          linear-gradient(135deg, #f7f3ec 0%, #edf3ef 100%);
      }
      .page {
        max-width: 1320px;
        margin: 0 auto;
        padding: 28px 18px 48px;
      }
      .topbar {
        display: flex;
        justify-content: space-between;
        align-items: center;
        gap: 16px;
        margin-bottom: 22px;
      }
      .brand {
        display: flex;
        align-items: center;
        gap: 14px;
      }
      .brand-mark {
        width: 48px;
        height: 48px;
        border-radius: 16px;
        background: linear-gradient(135deg, var(--primary), #1d6d3d);
        color: #fff;
        display: grid;
        place-items: center;
        font-weight: 700;
        box-shadow: 0 16px 30px rgba(20,83,45,0.24);
      }
      .brand-copy h1 {
        margin: 0;
        font-size: 28px;
      }
      .brand-copy p {
        margin: 4px 0 0;
        color: var(--muted);
      }
      .toolbar {
        display: flex;
        flex-wrap: wrap;
        justify-content: flex-end;
        gap: 10px;
      }
      .hero {
        display: grid;
        grid-template-columns: 1.3fr 0.7fr;
        gap: 18px;
        margin-bottom: 22px;
      }
      .hero-card,
      .status-panel {
        background: var(--surface);
        border: 1px solid rgba(255,255,255,0.52);
        border-radius: var(--radius-lg);
        box-shadow: var(--shadow);
        backdrop-filter: blur(8px);
      }
      .hero-card {
        padding: 28px;
        position: relative;
        overflow: hidden;
      }
      .hero-card:before {
        content: "";
        position: absolute;
        inset: auto -40px -80px auto;
        width: 220px;
        height: 220px;
        border-radius: 999px;
        background: rgba(20,83,45,0.08);
      }
      .eyebrow {
        display: inline-flex;
        align-items: center;
        gap: 10px;
        padding: 8px 14px;
        border-radius: 999px;
        background: var(--surface-muted);
        color: var(--primary);
        font-size: 12px;
        font-weight: 700;
        letter-spacing: 0.08em;
        text-transform: uppercase;
      }
      .hero-card h2 {
        margin: 18px 0 12px;
        font-size: clamp(28px, 4vw, 44px);
        line-height: 1.05;
      }
      .hero-card p {
        max-width: 680px;
        margin: 0;
        line-height: 1.7;
        color: var(--muted);
      }
      .hero-metrics {
        display: grid;
        grid-template-columns: repeat(3, minmax(0, 1fr));
        gap: 12px;
        margin-top: 24px;
      }
      .metric {
        padding: 16px;
        border-radius: var(--radius-md);
        background: var(--surface-muted);
        border: 1px solid rgba(31,41,51,0.08);
      }
      .metric strong {
        display: block;
        font-size: 20px;
        margin-bottom: 4px;
      }
      .metric span {
        color: var(--muted);
        font-size: 13px;
        line-height: 1.5;
      }
      .status-panel {
        padding: 22px;
        display: flex;
        flex-direction: column;
        gap: 14px;
      }
      .status-chip {
        display: inline-flex;
        align-items: center;
        gap: 8px;
        padding: 9px 12px;
        border-radius: 999px;
        background: var(--primary-soft);
        color: var(--primary);
        font-weight: 700;
        font-size: 13px;
      }
      .status-label {
        color: var(--muted);
        font-size: 13px;
        margin-bottom: 4px;
      }
      .status-value {
        font-size: 20px;
        font-weight: 700;
      }
      .status-value.small {
        font-size: 16px;
        word-break: break-word;
      }
      .layout {
        display: grid;
        grid-template-columns: minmax(0, 1fr) 320px;
        gap: 22px;
        align-items: start;
      }
      .main-form {
        display: grid;
        gap: 18px;
      }
      .section-card {
        background: var(--surface);
        border: 1px solid rgba(255,255,255,0.52);
        border-radius: var(--radius-lg);
        box-shadow: var(--shadow);
        overflow: hidden;
      }
      fieldset {
        border: none;
        margin: 0;
        padding: 22px;
      }
      legend {
        padding: 0;
      }
      .section-head {
        display: flex;
        justify-content: space-between;
        align-items: center;
        gap: 12px;
        margin-bottom: 18px;
      }
      .section-title {
        margin: 0;
        font-size: 20px;
        font-weight: 700;
        color: var(--ink);
      }
      .section-subtitle {
        margin: 6px 0 0;
        color: var(--muted);
        font-size: 14px;
      }
      .section-badge {
        display: inline-flex;
        align-items: center;
        gap: 8px;
        padding: 8px 12px;
        border-radius: 999px;
        background: var(--surface-muted);
        color: var(--primary);
        font-size: 12px;
        font-weight: 700;
        text-transform: uppercase;
        letter-spacing: 0.06em;
      }
      select {
        width: 100%;
      }
      input,
      select {
        margin: 0;
        padding: 12px 14px;
        width: 100%;
        border: 1px solid var(--line);
        border-radius: var(--radius-sm);
        background: rgba(255,255,255,0.96);
        color: var(--ink);
        font-size: 14px;
        transition: border-color 0.2s ease, box-shadow 0.2s ease, background 0.2s ease;
      }
      input:focus,
      select:focus {
        outline: none;
        border-color: rgba(20,83,45,0.44);
        box-shadow: 0 0 0 4px rgba(20,83,45,0.11);
      }
      input:disabled,
      select:disabled {
        background: #eef1f3;
        color: #8b96a3;
        cursor: not-allowed;
      }
      button {
        margin: 0;
        padding: 13px 18px;
        background: linear-gradient(135deg, var(--primary), #1a6b3b);
        color: #fff;
        border: none;
        border-radius: 15px;
        cursor: pointer;
        font-size: 14px;
        font-weight: 700;
        transition: transform 0.18s ease, box-shadow 0.18s ease, opacity 0.18s ease;
        box-shadow: 0 14px 28px rgba(20,83,45,0.2);
      }
      button:hover {
        transform: translateY(-1px);
      }
      button:disabled {
        cursor: wait;
        opacity: 0.75;
      }
      .button-secondary {
        background: #fff;
        color: var(--ink);
        border: 1px solid var(--line);
        box-shadow: none;
      }
      .button-danger {
        background: linear-gradient(135deg, #b42318, #d92d20);
      }
      .container {
        display: inline-flex;
        align-items: center;
        gap: 12px;
        position: relative;
        padding-left: 34px;
        min-height: 20px;
        margin: 0;
        cursor: pointer;
        user-select: none;
        font-weight: 600;
      }
      .container input {
        position: absolute;
        opacity: 0;
        cursor: pointer;
        height: 0;
        width: 0;
      }

      .checkmark {
        position: absolute;
        top: 1px;
        left: 0;
        height: 20px;
        width: 20px;
        background-color: #eef1f3;
        border-radius: 6px;
        border: 1px solid rgba(31,41,51,0.12);
      }
      .container:hover input ~ .checkmark {
        background-color: #e4e8eb;
      }
      .container input:checked ~ .checkmark {
        background-color: var(--primary);
        border-color: var(--primary);
      }
      .checkmark:after {
        content: "";
        position: absolute;
        display: none;
      }
      .container input:checked ~ .checkmark:after {
        display: block;
      }
      .container .checkmark:after {
        left: 6px;
        top: 1px;
        width: 4px;
        height: 10px;
        border: solid white;
        border-width: 0 3px 3px 0;
        transform: rotate(45deg);
      }
      table {
        width: 100%;
        border-collapse: separate;
        border-spacing: 0 12px;
      }
      td {
        padding: 0 8px;
        vertical-align: top;
      }
      td:first-child {
        padding-left: 0;
      }
      td:last-child {
        padding-right: 0;
      }
      label {
        display: block;
        margin-bottom: 8px;
        font-size: 13px;
        font-weight: 600;
        color: var(--muted);
      }
      .row-card {
        background: var(--surface-muted);
        border: 1px solid rgba(31,41,51,0.06);
        border-radius: var(--radius-md);
        padding: 14px;
      }
      .toggle-row {
        display: flex;
        flex-wrap: wrap;
        gap: 12px 18px;
        align-items: center;
      }
      .sidebar {
        display: grid;
        gap: 18px;
        position: sticky;
        top: 18px;
      }
      .sidebar-card {
        background: var(--surface);
        border: 1px solid rgba(255,255,255,0.52);
        border-radius: var(--radius-lg);
        box-shadow: var(--shadow);
        padding: 22px;
      }
      .sidebar-card h3 {
        margin: 0 0 10px;
        font-size: 18px;
      }
      .sidebar-card p {
        margin: 0 0 14px;
        line-height: 1.6;
        color: var(--muted);
        font-size: 14px;
      }
      .sidebar-links {
        display: grid;
        gap: 10px;
      }
      .sidebar-links a,
      .resource-link {
        display: flex;
        justify-content: space-between;
        align-items: center;
        gap: 12px;
        padding: 13px 14px;
        border-radius: 15px;
        background: var(--surface-muted);
        border: 1px solid rgba(31,41,51,0.06);
        color: var(--ink);
        text-decoration: none;
        font-weight: 600;
      }
      .save-bar {
        position: sticky;
        bottom: 16px;
        display: flex;
        align-items: center;
        justify-content: space-between;
        gap: 14px;
        margin-top: 10px;
        padding: 16px 18px;
        border-radius: 20px;
        background: rgba(15,63,35,0.94);
        color: #f3f7f4;
        box-shadow: 0 20px 40px rgba(15,63,35,0.2);
      }
      .save-copy strong {
        display: block;
        margin-bottom: 4px;
        font-size: 15px;
      }
      .save-copy span {
        color: rgba(243,247,244,0.78);
        font-size: 13px;
      }
      .save-actions {
        display: flex;
        gap: 10px;
        flex-wrap: wrap;
      }
      .inline-note,
      .banner {
        border-radius: 16px;
        padding: 14px 16px;
        font-size: 14px;
        line-height: 1.6;
      }
      .inline-note {
        background: rgba(201,139,44,0.12);
        border: 1px solid rgba(201,139,44,0.18);
        color: #7a561a;
      }
      .banner {
        display: none;
        margin-bottom: 18px;
      }
      .banner.success {
        display: block;
        background: rgba(20,83,45,0.1);
        border: 1px solid rgba(20,83,45,0.16);
        color: var(--primary-strong);
      }
      .banner.error {
        display: block;
        background: rgba(180,35,24,0.08);
        border: 1px solid rgba(180,35,24,0.16);
        color: var(--danger);
      }
      .status-list {
        display: grid;
        gap: 12px;
      }
      .hidden {
        display: none !important;
      }
      /* Keep the device portal focused on configuration, not a dashboard. */
      .page {
        max-width: 1080px;
        padding-top: 18px;
      }
      .brand-mark,
      .brand-copy p,
      .hero,
      .sidebar,
      .section-subtitle,
      .section-badge,
      .save-copy {
        display: none;
      }
      .brand-copy h1 {
        font-size: 22px;
      }
      .layout {
        display: block;
      }
      .main-form {
        gap: 12px;
      }
      .section-card {
        border-radius: 16px;
        box-shadow: 0 8px 24px rgba(25,39,53,0.08);
      }
      fieldset {
        padding: 18px;
      }
      .section-head {
        margin-bottom: 14px;
      }
      .section-title {
        font-size: 18px;
      }
      .save-bar {
        justify-content: flex-end;
        padding: 14px 18px;
      }
      .inline-note {
        margin-top: 14px;
        padding: 10px 12px;
      }
      /* Compact visual system for the on-device control panel. */
      body {
        background:
          radial-gradient(circle at 8% -8%, rgba(45, 212, 191, 0.16), transparent 28%),
          radial-gradient(circle at 100% 0%, rgba(59, 130, 246, 0.13), transparent 25%),
          #f4f7fb;
      }
      .topbar {
        position: sticky;
        top: 12px;
        z-index: 5;
        margin-bottom: 18px;
        padding: 12px 14px 12px 18px;
        border: 1px solid rgba(148,163,184,0.22);
        border-radius: 18px;
        background: rgba(255,255,255,0.88);
        box-shadow: 0 10px 30px rgba(15,23,42,0.08);
        backdrop-filter: blur(14px);
      }
      .brand-copy h1 {
        color: #0f172a;
        letter-spacing: -0.03em;
      }
      .toolbar button {
        padding: 10px 14px;
        border-radius: 11px;
      }
      .section-card {
        border: 1px solid #e2e8f0;
        background: rgba(255,255,255,0.96);
        box-shadow: 0 10px 24px rgba(15,23,42,0.055);
      }
      .section-card:hover {
        border-color: #cbd5e1;
        box-shadow: 0 14px 30px rgba(15,23,42,0.075);
      }
      .section-title {
        display: flex;
        align-items: center;
        gap: 9px;
        color: #0f172a;
      }
      .section-title:before {
        content: "";
        width: 8px;
        height: 8px;
        border-radius: 50%;
        background: linear-gradient(135deg, #14b8a6, #2563eb);
        box-shadow: 0 0 0 4px #ecfeff;
      }
      label {
        color: #475569;
        font-weight: 700;
      }
      input,
      select {
        border-color: #dbe3ee;
        border-radius: 11px;
        background: #fbfdff;
      }
      input:focus,
      select:focus {
        border-color: #14b8a6;
        box-shadow: 0 0 0 4px rgba(20,184,166,0.13);
      }
      .row-card {
        background: #f8fafc;
        border-color: #e7edf5;
        border-radius: 12px;
      }
      .container {
        color: #334155;
      }
      .inline-note {
        display: flex;
        flex-wrap: wrap;
        gap: 5px 10px;
        align-items: center;
        background: #f8fafc;
        border-color: #e2e8f0;
        color: #64748b;
      }
      .inline-note strong {
        color: #334155;
      }
      .inline-note.is-connected {
        background: #ecfdf5;
        border-color: #a7f3d0;
        color: #047857;
      }
      .inline-note.is-connected strong,
      .inline-note.is-connected #lanRuntimeState {
        color: #047857;
      }
      .save-bar {
        border: 1px solid rgba(15,23,42,0.1);
        border-radius: 16px;
        background: #0f172a;
        box-shadow: 0 16px 35px rgba(15,23,42,0.22);
      }
      .save-actions .button-secondary {
        background: rgba(255,255,255,0.1);
        border-color: rgba(255,255,255,0.2);
        color: #fff;
      }
      /* Reference layout: colored section headers with compact, readable fields. */
      body {
        background:
          radial-gradient(circle at 15% 0%, rgba(147,197,253,0.5), transparent 30%),
          linear-gradient(180deg, #edf7ff 0%, #f7fbff 58%, #eaf4ff 100%);
      }
      .page {
        max-width: 1120px;
      }
      .topbar {
        padding: 16px 22px;
        border: 0;
        border-radius: 0 0 16px 16px;
        background: linear-gradient(118deg, #1685ec 0%, #1668d7 54%, #1059c0 100%);
        box-shadow: 0 15px 36px rgba(30,100,200,0.28);
      }
      .brand-mark {
        display: grid;
        width: 46px;
        height: 46px;
        border-radius: 13px;
        background: rgba(255,255,255,0.16);
        box-shadow: none;
        font-size: 0;
      }
      .brand-mark:after {
        content: "▣";
        font-size: 27px;
        color: #fff;
      }
      .brand-copy p {
        display: block;
        margin-top: 2px;
        color: rgba(255,255,255,0.84);
        font-size: 13px;
      }
      .brand-copy h1 {
        color: #fff;
        font-size: 25px;
      }
      .toolbar button {
        min-width: 104px;
        color: #fff;
        background: rgba(255,255,255,0.17);
        border: 1px solid rgba(255,255,255,0.28);
        box-shadow: none;
      }
      .toolbar .button-danger {
        background: linear-gradient(135deg, #fb4b64, #f13651);
        border-color: transparent;
      }
      .main-form {
        display: grid;
        grid-template-columns: repeat(2, minmax(0, 1fr));
        gap: 14px;
      }
      .main-form > .section-card,
      .main-form > .save-bar {
        grid-column: 1 / -1;
      }
      #modbusSettingsSection,
      #canbusSettingsSection {
        grid-column: auto;
      }
      .section-card {
        --section: #2495f4;
        --section-tint: #eaf6ff;
        position: relative;
        overflow: hidden;
        border-radius: 15px;
        border-color: #e2ebf5;
      }
      .section-card:before {
        content: "";
        position: absolute;
        left: 0;
        top: 0;
        bottom: 0;
        width: 4px;
        background: var(--section);
      }
      .section-card fieldset {
        padding: 0 24px 18px;
      }
      .section-head {
        margin: 0 -24px 15px;
        padding: 13px 24px;
        background: linear-gradient(90deg, var(--section-tint), rgba(255,255,255,0.3));
      }
      .section-subtitle {
        display: block;
        margin-top: 2px;
        color: #64748b;
        font-size: 13px;
      }
      .section-title:before {
        content: "";
        width: 38px;
        height: 38px;
        border-radius: 11px;
        background: var(--section);
        box-shadow: 0 7px 14px color-mix(in srgb, var(--section) 30%, transparent);
      }
      #deviceInfoSection { --section: #1597ed; --section-tint: #e5f4ff; }
      #mqttSection { --section: #8b35ef; --section-tint: #f2eaff; }
      #lanSection { --section: #18c97c; --section-tint: #e6fbf1; }
      #wlanSettingsSection { --section: #288def; --section-tint: #e8f4ff; }
      #serialSettingsSection { --section: #f59e0b; --section-tint: #fff6df; }
      #modbusSettingsSection { --section: #f04462; --section-tint: #ffebef; }
      #canbusSettingsSection { --section: #813cf0; --section-tint: #f2ebff; }
      #deviceInfoSection .section-title:before { content: "▣"; }
      #mqttSection .section-title:before { content: "⌁"; }
      #lanSection .section-title:before { content: "⌘"; }
      #wlanSettingsSection .section-title:before { content: "⌁"; }
      #serialSettingsSection .section-title:before { content: "▤"; }
      #modbusSettingsSection .section-title:before { content: "⚙"; }
      #canbusSettingsSection .section-title:before { content: "⌘"; }
      .section-title:before {
        display: grid;
        place-items: center;
        color: #fff;
        font-size: 21px;
        box-shadow: 0 8px 15px rgba(15,23,42,0.13);
      }
      table {
        border-spacing: 0 8px;
      }
      td {
        padding: 0 6px;
      }
      label {
        white-space: nowrap;
        font-size: 12px;
        color: #18233b;
      }
      input,
      select {
        min-height: 38px;
        padding: 9px 11px;
        background: #fff;
      }
      .row-card {
        margin-bottom: 6px;
        padding: 10px 0;
        background: transparent;
        border: 0;
      }
      .checkmark {
        border-radius: 50%;
      }
      .inline-note {
        border-radius: 9px;
        font-size: 12px;
      }
      .save-bar {
        position: static;
        margin-top: 2px;
        background: transparent;
        box-shadow: none;
        border: 0;
      }
      .save-actions .button-secondary {
        background: #fff;
        border-color: #d5e1ee;
        color: #20304a;
      }
      .save-actions #saveButton {
        background: linear-gradient(135deg, #20c979, #13a85f);
        box-shadow: 0 9px 18px rgba(19,168,95,0.24);
      }
      @media (max-width: 1100px) {
        .layout {
          grid-template-columns: 1fr;
        }
        .sidebar {
          position: static;
        }
      }
      @media (max-width: 920px) {
        .hero {
          grid-template-columns: 1fr;
        }
        .hero-metrics {
          grid-template-columns: 1fr;
        }
      }
      @media (max-width: 720px) {
        .main-form {
          grid-template-columns: 1fr;
        }
        #modbusSettingsSection,
        #canbusSettingsSection {
          grid-column: 1 / -1;
        }
        .page {
          padding: 18px 12px 28px;
        }
        .topbar {
          align-items: flex-start;
          flex-direction: column;
        }
        .toolbar {
          width: 100%;
          justify-content: stretch;
        }
        .toolbar button,
        .toolbar a {
          flex: 1 1 100%;
        }
        table,
        tbody,
        tr,
        td {
          display: block;
          width: 100%;
        }
        td {
          padding: 0;
          margin-bottom: 12px;
        }
        .save-bar {
          flex-direction: column;
          align-items: stretch;
        }
        .save-actions button {
          width: 100%;
        }
      }
    </style>
  </head>
  <body>
    <div class="page">
      <div class="topbar">
        <div class="brand">
          <div class="brand-mark">32</div>
          <div class="brand-copy">
            <h1>ESP32 Config</h1>
            <p>Device Configuration &amp; Network Settings</p>
          </div>
        </div>
        <div class="toolbar">
          <button class="button-secondary" type="button" onclick="refreshSettings()">↻&nbsp; Refresh</button>
          <button class="button-danger" type="button" onclick="logout()">⇥&nbsp; Logout</button>
        </div>
      </div>

      <div class="hero">
        <section class="hero-card">
          <div class="eyebrow">Device Configuration Portal</div>
          <h2>Provision faster, deploy cleaner, maintain with confidence.</h2>
          <p>Review device identity, update connectivity, and tune industrial interfaces from a single browser session. Built to work directly from the ESP32 without external cloud services.</p>
          <div class="hero-metrics">
            <div class="metric">
              <strong id="metricDeviceName">Device</strong>
              <span>Configured device identity</span>
            </div>
            <div class="metric">
              <strong id="metricNetworkMode">Waiting</strong>
              <span>Primary wireless provisioning mode</span>
            </div>
            <div class="metric">
              <strong id="metricMQTTState">Disabled</strong>
              <span>Telemetry publishing status</span>
            </div>
          </div>
        </section>
        <aside class="status-panel">
          <div class="status-chip">Live Session</div>
          <div>
            <div class="status-label">Portal status</div>
            <div class="status-value" id="portalState">Active</div>
          </div>
          <div>
            <div class="status-label">Firmware version</div>
            <div class="status-value small" id="heroFirmwareVersion">Unknown</div>
          </div>
          <div>
            <div class="status-label">Current WiFi SSID</div>
            <div class="status-value small" id="heroWifiSsid">Not configured</div>
          </div>
        </aside>
      </div>

      <div id="pageBanner" class="banner"></div>

      <div class="layout">
        <form id="mqttForm" class="main-form">
          <section id="deviceInfoSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">Device</div>
            <div class="section-subtitle">Read-only metadata currently embedded in the firmware build.</div>
          </div>
          <div class="section-badge">Overview</div>
        </legend>
        <table>
          <tr>
            <td><label>Device Name</label></td>
            <td><input type="text" id="deviceName" placeholder="Device Name" disabled></td>
            <td><label>Firmware Version</label></td>
            <td><input type="text" id="firmwareVersion" placeholder="Firmware Version" disabled></td>
          </tr>
        </table>
      </fieldset>
          </section>
      
      <section id="mqttSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">MQTT</div>
            <div class="section-subtitle">Telemetry broker, identity, and publish-subscribe topics.</div>
          </div>
          <div class="section-badge">Cloud</div>
        </legend>
        <div class="row-card toggle-row">
          <label class="container">Enable MQTT
            <input type="checkbox" id="EnableMQTT" onchange="toggleEnableMQTT()">
            <span class="checkmark"></span>
          </label>
        </div>
        <table>
          <tr>
            <td><label>MQTT Server</label></td>
            <td><input type="text" id="mqttServer" placeholder="MQTT Server" required></td>
            <td><label>Port</label></td>
            <td><input type="number" id="mqttPort" placeholder="Port" required></td>
          </tr>
          <tr>
            <td><label>Username</label></td>
            <td><input type="text" id="mqttUser" placeholder="Username"></td>
            <td><label>Password</label></td>
            <td><input type="password" id="mqttPassword" placeholder="Password"></td>
          </tr>
          <tr>
            <td><label>ClientId</label></td>
            <td><input type="text" id="clientId" placeholder="ClientId"></td>
            <td><label>Subscribe Topic</label></td>
            <td><input type="text" id="mqttSubscribe" placeholder="Subscribe Topic"></td>
          </tr>
          <tr>
            <td><label>Publish Topic</label></td>
            <td><input type="text" id="mqttPublish" placeholder="Publish Topic"></td>
            <td><label>Sampling Time</label></td>
            <td><input type="number" id="samplingtime" placeholder="Sampling Time"></td>
          </tr>
        </table>
      </fieldset>
      </section>
      
      <section id="lanSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">Ethernet</div>
            <div class="section-subtitle">Ethernet addressing and local network access parameters.</div>
          </div>
          <div class="section-badge">Network</div>
        </legend>
        <div class="row-card toggle-row">
          <label class="container">Use DHCP
            <input type="radio" name="lanType" id="lanDhcp" value="dhcp" onchange="toggleLanInputs()">
            <span class="checkmark"></span>
          </label>
          <label class="container">Use Static IP
            <input type="radio" name="lanType" id="lanStatic" value="static" onchange="toggleLanInputs()">
            <span class="checkmark"></span>
          </label>
        </div>
        <table>
          <tr>
            <td><label>MAC Address</label></td>
            <td><input type="text" id="lanMac" placeholder="AA:BB:CC:DD:EE:FF"  disabled></td>
          </tr>
          <tr>
            <td><label>IP Address</label></td>
            <td><input type="text" id="lanIp" pattern="^(\d{1,3}\.){3}\d{1,3}$" placeholder="192.168.1.100"></td>
            <td><label>Subnet Mask</label></td>
            <td><input type="text" id="lanSubnet" placeholder="255.255.255.0"></td>
          </tr>
          <tr>
            <td><label>Gateway</label></td>
            <td><input type="text" id="lanGateway" placeholder="192.168.1.1"></td>
            <td><label>DNS</label></td>
            <td><input type="text" id="lanDns" placeholder="8.8.8.8"></td>
          </tr>
        </table>
        <div id="lanRuntimeStatus" class="inline-note">
          <strong>Active LAN:</strong>
          <span id="lanRuntimeState">Waiting for an IP address...</span>
          <span id="lanRuntimeValues"></span>
        </div>
      </fieldset>
      </section>
      
      <section id="wlanSettingsSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">Wi-Fi</div>
            <div class="section-subtitle">WiFi credentials, scanning, and static IP overrides.</div>
          </div>
          <div class="section-badge">Wireless</div>
        </legend>
        <div class="row-card toggle-row">
          <label class="container">Use DHCP
            <input type="radio" name="wlanType" id="wlanDhcp" value="dhcp" onchange="toggleWlanInputs()">
            <span class="checkmark"></span>
          </label>
          <label class="container">Use Static IP
            <input type="radio" name="wlanType" id="wlanStatic" value="static" onchange="toggleWlanInputs()">
            <span class="checkmark"></span>
          </label>
          <button class="button-secondary" type="button" onclick="scanWifi()">Scan WiFi</button>
        </div>
        <table>
          <tr>
            <td><label>MAC Address</label></td>
            <td><input type="text" id="wlanMac" placeholder="AA:BB:CC:DD:EE:FF" disabled></td>
          </tr>
          <tr>
            <td><label>SSID</label></td>
            <td>
              <input  type="text" id="wlanSsid" placeholder="WiFi SSID" list="ssidList" required>
              <datalist id="ssidList"></datalist>
            </td>
            <td><label>Password</label></td>
            <td><input type="password" id="wlanPassword" placeholder="WiFi Password"></td>
          </tr>
          <tr>
            <td><label>IP Address</label></td>
            <td><input type="text" id="wlanIp" pattern="^(\d{1,3}\.){3}\d{1,3}$" placeholder="192.168.1.101"></td>
            <td><label>Subnet Mask</label></td>
            <td><input type="text" id="wlanSubnet" placeholder="255.255.255.0"></td>
          </tr>
          <tr>
            <td><label>Gateway</label></td>
            <td><input type="text" id="wlanGateway" placeholder="192.168.1.1"></td>
            <td><label>DNS</label></td>
            <td><input type="text" id="wlanDns" placeholder="8.8.8.8"></td>
          </tr>
        </table>
        <div id="wifiScanNote" class="inline-note hidden">WiFi scan will populate the SSID suggestions list.</div>
      </fieldset>
      </section>
      
      <section id="serialSettingsSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">Serial</div>
            <div class="section-subtitle">UART framing and baud settings for device communication.</div>
          </div>
          <div class="section-badge">Interface</div>
        </legend>
        <table>
          <tr>
            <td><label>Baud rates </label></td>
            <td>
              <select id="serialBaud">
                <option value=4800>4800</option>
                <option value=9600>9600</option>
                <option value=19200>19200</option>
                <option value=38400>38400</option>
                <option value=115200>115200</option>
              </select>
            </td>
            <td><label>Data bits </label></td>
            <td>
              <select id="serialDataBits">
                <option value=8>8</option>
                <option value=7>7</option>
                
              </select>
            </td>
          </tr>

          <tr>
            <td><label>Parity </label></td>
            <td>
              <select id="serialParity">
                <option value="none">None</option>
                <option value="even">Even</option>
                <option value="odd">Odd</option>
              </select>
            </td>
            <td><label>Stop bits </label></td>
            <td>
              <select id="serialStopBits">
                <option value=1>1</option>
                <option value=2>2</option>
                
              </select>
            </td>
          </tr>
          
        </table>
      </fieldset>
      </section>
      
      <section id="modbusSettingsSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">Modbus</div>
            <div class="section-subtitle">Choose TCP or RTU operation and define endpoint settings.</div>
          </div>
          <div class="section-badge">Industrial</div>
        </legend>
        <div class="row-card toggle-row">
          <label class="container">TCP Client
            <input type="checkbox" id="modbusTcp" onchange="toggleModbusTcp()">
            <span class="checkmark"></span>
          </label>
          <label class="container">RTU Master
            <input type="checkbox" id="modbusRtu" onchange="toggleModbusRTU()">
            <span class="checkmark"></span>
          </label>
        </div>
      
        <table id="tcpClientSection" style="display:none;">
          <tr>
            <td><label>Server IP</label></td>
            <td><input type="text" id="server_ip" placeholder="Server IP"></td>
            <td><label>Server Port</label></td>
            <td><input type="number" id="server_port" placeholder="Server Port"></td>
          </tr>
        </table>
	        <table id="rtuMasterSection" style="display:none;">
	           <tr>
            <td><label> Address </label></td>
            <td><input type="number" id="rtuAddress" placeholder="Address" min="1" step="1"></td>
            
           </tr>
        </table>

      </fieldset>
      </section>

      <section id="canbusSettingsSection" class="section-card">
      <fieldset>
        <legend class="section-head">
          <div>
            <div class="section-title">CAN Bus</div>
            <div class="section-subtitle">Control bus role, identity, and baud configuration.</div>
          </div>
          <div class="section-badge">Field Bus</div>
        </legend>
         <div class="row-card toggle-row">
          <label class="container">Master
            <input type="checkbox" id="canbusMaster" >
            <span class="checkmark"></span>
          </label>
        </div>
         <table>
          
          <tr>
            <td><label>ID</label></td>
            <td><input type="text" id="canbusId" placeholder="CAN Bus ID" ></td>
            
          </tr>
           <tr>
            <td><label>Baud rates  </label></td>
            <td>
              <select id="canbusBaud">
                <option value=100>100KBPS</option>
                <option value=125>125KBPS</option>
                <option value=250>250KBPS</option>
                <option value=500>500KBPS</option>
                <option value=800>800KBPS</option>
                <option value=1000>1MBPS</option>
              </select>
            </td>
          </tr>
         </table>

      </fieldset>
      </section>
      <div class="save-bar">
        <div class="save-copy">
          <strong>Ready to apply changes</strong>
          <span>Saving will persist the configuration and restart the device.</span>
        </div>
        <div class="save-actions">
          <button class="button-secondary" type="button" onclick="refreshSettings()">Reload</button>
          <button id="saveButton" type="button" onclick="submitSettings()">Save &amp; Restart</button>
        </div>
      </div>
    </form>

    <aside class="sidebar">
      <div class="sidebar-card">
        <h3>Quick Actions</h3>
        <p>Open maintenance tools exposed by the current firmware build.</p>
        <div class="sidebar-links" id="featureLinks">
          <a id="otaLink" href="/update">Firmware Update <span>Open</span></a>
          <a id="spiffsLink" href="/update-spiffs">SPIFFS Update <span>Open</span></a>
          <a id="fileManagerLink" href="/file-manager">File Manager <span>Open</span></a>
        </div>
      </div>
      <div class="sidebar-card">
        <h3>Session Notes</h3>
        <div class="status-list">
          <div>
            <div class="status-label">MQTT broker</div>
            <div class="status-value small" id="summaryBroker">Not configured</div>
          </div>
          <div>
            <div class="status-label">LAN mode</div>
            <div class="status-value small" id="summaryLanMode">DHCP</div>
          </div>
          <div>
            <div class="status-label">Serial profile</div>
            <div class="status-value small" id="summarySerial">9600 / 8N1</div>
          </div>
        </div>
      </div>
      <div class="sidebar-card">
        <h3>Operational Reminder</h3>
        <p>Keep network values complete when static IP mode is enabled. Partial static settings can prevent the device from reconnecting after restart.</p>
      </div>
    </aside>
      </div>
    </div>

    <script>
      if (!sessionStorage.getItem("esp32_token")) {
        sessionStorage.removeItem("esp32_token");
         window.location.href = "/login";
        }
      document.getElementById('wlanSsid').addEventListener('focus', function () {
        document.getElementById('wifiScanNote').classList.remove('hidden');
      });
      function showBanner(message, type) {
        const banner = document.getElementById('pageBanner');
        banner.textContent = message;
        banner.className = 'banner ' + type;
      }
      function clearBanner() {
        const banner = document.getElementById('pageBanner');
        banner.textContent = '';
        banner.className = 'banner';
      }
      function setSaveBusy(isBusy) {
        const saveButton = document.getElementById('saveButton');
        saveButton.disabled = isBusy;
        saveButton.textContent = isBusy ? 'Saving...' : 'Save & Restart';
      }
      async function scanWifi() {
        document.getElementById('wifiScanNote').classList.remove('hidden');
        document.getElementById('wifiScanNote').textContent = 'Scanning nearby access points...';
        const token = sessionStorage.getItem("esp32_token");
        try {
          const response = await fetch('/scan_wifi', {
            headers: { 'Authorization': 'Bearer ' + token }
          });

          if (!response.ok) {
            sessionStorage.removeItem("esp32_token");
            window.location.href = "/login";
            return;
          }

          const ssids = await response.json();
          const datalist = document.getElementById('ssidList');
          datalist.innerHTML = '';
          ssids.forEach(ssid => {
            const option = document.createElement('option');
            option.value = ssid;
            datalist.appendChild(option);
          });
          document.getElementById('wifiScanNote').textContent = ssids.length
            ? 'Found ' + ssids.length + ' network(s). Suggestions updated below.'
            : 'No WiFi networks were found during this scan.';
        } catch (error) {
          document.getElementById('wifiScanNote').textContent = 'WiFi scan failed. Please try again.';
        }
      }
      async function submitSettings() {
        clearBanner();
        setSaveBusy(true);
        const data = {
          server: document.getElementById('mqttServer').value,
          port: document.getElementById('mqttPort').value,
          clientId: document.getElementById('clientId').value,
          user: document.getElementById('mqttUser').value,
          password: document.getElementById('mqttPassword').value,
          publishTopic: document.getElementById('mqttPublish').value,
          subscribeTopic: document.getElementById('mqttSubscribe').value,
          samplingtime: document.getElementById('samplingtime').value,
          enableMQTT: document.getElementById('EnableMQTT').checked,
          server_ip: document.getElementById('server_ip').value,
          server_port: document.getElementById('server_port').value,
          rtuAddress: document.getElementById('rtuAddress').value,

          lanType: document.getElementById('lanDhcp').checked ? true : false,
          lanMac: document.getElementById('lanMac').value,
          lanIp: document.getElementById('lanIp').value,
          lanSubnet: document.getElementById('lanSubnet').value,
          lanGateway: document.getElementById('lanGateway').value,
          lanDns: document.getElementById('lanDns').value,

          wlanType: document.getElementById('wlanDhcp').checked ? true : false,
          wlanMac: document.getElementById('wlanMac').value,
          wlanSsid: document.getElementById('wlanSsid').value,
          wlanPassword: document.getElementById('wlanPassword').value,
          wlanIp: document.getElementById('wlanIp').value,
          wlanSubnet: document.getElementById('wlanSubnet').value,
          wlanGateway: document.getElementById('wlanGateway').value,
          wlanDns: document.getElementById('wlanDns').value,

          deviceName: document.getElementById('deviceName').value,
          firmwareVersion: document.getElementById('firmwareVersion').value,

          serialBaud: document.getElementById('serialBaud').value,
          serialDataBits: document.getElementById('serialDataBits').value,
          serialParity: document.getElementById('serialParity').value,
          serialStopBits: document.getElementById('serialStopBits').value,

          modbusTcp: document.getElementById('modbusTcp').checked,
          modbusRtu: document.getElementById('modbusRtu').checked,
          rtuAddress: document.getElementById('rtuAddress').value,
          canbusMaster: document.getElementById('canbusMaster').checked,
          canbusId: document.getElementById('canbusId').value,
          canbusBaud: document.getElementById('canbusBaud').value

        };
        const token = sessionStorage.getItem("esp32_token");
        try {
          const response = await fetch('/save', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json','Authorization': 'Bearer ' + token },
            body: JSON.stringify(data)
          });
          const message = await response.text();
          if (!response.ok) {
            sessionStorage.removeItem("esp32_token");
            showBanner(message || 'Unable to save settings.', 'error');
            setSaveBusy(false);
            return;
          }
          showBanner(message || 'Settings saved. Device is restarting...', 'success');
        } catch (error) {
          showBanner('Unable to save settings. Please retry once the device is reachable.', 'error');
          setSaveBusy(false);
        }
      }
      function safeDisplay(flag) {
        return flag === true ? 'block' : 'none';
      }
      function updateSummary(data) {
        document.getElementById('metricDeviceName').textContent = data.deviceName || 'ESP32 Device';
        document.getElementById('metricNetworkMode').textContent = data.wlanType ? 'DHCP' : 'Static IP';
        document.getElementById('metricMQTTState').textContent = data.enableMQTT ? 'Enabled' : 'Disabled';
        document.getElementById('portalState').textContent = data.configPortalEnabled ? 'Active' : 'Disabled';
        document.getElementById('heroFirmwareVersion').textContent = data.firmwareVersion || 'Unknown';
        document.getElementById('heroWifiSsid').textContent = data.wlanSsid || 'Not configured';
        document.getElementById('summaryBroker').textContent = data.server || 'Not configured';
        document.getElementById('summaryLanMode').textContent = data.lanType ? 'DHCP' : 'Static IP';
        document.getElementById('summarySerial').textContent =
          String(data.serialBaud || '9600') + ' / ' +
          String(data.serialDataBits || '8') +
          (data.serialParity === 'even' ? 'E' : data.serialParity === 'odd' ? 'O' : 'N') +
          String(data.serialStopBits || '1');
      }
      function updateLanRuntime(data) {
        const panel = document.getElementById('lanRuntimeStatus');
        const state = document.getElementById('lanRuntimeState');
        const values = document.getElementById('lanRuntimeValues');
        panel.classList.toggle('is-connected', data.lanConnected);
        if (!data.lanConnected) {
          state.textContent = 'Waiting for an IP address...';
          values.textContent = '';
          return;
        }
        state.textContent = 'Connected';
        values.textContent = 'IP: ' + data.lanIp +
          ' | Subnet: ' + data.lanSubnet +
          ' | Gateway: ' + data.lanGateway +
          ' | DNS: ' + data.lanDns;
      }
      async function loadLanRuntime() {
        const token = sessionStorage.getItem('esp32_token');
        if (!token) return;
        try {
          const response = await fetch('/network-status', {
            headers: { 'Authorization': 'Bearer ' + token }
          });
          if (response.ok) updateLanRuntime(await response.json());
        } catch (error) {
        }
      }
      async function loadSettings() {
        const token = sessionStorage.getItem("esp32_token");
        const response = await fetch('/load',{
          
          headers: { 'Authorization': 'Bearer ' + token },
          
        });
        if (!response.ok) {
          sessionStorage.removeItem("esp32_token");
          //window.location.href = "/login";
          return;
        }
        const data = await response.json();
        document.getElementById('deviceInfoSection').style.display     = safeDisplay(data.showDeviceInfo);
        document.getElementById('mqttSection').style.display           = safeDisplay(data.showMQTT);
        document.getElementById('lanSection').style.display            = safeDisplay(data.showLAN);
        document.getElementById('wlanSettingsSection').style.display   = safeDisplay(data.showWLAN);
        document.getElementById('serialSettingsSection').style.display = safeDisplay(data.showSerial);
        document.getElementById('modbusSettingsSection').style.display = safeDisplay(data.showModbus);
        document.getElementById('canbusSettingsSection').style.display = safeDisplay(data.showCanBus);
        updateSummary(data);

        document.getElementById('deviceName').value = data.deviceName || '';
        document.getElementById('firmwareVersion').value = data.firmwareVersion || '';
        if(data.enableMQTT){
          document.getElementById('EnableMQTT').checked = true;
        }else{
          document.getElementById('EnableMQTT').checked = false;
        }
        document.getElementById('mqttServer').value = data.server || '';
        document.getElementById('mqttPort').value = data.port || '';
        document.getElementById('clientId').value = data.clientId || '';
        document.getElementById('mqttUser').value = data.user || '';
        document.getElementById('mqttPassword').value = data.password || '';
        document.getElementById('mqttPublish').value = data.publishTopic || '';
        document.getElementById('mqttSubscribe').value = data.subscribeTopic || '';
        document.getElementById('samplingtime').value = data.samplingtime || '';
        toggleEnableMQTT()
       
        if (data.lanType) {
          document.getElementById('lanDhcp').checked = true;
        } else {
          document.getElementById('lanStatic').checked = true;
        }

        document.getElementById('lanMac').value = data.lanMac || '';
        document.getElementById('lanIp').value = data.lanIp || '';
        document.getElementById('lanSubnet').value = data.lanSubnet || '';
        document.getElementById('lanGateway').value = data.lanGateway || '';
        document.getElementById('lanDns').value = data.lanDns || '';

        toggleLanInputs();

        if (data.wlanType) {
          document.getElementById('wlanDhcp').checked = true;
        } else {
          document.getElementById('wlanStatic').checked = true;
        }

        document.getElementById('wlanMac').value = data.wlanMac || '';
        document.getElementById('wlanSsid').value = data.wlanSsid || '';
        document.getElementById('wlanPassword').value = data.wlanPassword || '';
        document.getElementById('wlanIp').value = data.wlanIp || '';
        document.getElementById('wlanSubnet').value = data.wlanSubnet || '';
        document.getElementById('wlanGateway').value = data.wlanGateway || '';
        document.getElementById('wlanDns').value = data.wlanDns || '';

        toggleWlanInputs();

        document.getElementById('serialBaud').value = String(data.serialBaud || '9600');
        document.getElementById('serialDataBits').value = String(data.serialDataBits || '8');
        document.getElementById('serialParity').value = data.serialParity || 'none';
        document.getElementById('serialStopBits').value = String(data.serialStopBits || '1');

        document.getElementById('modbusTcp').checked = data.modbusTcp || false;
        document.getElementById('modbusRtu').checked = data.modbusRtu || false;
        toggleModbusTcp();
        document.getElementById('server_ip').value = data.server_ip || '';
        document.getElementById('server_port').value = data.server_port || '';
        toggleModbusRTU();
        document.getElementById('rtuAddress').value = String(data.rtuAddress || '1');
        document.getElementById('canbusMaster').checked = data.canbusMaster || false;
        document.getElementById('canbusId').value = data.canbusId || '';
        document.getElementById('canbusBaud').value = String(data.canbusBaud || '100');

      }
      async function refreshSettings() {
        clearBanner();
        await loadSettings();
        showBanner('Configuration reloaded from device storage.', 'success');
        setTimeout(clearBanner, 2200);
      }
      async function logout() {
        const token = sessionStorage.getItem('esp32_token');
        try {
          await fetch('/logout', {
            method: 'POST',
            headers: { 'Authorization': 'Bearer ' + token }
          });
        } catch (error) {
        }
        sessionStorage.removeItem('esp32_token');
        window.location.href = '/login';
      }
      function toggleLanInputs() {
        const isDhcp = document.getElementById('lanDhcp').checked;
        const fields = ['lanIp', 'lanSubnet', 'lanGateway', 'lanDns'];
        fields.forEach(id => {
          document.getElementById(id).disabled = isDhcp;
        });
      }

      function toggleWlanInputs() {
        const isDhcp = document.getElementById('wlanDhcp').checked;
        const fields = ['wlanIp', 'wlanSubnet', 'wlanGateway', 'wlanDns'];
        fields.forEach(id => {
          document.getElementById(id).disabled = isDhcp;
        });
      }
     
      function toggleEnableMQTT() {
        const enableMQTT = document.getElementById('EnableMQTT').checked;
        const fields = ['mqttServer', 'mqttPort','clientId', 'mqttUser', 'mqttPassword', 'mqttPublish', 'mqttSubscribe', 'samplingtime'];
        fields.forEach(id => {
          document.getElementById(id).disabled = !enableMQTT;
        });
      }
      function toggleModbusTcp() {
        const show = document.getElementById('modbusTcp').checked;
        document.getElementById('tcpClientSection').style.display = show ? 'table' : 'none';
      }
      function toggleModbusRTU() {
        const show = document.getElementById('modbusRtu').checked;
        const rtuMasterSection = document.getElementById('rtuMasterSection');
        if (rtuMasterSection) {
          rtuMasterSection.style.display = show ? 'table' : 'none';
        }
      }
      window.addEventListener('DOMContentLoaded', () => {
        if (location.pathname === '/login') return;
        loadSettings();
        loadLanRuntime();
        setInterval(loadLanRuntime, 5000);

        toggleEnableMQTT();
        toggleLanInputs();
        toggleWlanInputs();
        toggleModbusTcp();
        toggleModbusRTU();
      });
    </script>
  </body>
</html>)rawliteral");
}
