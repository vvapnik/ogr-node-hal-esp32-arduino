// EN_IN / EN_OUT via plain Arduino digital I/O — identical across
// ESP32/ESP32-S3/ESP32-C3, no chip-specific branching needed.
#pragma once

#include <Arduino.h>
#include "OgrGpio.h"

namespace ogr_hal_esp32_node {

class OgrEsp32NodeGpio : public ogr::IOgrGpio {
public:
  OgrEsp32NodeGpio(uint8_t enInPin, uint8_t enOutPin) : enInPin_(enInPin), enOutPin_(enOutPin) {}

  void begin() override {
    pinMode(enInPin_, INPUT);
    pinMode(enOutPin_, OUTPUT);
    digitalWrite(enOutPin_, LOW);
  }

  bool readEnIn() override { return digitalRead(enInPin_) == HIGH; }
  void writeEnOut(bool high) override { digitalWrite(enOutPin_, high ? HIGH : LOW); }

private:
  uint8_t enInPin_;
  uint8_t enOutPin_;
};

} // namespace ogr_hal_esp32_node
