# ไลบรารี ESP32 Web Configuration

ไลบรารี Arduino สำหรับ ESP32 ที่ใช้สร้างหน้าเว็บสำหรับตั้งค่าอุปกรณ์ โดยออกแบบเป็น API แบบ class เพื่อให้เรียกใช้งานได้ง่ายขึ้น

## คุณสมบัติ

- หน้าเว็บสำหรับตั้งค่าอุปกรณ์
- ระบบล็อกอินด้วย token
- บันทึกค่าตั้งค่าลงใน SPIFFS
- หน้าตั้งค่า MQTT, WiFi, Serial, Modbus และ CAN Bus
- รองรับเส้นทาง OTA update แบบเลือกเปิดหรือปิดได้
- รองรับเส้นทางอัปเดต SPIFFS แบบเลือกเปิดหรือปิดได้
- รองรับ file manager แบบเลือกเปิดหรือปิดได้
- รองรับการสแกน WiFi แบบเลือกเปิดหรือปิดได้
- รองรับปุ่ม config และไฟแสดงสถานะ

## API ปัจจุบัน

จุดเริ่มต้นหลักของไลบรารีคือ `ESP32WebConfigApp`

ตัวอย่าง sketch แบบพื้นฐาน:

```cpp
#include "src/ESP32WebConfigApp.h"

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
```

ดูตัวอย่างเพิ่มเติมได้ที่:

- `examples/esp32webconfig/esp32webconfig.ino`
- `examples/custom_settings/custom_settings.ino`
- `examples/minimal_features/minimal_features.ino`
- `examples/custom_button_led/custom_button_led.ino`
- `examples/ethernet_lan_enabled/ethernet_lan_enabled.ino`
- `examples/mqtt_serial_focus/mqtt_serial_focus.ino`
- `examples/serial_runtime/serial_runtime.ino`
- `examples/modbus_rtu_runtime/modbus_rtu_runtime.ino`
- `examples/modbus_tcp_runtime/modbus_tcp_runtime.ino`
- `examples/canbus_twai_runtime/canbus_twai_runtime.ino`
- `examples/status_led_disabled/status_led_disabled.ino`

## การตั้งค่าแบบกำหนดเอง

สามารถส่งอ็อบเจ็กต์ `ESP32WebConfigAppSettings` ตอนสร้าง app เพื่อกำหนดค่าเองได้

```cpp
#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.apSsid = "MyESP32Config";
  settings.apPass = "87654321";
  settings.username = "admin";
  settings.password = "secret123";
  settings.deviceName = "ESP32_Custom_Config";
  settings.firmwareVersion = "2.0.0";
  settings.configButtonPin = 0;
  settings.statusLedPin = 2;
  settings.statusLedActiveLevel = LOW;

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
```

## การเปิดหรือปิดฟีเจอร์

สามารถกำหนด feature flags ได้ก่อน include `ESP32WebConfigApp.h`

ตัวอย่าง:

```cpp
#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 0
#define ESP32WC_SECTION_LAN 0

#include <ESP32WebConfigApp.h>
```

รายการ macro สำหรับฟีเจอร์:

- `ESP32WC_FEATURE_CONFIG_BUTTON`
- `ESP32WC_FEATURE_STATUS_LED`
- `ESP32WC_FEATURE_SERIAL_LOG`
- `ESP32WC_FEATURE_PRINT_CONFIG`
- `ESP32WC_FEATURE_WIFI_SCAN`
- `ESP32WC_FEATURE_OTA`
- `ESP32WC_FEATURE_SPIFFS_UPDATE`
- `ESP32WC_FEATURE_FILE_MANAGER`

รายการ macro สำหรับ section:

- `ESP32WC_SECTION_DEVICE_INFO`
- `ESP32WC_SECTION_MQTT`
- `ESP32WC_SECTION_LAN`
- `ESP32WC_SECTION_WLAN`
- `ESP32WC_SECTION_SERIAL`
- `ESP32WC_SECTION_MODBUS`
- `ESP32WC_SECTION_CANBUS`

ค่าเริ่มต้นของ macro ต่าง ๆ อยู่ใน `src/ESP32WebConfig.h`

## โครงสร้างข้อมูลการตั้งค่า

