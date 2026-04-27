#pragma once

#include "ESP32WebConfig.h"

inline bool isAuthorized() {
  if (myActiveToken.length() == 0) return false;

  if (server.hasHeader("Authorization")) {
    const String header = server.header("Authorization");
    if (header == "Bearer " + myActiveToken) {
      return true;
    }
  }

  if (server.hasArg("token")) {
    return server.arg("token") == myActiveToken;
  }

  return false;
}

inline bool requireAuthorizationJson() {
  if (isAuthorized()) return true;
  server.send(401, "application/json", R"({"valid": false, "message": "Unauthorized"})");
  return false;
}

inline bool requireAuthorizationText() {
  if (isAuthorized()) return true;
  server.send(401, "text/plain", "Unauthorized");
  return false;
}

inline bool requireConfigPortalJson() {
  if (configPortalEnabled) return true;
  server.send(403, "application/json", R"({"valid": false, "message": "Configuration portal disabled"})");
  return false;
}

inline bool requireConfigPortalText() {
  if (configPortalEnabled) return true;
  server.send(403, "text/plain", "Configuration portal disabled");
  return false;
}

inline bool requireConfigPortalPage() {
  if (configPortalEnabled) return true;
  server.send(403, "text/html", "<h2>Configuration portal disabled</h2>");
  return false;
}

inline String genToken(int len = 24) {
  const String chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  String token = "";
  for (int i = 0; i < len; i++) {
    token += chars[random(chars.length())];
  }
  return token;
}

inline String formatBytes(size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  if (bytes < (1024 * 1024)) return String(bytes / 1024.0, 2) + " KB";
  return String(bytes / (1024.0 * 1024.0), 2) + " MB";
}
