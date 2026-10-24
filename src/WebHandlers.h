#pragma once
#include <WebServer.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include "WebSupport.h"

// ===== HANDLER DECLARATIONS =====
void handleRoot();
void handleLoginForm();
void handleLogin();
void handleLogout();
void handleLoad();
void handleSave();
void handleUpdateForm();
void handleUpdate();
void handleSpiffsUpdateForm();
void handleSpiffsUpdate();
void handleFileManager();
void handleFileList();
void handleDownloadFile();
void handleEditFile();
void handleSaveFile();
void handleDeleteFile();
void handleFileUploadPage();
void handleFileUpload();
void handleScanWiFi();
void handleCheckToken();
void saveSettings(bool restart);
void loadSettings();