ไลบรารีใช้โครงสร้างกลางชื่อ `Configsettings`

```cpp
struct Configsettings {
  DeviceInfo device;
  MQTTConfig mqtt;
  NetworkConfig lan;
  WLANConfig wlan;
  MySerialConfig serial;
  ModbusConfig modbus;
  CanbusConfig canbus;
};
```

## ค่าพื้นฐานเริ่มต้น

- AP SSID: `ESP32_MQTT_Config`
- AP password: `12345678`
- Username: `admin`
- Password: `admin123`
- ขา config button: `GPIO 26`
- ขา status LED: `GPIO 23`

## Web Routes

เส้นทางที่เปิดใช้งานเสมอ:

- `/`
- `/login`
- `/load`
- `/save`
- `/check_token`

เส้นทางที่เปิดหรือปิดได้ตาม feature flag:

- `/update`
- `/update-spiffs`
- `/file-manager`
- `/file-list`
- `/download-file`
- `/delete-file`
- `/upload`
- `/scan_wifi`

## Dependencies

ไลบรารีที่จำเป็น:

- `ArduinoJson`

ไลบรารีจาก ESP32 core ที่มีการใช้งาน:

- `WiFi`
- `WebServer`
- `FS`
- `SPIFFS`
- `Update`

หมายเหตุ:

- ใน `library.properties` ยังมีการระบุ `PubSubClient` และ `Ethernet` เป็น dependency ด้วย

## การติดตั้ง

1. คัดลอกโฟลเดอร์นี้ไปไว้ในโฟลเดอร์ `libraries` ของ Arduino
2. รีสตาร์ต Arduino IDE หรือ reload environment ที่ใช้งาน
3. ติดตั้ง `ArduinoJson` หากยังไม่มี
4. หากใช้งาน MQTT หรือ Ethernet ให้ติดตั้ง `PubSubClient` และ `Ethernet` เพิ่มด้วย

## หมายเหตุเพิ่มเติม

- โค้ดชุดนี้ถูกปรับจากโครงสร้างแบบเก่าใน `web.h` มาเป็นรูปแบบที่แยกส่วนมากขึ้น
- แม้ภายนอกจะใช้งานผ่าน class แต่ภายในบาง handler ยังใช้ shared global state เพื่อให้เข้ากันได้กับโค้ดเดิม
- มีการอ้างอิง logic จากโฟลเดอร์ `old` สำหรับส่วน network, MQTT และพฤติกรรมหน้า config บางส่วน

## ตัวอย่างการใช้งาน

- `examples/esp32webconfig/esp32webconfig.ino`: ตัวอย่างแบบพื้นฐาน
- `examples/custom_settings/custom_settings.ino`: ตัวอย่างการเปลี่ยน credentials, pin และ feature flags
- `examples/minimal_features/minimal_features.ino`: ตัวอย่างแบบเปิดเฉพาะฟีเจอร์ที่จำเป็น
- `examples/custom_button_led/custom_button_led.ino`: ตัวอย่างการเปลี่ยนขาปุ่มและไฟแสดงสถานะ
- `examples/ethernet_lan_enabled/ethernet_lan_enabled.ino`: ตัวอย่างการเปิดใช้งาน LAN/W5500
- `examples/mqtt_serial_focus/mqtt_serial_focus.ino`: ตัวอย่างที่เน้น MQTT, Serial และ Modbus
- `examples/serial_runtime/serial_runtime.ino`: ตัวอย่างการเปิด `Serial2` จากค่าที่ตั้งในหน้าเว็บ
- `examples/modbus_rtu_runtime/modbus_rtu_runtime.ino`: ตัวอย่าง Modbus RTU แบบ raw frame ผ่าน RS485
- `examples/modbus_tcp_runtime/modbus_tcp_runtime.ino`: ตัวอย่าง Modbus TCP แบบ raw frame ผ่าน WiFi
- `examples/canbus_twai_runtime/canbus_twai_runtime.ino`: ตัวอย่าง CAN Bus ด้วย TWAI ของ ESP32
- `examples/status_led_disabled/status_led_disabled.ino`: ตัวอย่างปิดฟีเจอร์ `STATUS_LED`
