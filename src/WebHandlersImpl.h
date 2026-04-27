#pragma once
#include "WebHandlers.h"

// ===== AUTHENTICATION & LOGIN =====
void handleLoginForm(){
  if (!requireConfigPortalPage()) return;
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  server.send(200,"text/html", R"rawliteral(<!DOCTYPE html>
<html>
<head>
<title>Login - ESP32 Config</title>
<style>
body {font-family: Arial;background: #f4f4f4;color: #333;text-align: center;margin: 0;padding: 0;}
#loginBox {margin: 90px auto 0;padding: 30px 24px;background: #fff;border-radius: 10px;box-shadow: 0 0 16px rgba(0,0,0,0.13);max-width: 320px;}
input {width: 90%;padding: 8px;margin: 10px 0;border-radius: 5px;border: 1px solid #bbb;}
button {width: 100%;padding: 10px;background: #007bff;color: #fff;border: none;border-radius: 5px;cursor: pointer;}
button:hover {background: #0056b3;}
.login-error {color: #c00;font-size: 14px;margin-top: 8px;display: none;}
</style>
</head>
<body>
<div id="loginBox">
<h2>Login</h2>
<form id="loginForm" onsubmit="return loginSubmit(event)">
<input type="text" id="loginUser" placeholder="Username" required>
<input type="password" id="loginPass" placeholder="Password" required>
<button type="submit">Login</button>
<div id="loginError" class="login-error">Invalid username or password</div>
</form>
</div>
<script>
async function loginSubmit(e) {
e.preventDefault();
const data = {user: document.getElementById('loginUser').value, pass: document.getElementById('loginPass').value};
const response = await fetch('/login', {method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(data)});
const result = await response.json();
if (result.success && result.token) {
sessionStorage.setItem("esp32_token", result.token);
window.location.href = "/";
} else {
document.getElementById('loginError').style.display = "block";
}
}
async function validateExistingToken() {
const token = sessionStorage.getItem("esp32_token");
if (!token) return;
const response = await fetch('/check_token', {headers: {'Authorization': 'Bearer ' + token}});
if (response.ok) {
window.location.href = "/";
return;
}
sessionStorage.removeItem("esp32_token");
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
<html>
  <head>
    <title>MQTT Configuration</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        margin: 20px;
        background-color: #f4f4f4;
        color: #333;
      }
      form {
        display: inline-block;
        text-align: left;
        background: #fff;
        padding: 20px;
        border-radius: 10px;
        box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      }
      input {
        margin: 5px 0;
        padding: 8px;
        width: calc(100% - 20px);
        border: 1px solid #ddd;
        border-radius: 5px;
      }
      select {
        margin: 5px 0;
        padding: 8px;
        width: calc(100% - 2px);
        border: 1px solid #ddd;
        border-radius: 5px;
      }
      
      button {
        margin: 10px 0;
        padding: 10px 20px;
        background-color: #007bff;
        color: white;
        border: none;
        border-radius: 5px;
        cursor: pointer;
      }
      button:hover {
        background-color: #0056b3;
      }

      .a {
        font-size: 18px;
        font-weight: bold;
        color: #7bae23;
        background-color: #FFFFFF;
      }

      .b {
        background-color: #f5f5f5;
        height: 35px;
        font-size: 16px;
      }

      .container {
        display: block;
        position: relative;
        padding-left: 25px;
        margin-bottom: 10px;
        cursor: pointer;
        user-select: none;
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
        top: 0;
        left: 0;
        height: 15px;
        width: 15px;
        background-color: #eee;
      }

      .container:hover input ~ .checkmark {
        background-color: #ccc;
      }

      .container input:checked ~ .checkmark {
        background-color: #2196F3;
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
        left: 4px;
        top: 0px;
        width: 4px;
        height: 10px;
        border: solid white;
        border-width: 0 3px 3px 0;
        transform: rotate(45deg);
      }

      body.dark-mode {
        background-color: #1e1e2f;
        color: #e0e0e0;
      }

      body.dark-mode form {
        background-color: #282c34;
        box-shadow: 0 0 12px rgba(0, 0, 0, 0.4);
        border: 1px solid #3c3f4c;
      }

      body.dark-mode input,
      body.dark-mode select,
      body.dark-mode textarea {
        background-color: #3b3f4a;
        color: #f5f5f5;
        border: 1px solid #555;
        padding: 8px;
        border-radius: 5px;
      }

      body.dark-mode input:disabled {
        background-color: #2b2f39;
        color: #999;
        border: 1px solid #444;
      }

      body.dark-mode button {
        background-color: #4a90e2;
        color: #fff;
        border: none;
        transition: background-color 0.3s ease;
      }

      body.dark-mode button:hover {
        background-color: #357abd;
      }

      body.dark-mode .a {
        color: #98c379; /* เขียวอ่อนแบบ developer theme */
      }

      body.dark-mode .b {
        background-color: #2a2e3a;
        font-size: 16px;
      }

      body.dark-mode .checkmark {
        background-color: #444;
        border-radius: 3px;
      }

      body.dark-mode .container input:checked ~ .checkmark {
        background-color: #4a90e2;
      }

      body.dark-mode #darkModeToggle {
        background-color: #3b3f4a;
        color: #fff;
      }

      body.dark-mode #darkModeToggle:hover {
        background-color: #4a90e2;
      }

     
    </style>
  </head>
  <body>
    
    <form id="mqttForm">
      <fieldset id="deviceInfoSection">
        <legend class="a">Device Information</legend>
        <table>
          <tr class="b">
            <td><label>Device Name</label></td>
            <td><input type="text" id="deviceName" placeholder="Device Name" disabled></td>
            <td><label>Firmware Version</label></td>
            <td><input type="text" id="firmwareVersion" placeholder="Firmware Version" disabled></td>
          </tr>
        </table>
      </fieldset>
      
      <fieldset id="mqttSection">
        <legend class="a">MQTT Configuration</legend>
        <table>
          <tr>
            <td> <label class="container">Enable
              <input type="checkbox" id="EnableMQTT" onchange="toggleEnableMQTT()">
              <span class="checkmark"></span>
            </label></td>
          </tr>
          <tr class="b">
            <td><label>MQTT Server</label></td>
            <td><input type="text" id="mqttServer" placeholder="MQTT Server" required></td>
            <td><label>Port</label></td>
            <td><input type="number" id="mqttPort" placeholder="Port" required></td>
          </tr>
          <tr class="b">
            <td><label>Username</label></td>
            <td><input type="text" id="mqttUser" placeholder="Username"></td>
            <td><label>Password</label></td>
            <td><input type="password" id="mqttPassword" placeholder="Password"></td>
          </tr>
          <tr class="b">
            <td><label>ClientId</label></td>
            <td><input type="text" id="clientId" placeholder="ClientId"></td>
            <td><label>Subscribe Topic</label></td>
            <td><input type="text" id="mqttSubscribe" placeholder="Subscribe Topic"></td>
          </tr>
          <tr class="b">
            <td><label>Publish Topic</label></td>
            <td><input type="text" id="mqttPublish" placeholder="Publish Topic"></td>
            <td><label>Sampling Time</label></td>
            <td><input type="number" id="samplingtime" placeholder="Sampling Time"></td>
          </tr>
        </table>
      </fieldset>
      
      <fieldset id="lanSection">
        <legend class="a">LAN Setting</legend>
        <table>
          <tr>
            <td >
              <label class="container">Use DHCP &nbsp;&nbsp;
                <input type="radio" name="lanType" id="lanDhcp" value="dhcp" onchange="toggleLanInputs()">
                <span class="checkmark"></span>
              </label></td>
              <td ><label class="container">Use Static 
                <input type="radio" name="lanType" id="lanStatic" value="static" onchange="toggleLanInputs()">
                <span class="checkmark"></span>
              </label></td>
            
          </tr>
          <tr class="b">
            <td><label>MAC Address</label></td>
            <td><input type="text" id="lanMac" placeholder="AA:BB:CC:DD:EE:FF"  disabled></td>
          </tr>
          <tr class="b">
            <td><label>IP Address</label></td>
            <td><input type="text" id="lanIp" pattern="^(\d{1,3}\.){3}\d{1,3}$" placeholder="192.168.1.100"></td>
            <td><label>Subnet Mask</label></td>
            <td><input type="text" id="lanSubnet" placeholder="255.255.255.0"></td>
          </tr>
          <tr class="b">
            <td><label>Gateway</label></td>
            <td><input type="text" id="lanGateway" placeholder="192.168.1.1"></td>
            <td><label>DNS</label></td>
            <td><input type="text" id="lanDns" placeholder="8.8.8.8"></td>
          </tr>
        </table>
      </fieldset>
      
      <fieldset id="wlanSettingsSection">
        <legend class="a">WLAN Setting</legend>
        <table>
          <tr>
            <td >
              <label class="container">Use DHCP&nbsp;&nbsp;
                <input type="radio" name="wlanType" id="wlanDhcp" value="dhcp" onchange="toggleWlanInputs()">
                <span class="checkmark"></span>
              </label>
            </td>
            <td >
              <label class="container">Use Static
                <input type="radio" name="wlanType" id="wlanStatic" value="static" onchange="toggleWlanInputs()">
                <span class="checkmark"></span>
              </label>
            </td>
          </tr>
          <tr class="b">
            <td><label>MAC Address</label></td>
            <td><input type="text" id="wlanMac" placeholder="AA:BB:CC:DD:EE:FF" disabled></td>
            
          </tr>
          <tr class="b">
            
            <td><label>SSID</label></td>
            <td>
              <input  type="text" id="wlanSsid" placeholder="WiFi SSID" list="ssidList" required>
              <datalist id="ssidList"></datalist>
            </td>
            <td><label>Password</label></td>
            <td><input type="password" id="wlanPassword" placeholder="WiFi Password"></td>
          </tr>
          <tr class="b">
            <td><label>IP Address</label></td>
            <td><input type="text" id="wlanIp" pattern="^(\d{1,3}\.){3}\d{1,3}$" placeholder="192.168.1.101"></td>
            <td><label>Subnet Mask</label></td>
            <td><input type="text" id="wlanSubnet" placeholder="255.255.255.0"></td>
          </tr>
          <tr class="b">
            <td><label>Gateway</label></td>
            <td><input type="text" id="wlanGateway" placeholder="192.168.1.1"></td>
            <td><label>DNS</label></td>
            <td><input type="text" id="wlanDns" placeholder="8.8.8.8"></td>
          </tr>
        </table>
      </fieldset>
      
      <fieldset id="serialSettingsSection">
        <legend class="a">Serial Settings</legend>
        <table>
          <tr class="b">
            <td width="14%"><label>Baud rates </label></td>
            <td width="14%">
              <select id="serialBaud">
                <option value=4800>4800</option>
                <option value=9600>9600</option>
                <option value=19200>19200</option>
                <option value=38400>38400</option>
                <option value=115200>115200</option>
              </select>
            </td>
            <td width="14%"><label>Data bits </label></td>
            <td width="14%">
              <select id="serialDataBits">
                <option value=8>8</option>
                <option value=7>7</option>
                
              </select>
            </td>
            <!-- <td><label>Baud rates </label></td>
            <td><input type="number" id="serialBaud" placeholder="9600"></td>
            <td><label>Data bits </label></td>
            <td><input type="number" id="serialDataBits" placeholder="8"></td> -->
          </tr>
          
          <tr class="b">
            <td width="14%"><label>Parity </label></td>
            <td width="14%">
              <select id="serialParity">
                <option value="none">None</option>
                <option value="even">Even</option>
                <option value="odd">Odd</option>
              </select>
            </td>
            <td width="14%"><label>Stop bits </label></td>
            <td width="14%">
              <select id="serialStopBits">
                <option value=1>1</option>
                <option value=2>2</option>
                
              </select>
            </td>
            <!-- <td><label>Stop bits </label></td>
            <td><input type="number" id="serialStopBits" placeholder="1"></td> -->
          </tr>
          
        </table>
      </fieldset>
      
      <fieldset id="modbusSettingsSection">
        <legend class="a">Modbus Settings</legend>
        <table>
          <tr>
            <td>
              <label class="container">TCP Client
                <input type="checkbox" id="modbusTcp" onchange="toggleModbusTcp()">
                <span class="checkmark"></span>
              </label>
            </td>
          </tr>
        </table>
      
        <!-- ส่วน TCP Client -->
        <table id="tcpClientSection" style="display:none;">
          <tr class="b">
            <td><label>Server IP</label></td>
            <td><input type="text" id="server_ip" placeholder="Server IP"></td>
            <td><label>Server Port</label></td>
            <td><input type="number" id="server_port" placeholder="Server Port"></td>
          </tr>
        </table>
        <br />
       
        <table >
          <tr>
            <td>
              <label class="container">RTU Master
                <input type="checkbox" id="modbusRtu" onchange="toggleModbusRTU()">
                <span class="checkmark"></span>
              </label>
            </td>
          </tr>
        </table>
	        <table >
	          <!-- เพิ่มฟิลด์สำหรับ RTU Master ที่นี่ -->
	           <tr class="b">
            <td><label> Address </label></td>
            <td><input type="number" id="rtuAddress" placeholder="Address" min="1" step="1"></td>
            
           </tr>
        </table>

      </fieldset>

      <fieldset id="canbusSettingsSection">
        <legend class="a">CAN Bus Settings</legend>
        <!-- เพิ่มฟิลด์สำหรับ CAN Bus ที่นี่ -->
         <table>
          <tr>
            <td>
              <label class="container">Master
                <input type="checkbox" id="canbusMaster" >
                <span class="checkmark"></span>
              </label>
            </td>
          </tr>
        </table>
         <table>
          
          <tr class="b">
            <td><label>ID</label></td>
            <td><input type="text" id="canbusId" placeholder="CAN Bus ID" ></td>
            
          </tr>
           <tr class="b">
            <td ><label>Baud rates  </label></td>
            <td >
              <select id="canbusBaud">
                <option value=100>100KBPS</option>
                <option value=125>125KBPS</option>
                <option value=250>250KBPS</option>
                <option value=500>500KBPS</option>
                <option value=800>800KBPS</option>
                <option value=1000>1MBPS</option>
              </select>
            </td>
            
            <!-- <td><label>Baud rates </label></td>
            <td><input type="number" id="serialBaud" placeholder="9600"></td>
            <td><label>Data bits </label></td>
            <td><input type="number" id="serialDataBits" placeholder="8"></td> -->
          </tr>
         </table>

      </fieldset>
      <!-- <fieldset>
        <legend class="a">Modbus Settings</legend>
        <table>
          <tr>
            <td> <label class="container">TCP Client
              <input type="checkbox" id="checkbox">
              <span class="checkmark"></span>
            </label></td>
          </tr>
          <tr class="b">
            <td><label>IP Address</label></td>
            <td><input type="text" id="ip" placeholder="IP Address" required></td>
            <td><label>Subnet Mask</label></td>
            <td><input type="text" id="subnet" placeholder="Subnet Mask" required></td>
          </tr>
          <tr class="b">
            <td><label>Gateway</label></td>
            <td><input type="text" id="gateway" placeholder="Gateway" required></td>
            <td><label>Port</label></td>
            <td><input type="number" id="port" placeholder="Port" required></td>
          </tr>
          <tr class="b">
            <td><label>DHCP</label></td>
            <td colspan="3">
              <label class="container">Enable
                <input type="checkbox" id="checkbox">
                <span class="checkmark"></span>
              </label>
            </td>
          </tr>
          <tr class="b">
            <td><label>Server IP</label></td>
            <td><input type="text" id="server_ip" placeholder="Server IP" required></td>
            <td><label>Server Port</label></td>
            <td><input type="number" id="server_port" placeholder="Server Port" required></td>
          </tr>
        </table>
      </fieldset> -->
      <button type="button" onclick="submitSettings()" >
         Save Settings
      </button>
      <!-- <button id="darkModeToggle" onclick="toggleDarkMode()">🌗 Toggle Dark Mode</button> -->
    </form>

    <script>
      if (!sessionStorage.getItem("esp32_token")) {
        sessionStorage.removeItem("esp32_token");
         window.location.href = "/login";
        }
      //loadSettings();
      document.getElementById('wlanSsid').addEventListener('focus', scanWifi);
      async function scanWifi() {
        console.log("กำลังสแกน WiFi...");
        const token = sessionStorage.getItem("esp32_token");
        const response = await fetch('/scan_wifi', {
          headers: { 'Authorization': 'Bearer ' + token }
        });
        
        if (!response.ok) {
        sessionStorage.removeItem("esp32_token");
          window.location.href = "/login";
          return;
        }

        const ssids = await response.json();
        console.log("พบ SSID:", ssids);
        const datalist = document.getElementById('ssidList');
        datalist.innerHTML = ''; // เคลียร์ก่อน
        ssids.forEach(ssid => {
          const option = document.createElement('option');
          option.value = ssid;
          datalist.appendChild(option);
        });
      }
      async function submitSettings() {
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
          // ip: document.getElementById('ip').value,
          // subnet: document.getElementById('subnet').value,
          // gateway: document.getElementById('gateway').value,
          // mbport: document.getElementById('port').value,
          // checkbox: document.getElementById('checkbox').checked,
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
        const response = await fetch('/save', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json','Authorization': 'Bearer ' + token },
          body: JSON.stringify(data)
        });
        alert(await response.text());
        if (!response.ok) {
          sessionStorage.removeItem("esp32_token");
          //window.location.href = "/login";
          return;
        }
      }
      function safeDisplay(flag) {
        return flag === true ? 'block' : 'none';
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
        console.log(data);
        document.getElementById('deviceInfoSection').style.display     = safeDisplay(data.showDeviceInfo);
        document.getElementById('mqttSection').style.display           = safeDisplay(data.showMQTT);
        document.getElementById('lanSection').style.display            = safeDisplay(data.showLAN);
        document.getElementById('wlanSettingsSection').style.display   = safeDisplay(data.showWLAN);
        document.getElementById('serialSettingsSection').style.display = safeDisplay(data.showSerial);
        document.getElementById('modbusSettingsSection').style.display = safeDisplay(data.showModbus);
        document.getElementById('canbusSettingsSection').style.display = safeDisplay(data.showCanBus);

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

        // document.getElementById('ip').value = data.ip || '';
        // document.getElementById('subnet').value = data.subnet || '';
        // document.getElementById('gateway').value = data.gateway || '';
        // document.getElementById('port').value = data.mbport || '';
        // document.getElementById('checkbox').checked = data.checkbox || false;
        
        
        //document.getElementById('EnableMQTT').checked = data.enableMQTT || false;

       
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

        toggleLanInputs(); // เรียกตอนโหลดเพื่ออัปเดต disable

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
        toggleModbusTcp(); // เพื่อแสดง/ซ่อนตอนโหลด
        document.getElementById('server_ip').value = data.server_ip || '';
        document.getElementById('server_port').value = data.server_port || '';
        toggleModbusRTU(); // เพื่อแสดง/ซ่อนตอนโหลด
        document.getElementById('rtuAddress').value = String(data.rtuAddress || '1');
	        document.getElementById('canbusMaster').checked = data.canbusMaster || false;
	        document.getElementById('canbusId').value = data.canbusId || '';
	        document.getElementById('canbusBaud').value = String(data.canbusBaud || '100');

      }
          // document.getElementById('lanMac').disabled = true;
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
      function toggleDarkMode() {
        const body = document.body;
        const isDark = body.classList.toggle('dark-mode');
        localStorage.setItem('darkMode', isDark ? 'enabled' : 'disabled');
      }

      function loadDarkMode() {
        if (localStorage.getItem('darkMode') === 'enabled') {
          document.body.classList.add('dark-mode');
        }
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
        loadDarkMode();
        loadSettings();
        
        toggleEnableMQTT()
        toggleLanInputs();
        toggleWlanInputs();
        toggleModbusTcp();
        toggleModbusRTU();
        // document.getElementById('deviceInfoSection').style.display     = safeDisplay(true);
        // document.getElementById('mqttSection').style.display           = safeDisplay(true);
        // document.getElementById('lanSection').style.display            = safeDisplay(true);
        // document.getElementById('wlanSettingsSection').style.display   = safeDisplay(true);
        // document.getElementById('serialSettingsSection').style.display = safeDisplay(true);
        // document.getElementById('modbusSettingsSection').style.display = safeDisplay(true);
      });
    </script>
  </body>
</html>)rawliteral");
}
