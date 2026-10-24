# ESP32 Web Config

ไลบรารี Arduino สำหรับสร้างหน้าเว็บตั้งค่า ESP32 จากอุปกรณ์โดยตรง รองรับ Wi‑Fi, Ethernet, MQTT, Serial, Modbus และ CAN Bus โดยจัดเก็บค่าตั้งค่าไว้ใน SPIFFS

หน้าเว็บปัจจุบันเป็นฟอร์มตั้งค่าแบบ responsive พร้อมสถานะ Ethernet แบบ real-time สำหรับ IP, Subnet Mask, Gateway และ DNS ที่ได้รับจาก DHCP

## ความสามารถ

- Config portal แบบ AP พร้อม login token
- บันทึก/โหลดการตั้งค่าจาก SPIFFS
- Wi‑Fi DHCP หรือ Static IP และสแกน Wi‑Fi
- Ethernet LAN8720 (RMII) หรือ W5500 (SPI)
- สถานะ LAN ปัจจุบันและ DHCP lease บนหน้าเว็บ
- MQTT ผ่าน network ที่พร้อมใช้งาน
- Serial, Modbus TCP / RTU และ CAN Bus sections
- OTA firmware, SPIFFS update และ file manager (เปิด/ปิดได้)

## ข้อกำหนด

- Arduino-ESP32 core 3.x
- `ArduinoJson`
- `PubSubClient`

ติดตั้งไลบรารีนี้ไว้ใน Arduino `libraries` directory แล้วเปิด Arduino IDE ใหม่ หรือเพิ่มเป็น local library ของ PlatformIO

## เริ่มต้นใช้งาน

ตัวอย่างพื้นฐานด้านล่างเหมาะกับ ESP32-S3: ปิด config button ไว้ก่อน เพราะ GPIO26 อาจเป็นขา Flash/PSRAM บนบางบอร์ด

```cpp
#define ESP32WC_FEATURE_CONFIG_BUTTON 0

#include <ESP32WebConfigApp.h>

ESP32WebConfigApp app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
```

หลังบูต อุปกรณ์จะเปิด AP ชื่อ `ESP32_MQTT_Config` แล้วเข้าหน้าเว็บที่ `192.168.4.1`

| ค่าเริ่มต้น | ค่า |
| --- | --- |
| AP SSID | `ESP32_MQTT_Config` |
| AP password | `12345678` |
| Username | `admin` |
| Password | `admin123` |
| Status LED | GPIO 2, active HIGH |

เปลี่ยน credentials ก่อนใช้งานจริงเสมอ

## ตั้งค่าแอปเอง

กำหนดชื่ออุปกรณ์, credentials, LED และปุ่มได้ผ่าน `ESP32WebConfigAppSettings`

```cpp
#define ESP32WC_FEATURE_CONFIG_BUTTON 1

#include <ESP32WebConfigApp.h>

ESP32WebConfigAppSettings settings;
ESP32WebConfigApp* app = nullptr;

void setup() {
  settings.apSsid = "MyESP32Config";
  settings.apPass = "change-me";
  settings.username = "admin";
  settings.password = "change-me";
  settings.deviceName = "ESP32_Controller";
  settings.firmwareVersion = "1.0.0";

  // เลือก GPIO ที่ปลอดภัยตามบอร์ดของคุณ
  settings.configButtonPin = 0;
  settings.configButtonUsePullup = true;
  settings.statusLedPin = 2;
  settings.statusLedActiveLevel = HIGH;

  static ESP32WebConfigApp appInstance(settings);
  app = &appInstance;
  app->setup();
}

void loop() {
  app->loop();
}
```

> ห้ามใช้ GPIO26 เป็นปุ่มบน ESP32-S3 ที่ใช้ OPI Flash/PSRAM เพราะอาจทำให้เครื่อง reset วน

## Ethernet

Ethernet เริ่มทำงานแบบ asynchronous: ระบบจะรอ `ETH.hasIP()` ก่อนถือว่า LAN พร้อมใช้งาน จึงไม่บันทึกค่า `0.0.0.0` ระหว่าง DHCP กำลังทำงาน

### W5500 ผ่าน SPI

กำหนด macros ก่อน include header:

