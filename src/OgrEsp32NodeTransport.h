// I2C-slave transport for ESP32/ESP32-S3/ESP32-C3, built on Arduino's Wire.
//
// Sibling package to ogr-node-hal-esp32 (which uses ESP-IDF's native
// driver/i2c_slave.h directly instead). This one exists to answer a
// specific question: real-hardware testing originally found Arduino's
// Wire slave mode unreliable right around SET_ADDR — the master's
// post-SET_ADDR ProtVer verification read (issued essentially immediately
// after SET_ADDR's own bus transaction) sometimes got no response, because
// Wire.begin() tears down and recreates a whole FreeRTOS task on every
// call. That finding led to fixing the *actual* root cause on the master
// side instead: OgrMaster now waits 50ms after SET_ADDR before verifying
// (spec §3.1 — see ogr-main's OgrMaster.cpp). With that master-side fix in
// place, this straightforward Wire-based transport should now work fine
// too — this package exists to confirm that on real hardware.
//
// Namespace/class deliberately do NOT match ogr-main's own OgrEsp32Transport
// (namespace ogr_hal_esp32, implementing ogr_main::IOgrMasterTransport) —
// that's a same-named but unrelated class in a different repo, for the
// opposite bus role (I2C master).
#pragma once

#include <stdint.h>
#include <Wire.h>
#include "OgrTransport.h"

namespace ogr_hal_esp32_node {

class OgrEsp32NodeTransport : public ogr::IOgrTransport {
public:
  // `wire`: Wire or Wire1. `sdaPin`/`sclPin`: required — ESP32 I2C pins are
  // fully remappable via the GPIO matrix, so there's no chip-fixed default
  // the way CH32V003's PC1/PC2 are.
  OgrEsp32NodeTransport(TwoWire &wire, int sdaPin, int sclPin) : wire_(wire), sdaPin_(sdaPin), sclPin_(sclPin) {}

  void begin(ogr::IOgrProtocolSink &sink, uint8_t address) override;
  void setAddress(uint8_t address) override;
  void end() override;
  void poll() override;

private:
  void startSlave(uint8_t address);

  static void onReceiveTrampoline(int numBytes);
  static void onRequestTrampoline();

  static OgrEsp32NodeTransport *self_;
  TwoWire &wire_;
  int sdaPin_;
  int sclPin_;
  ogr::IOgrProtocolSink *sink_ = nullptr;
  uint8_t lastReg_ = 0;
};

} // namespace ogr_hal_esp32_node
