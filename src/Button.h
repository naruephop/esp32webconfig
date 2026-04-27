// Button.h
#pragma once
#include <Arduino.h>

class Button {
public:
  enum Event : uint8_t {
    NONE = 0,
    PRESS,
    RELEASE,
    CLICK_1,
    CLICK_2,
    CLICK_3,
    LONG_1,   // >= long1Ms
    LONG_2,   // >= long2Ms
    LONG_3    // >= long3Ms
  };

  struct Config {
    uint8_t  pin;
    bool     pullup = true;          // true: INPUT_PULLUP, pressed=LOW
    uint32_t debounceMs = 30;

    uint32_t clickGapMs = 350;       // เวลารอรวมคลิก (double/triple)
    uint32_t long1Ms = 2000;         // 2s
    uint32_t long2Ms = 5000;         // 5s
    uint32_t long3Ms = 10000;        // 10s
  };

  explicit Button(const Config& cfg) : c(cfg) {}

  void begin() {
    pinMode(c.pin, c.pullup ? INPUT_PULLUP : INPUT);
    _lastRead = readRaw();
    _stable = _lastRead;
    _prevStable = _stable;
    _lastChangeMs = millis();
  }

  // เรียกใน loop() บ่อย ๆ
  Event update(uint32_t now = millis()) {
    Event ev = NONE;

    // --- Debounce ---
    bool r = readRaw();
    if (r != _lastRead) {
      _lastRead = r;
      _lastChangeMs = now;
    }
    if (now - _lastChangeMs >= c.debounceMs) {
      _stable = _lastRead;
    }

    // --- Edge detect (press/release) ---
    if (_stable != _prevStable) {
      _prevStable = _stable;

      if (isPressed()) {
        _pressStartMs = now;
        _longFired1 = _longFired2 = _longFired3 = false;
        ev = PRESS;
      } else {
        ev = RELEASE;

        // ถ้าปล่อยก่อน long1 => นับเป็น click
        if (!_longFired1) {
          _clickCount++;
          _clickDeadlineMs = now + c.clickGapMs;
        } else {
          // ถ้าเคย long แล้ว ปล่อย ไม่ต้องนับเป็น click
          _clickCount = 0;
          _clickDeadlineMs = 0;
        }
      }
    }

    // --- Long press levels (ยิงครั้งเดียวต่อระดับ) ---
    if (isPressed()) {
      uint32_t held = now - _pressStartMs;

      if (!_longFired1 && held >= c.long1Ms) {
        _longFired1 = true;
        ev = LONG_1;
      }
      if (!_longFired2 && held >= c.long2Ms) {
        _longFired2 = true;
        ev = LONG_2;
      }
      if (!_longFired3 && held >= c.long3Ms) {
        _longFired3 = true;
        ev = LONG_3;
      }

      // ถ้ากดค้างแล้ว (เลย long1) เราไม่ควรรอ click อีก
      if (_longFired1) {
        _clickCount = 0;
        _clickDeadlineMs = 0;
      }
    }

    // --- Resolve click (1/2/3) เมื่อหมดเวลารอรวมคลิก ---
    if (!isPressed() && _clickDeadlineMs != 0 && now >= _clickDeadlineMs) {
      if (_clickCount == 1) ev = CLICK_1;
      else if (_clickCount == 2) ev = CLICK_2;
      else if (_clickCount >= 3) ev = CLICK_3;

      _clickCount = 0;
      _clickDeadlineMs = 0;
    }

    return ev;
  }

  bool pressed() const { return isPressed(); } // สถานะปุ่ม ณ ตอนนั้น

private:
  Config c;

  // สถานะ debounce
  bool _lastRead = true;
  bool _stable = true;
  bool _prevStable = true;
  uint32_t _lastChangeMs = 0;

  // long press
  uint32_t _pressStartMs = 0;
  bool _longFired1 = false;
  bool _longFired2 = false;
  bool _longFired3 = false;

  // click
  uint8_t _clickCount = 0;
  uint32_t _clickDeadlineMs = 0;

  bool readRaw() const {
    return digitalRead(c.pin);
  }

  bool isPressed() const {
    // pullup: pressed=LOW
    return c.pullup ? (_stable == LOW) : (_stable == HIGH);
  }
};