```cpp
#define ESP32WC_SECTION_LAN 1
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_ETHERNET_TYPE ESP32WC_ETHERNET_W5500

#define ESP32WC_W5500_SCK_PIN  18
#define ESP32WC_W5500_MISO_PIN 19
#define ESP32WC_W5500_MOSI_PIN 23
#define ESP32WC_W5500_CS_PIN   5
#define ESP32WC_W5500_INT_PIN  -1
#define ESP32WC_W5500_RST_PIN  -1
#define ESP32WC_W5500_SPI_FREQ_MHZ 20

#include <ESP32WebConfigApp.h>
```

ภายในไลบรารีใช้ API ปัจจุบันของ Arduino-ESP32:

```cpp
SPI.begin(SCK, MISO, MOSI);
ETH.begin(ETH_PHY_W5500, phyAddr, cs, irq, rst, SPI, spiFreq);
```

### ESP32-S3-ETH พร้อม W5500

สำหรับบอร์ดที่ใช้ W5500 onboard ตาม pinout นี้:

| Signal | GPIO |
| --- | --- |
| MOSI | 11 |
| MISO | 12 |
| SCK | 13 |
| CS | 14 |
| INT | 10 |
| RST | 9 |

ใช้ [examples/ethernet_w5500_esp32s3_eth/ethernet_w5500_esp32s3_eth.ino](examples/ethernet_w5500_esp32s3_eth/ethernet_w5500_esp32s3_eth.ino)

### LAN8720 ผ่าน RMII

LAN8720 รองรับเฉพาะ ESP32 target ที่มี RMII EMAC (เช่น ESP32 รุ่นดั้งเดิม) ไม่รองรับ ESP32-S2/S3/C3/C6 หากใช้ target ที่ไม่มี EMAC ระบบจะข้าม LAN8720 และ fallback ไป Wi‑Fi อย่างปลอดภัย

ค่าเริ่มต้น LAN8720:

| Signal | GPIO |
| --- | --- |
| MDC | 23 |
| MDIO | 18 |
| Clock | GPIO0 input |

ปรับได้ด้วย `ESP32WC_ETH_PHY_TYPE`, `ESP32WC_ETH_PHY_ADDR`, `ESP32WC_ETH_MDC_PIN`, `ESP32WC_ETH_MDIO_PIN`, `ESP32WC_ETH_POWER_PIN` และ `ESP32WC_ETH_CLOCK_MODE`

## Modbus TCP runtime

Modbus TCP client อยู่ใน `src/ModbusTcpRuntime.h` และทำงานอัตโนมัติเมื่อเปิด `TCP Client` ในหน้าเว็บ ไม่ต้องเพิ่ม loop สำหรับ Modbus ใน sketch ของคุณ

- เลือก Ethernet ก่อนเมื่อ `ETH.hasIP()` เป็นจริง
- fallback ไป Wi‑Fi เมื่อ LAN ยังไม่พร้อม
- อ่าน Holding Registers ด้วย function code `0x03`
- ตั้งค่า Server IP/Port, Unit ID, Start Register, จำนวน registers และช่วง poll ได้จากหน้าเว็บ
- ค่าเริ่มต้น: Unit ID `1`, start register `0`, จำนวน `2` registers และ poll ทุก `1000 ms`

ค่าจากรอบที่อ่านสำเร็จล่าสุดนำไปใช้ใน sketch ได้ทันที โดย index `0` คือ start register ที่ตั้งไว้:

```cpp
uint16_t value;
if (modbusTcpReadRegister(0, value)) {
  // ใช้งาน value ได้ที่นี่
}
```

ใช้ `modbusTcpHasData()` เพื่อตรวจสอบว่ามีข้อมูลหรือไม่ และ `modbusTcpRegisterCount()` เพื่อดูจำนวนค่าที่พร้อมใช้งาน โดยกำหนดจำนวน register ได้สูงสุด `16` ค่า (เปลี่ยนได้ด้วย `ESP32WC_MODBUS_TCP_MAX_REGISTERS` ก่อน include `ESP32WebConfigApp.h`)

## Feature flags

กำหนดก่อน include `ESP32WebConfigApp.h`

