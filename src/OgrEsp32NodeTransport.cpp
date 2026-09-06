#include "OgrEsp32NodeTransport.h"
#include "OgrTypes.h" // kMaxWriteFrame / kMaxReadFrame

namespace ogr_hal_esp32_node {

namespace {
// Spec §1: bus is fixed at 100 kHz — not something a node should tune.
constexpr uint32_t kBusFreqHz = 100000;
} // namespace

OgrEsp32NodeTransport *OgrEsp32NodeTransport::self_ = nullptr;

void OgrEsp32NodeTransport::begin(ogr::IOgrProtocolSink &sink, uint8_t address) {
  sink_ = &sink;
  self_ = this;
  startSlave(address);
}

void OgrEsp32NodeTransport::setAddress(uint8_t address) {
  // Wire.end()+begin() rather than relying on an in-place address change:
  // TwoWire has no dedicated "just change my slave address" call, and
  // begin() silently no-ops if a slave session is already active (checked
  // via arduino-esp32's own Wire.cpp source before writing this).
  wire_.end();
  startSlave(address);
}

void OgrEsp32NodeTransport::end() { wire_.end(); }

void OgrEsp32NodeTransport::poll() {
  // Fully task/interrupt-driven Wire slave implementation — nothing to
  // pump here.
}

void OgrEsp32NodeTransport::startSlave(uint8_t address) {
  wire_.begin(address, sdaPin_, sclPin_, kBusFreqHz);
  wire_.onReceive(onReceiveTrampoline);
  wire_.onRequest(onRequestTrampoline);
}

void OgrEsp32NodeTransport::onReceiveTrampoline(int numBytes) {
  if (!self_ || numBytes <= 0) {
    return;
  }

  uint8_t buf[ogr::kMaxWriteFrame];
  uint8_t n = 0;
  while (self_->wire_.available() && n < sizeof(buf)) {
    buf[n++] = static_cast<uint8_t>(self_->wire_.read());
  }
  while (self_->wire_.available()) {
    self_->wire_.read(); // defensive: drain anything past our max frame size
  }
  if (n == 0) {
    return;
  }

  self_->lastReg_ = buf[0];

  // A length of exactly 1 is always just the register-select prelude ahead
  // of a read (repeated START) — no writable register on this protocol
  // accepts a 0-byte payload, so it can never be a genuine complete write.
  if (n >= 2) {
    self_->sink_->onI2cWrite(buf, n);
  }
}

void OgrEsp32NodeTransport::onRequestTrampoline() {
  if (!self_) {
    return;
  }
  uint8_t resp[ogr::kMaxReadFrame];
  uint8_t n = self_->sink_->onI2cRead(self_->lastReg_, resp, sizeof(resp));
  self_->wire_.write(resp, n);
}

} // namespace ogr_hal_esp32_node