```cpp
#define ESP32WC_FEATURE_CONFIG_BUTTON 0
#define ESP32WC_FEATURE_FILE_MANAGER 0
#define ESP32WC_FEATURE_SPIFFS_UPDATE 0
#define ESP32WC_FEATURE_WIFI_SCAN 0
#define ESP32WC_FEATURE_OTA 0

#define ESP32WC_SECTION_LAN 0
#define ESP32WC_SECTION_CANBUS 0
```

### Features

`ESP32WC_FEATURE_CONFIG_BUTTON`, `ESP32WC_FEATURE_STATUS_LED`, `ESP32WC_FEATURE_SERIAL_LOG`, `ESP32WC_FEATURE_PRINT_CONFIG`, `ESP32WC_FEATURE_WIFI_SCAN`, `ESP32WC_FEATURE_OTA`, `ESP32WC_FEATURE_SPIFFS_UPDATE`, `ESP32WC_FEATURE_FILE_MANAGER`

### UI sections

`ESP32WC_SECTION_DEVICE_INFO`, `ESP32WC_SECTION_MQTT`, `ESP32WC_SECTION_LAN`, `ESP32WC_SECTION_WLAN`, `ESP32WC_SECTION_SERIAL`, `ESP32WC_SECTION_MODBUS`, `ESP32WC_SECTION_CANBUS`

## Web routes

| Route | หน้าที่ |
| --- | --- |
| `/` | หน้า config หลัก |
| `/login`, `/logout` | จัดการ session |
| `/load`, `/save` | โหลดและบันทึก config |
| `/network-status` | สถานะ LAN ปัจจุบัน |
| `/scan_wifi` | สแกน Wi‑Fi (เมื่อเปิด feature) |
| `/update` | OTA firmware (เมื่อเปิด feature) |
| `/update-spiffs` | อัปเดต SPIFFS (เมื่อเปิด feature) |
| `/file-manager` | จัดการไฟล์ (เมื่อเปิด feature) |

## Examples

| Example | ใช้สำหรับ |
| --- | --- |
| [esp32webconfig](examples/esp32webconfig/esp32webconfig.ino) | เริ่มต้นแบบปลอดภัยบน ESP32-S3 |
| [custom_settings](examples/custom_settings/custom_settings.ino) | กำหนด credentials, pin และ timing |
| [custom_button_led](examples/custom_button_led/custom_button_led.ino) | ปุ่มและ LED แบบกำหนดเอง |
| [minimal_features](examples/minimal_features/minimal_features.ino) | ลด feature เพื่อใช้ flash น้อยลง |
| [ethernet_lan_enabled](examples/ethernet_lan_enabled/ethernet_lan_enabled.ino) | LAN8720 บน ESP32 ที่มี EMAC |
| [ethernet_w5500_enabled](examples/ethernet_w5500_enabled/ethernet_w5500_enabled.ino) | W5500 pinout แบบ ESP32 DevKit |
| [ethernet_w5500_esp32s3_eth](examples/ethernet_w5500_esp32s3_eth/ethernet_w5500_esp32s3_eth.ino) | W5500 onboard บน ESP32-S3-ETH |
| [mqtt_serial_focus](examples/mqtt_serial_focus/mqtt_serial_focus.ino) | MQTT, Wi‑Fi, Serial และ Modbus |
| [serial_runtime](examples/serial_runtime/serial_runtime.ino) | ใช้ Serial2 ตามค่าบนเว็บ |
| [modbus_rtu_runtime](examples/modbus_rtu_runtime/modbus_rtu_runtime.ino) | Modbus RTU ผ่าน RS485 |
| [modbus_tcp_runtime](examples/modbus_tcp_runtime/modbus_tcp_runtime.ino) | Modbus TCP โดยใช้ LAN ก่อน แล้ว fallback ไป Wi‑Fi |
| [canbus_twai_runtime](examples/canbus_twai_runtime/canbus_twai_runtime.ino) | CAN Bus/TWAI |

## โครงสร้างข้อมูล

ค่าปัจจุบันอยู่ใน global `Config`:

```cpp
Config.device
Config.mqtt
Config.lan
Config.wlan
Config.serial
Config.modbus
Config.canbus
```

ตัวอย่าง runtime สามารถอ่านค่าเหล่านี้หลังเรียก `app.setup()` เพื่อใช้กับ protocol หรือ I/O ของตัวเองได้
