# ogr-node-hal-esp32-arduino

ESP32 hardware abstraction layer for [ogr-node-core](https://github.com/vvapnik/ogr-node-core), providing:

- `OgrEsp32NodeTransport` — I2C slave transport, on top of Arduino's `Wire`
- `OgrEsp32NodeGpio` — EN_IN / EN_OUT digital I/O

For an ESP32/ESP32-S3/ESP32-C3 acting as an OGR **Node** (I2C slave).

A zoomable map of this package's parts is in
[`ARCHITECTURE.takesix.json`](ARCHITECTURE.takesix.json) (a TakeSix diagram).

## Why this package exists alongside ogr-node-hal-esp32

[`ogr-node-hal-esp32`](https://github.com/vvapnik/ogr-node-hal-esp32) (note:
no `-arduino` suffix) implements the same role on top of ESP-IDF's native
`driver/i2c_slave.h` instead of Arduino's `Wire`. That package's README has
the full story, but the short version: Wire's slave mode was found
unreliable specifically around `SET_ADDR` — the master's very next
transaction (its post-`SET_ADDR` `PROT_VER` verification read) sometimes
got no response, because `Wire.begin()` tears down and recreates a whole
FreeRTOS task on every call, and that recreation doesn't reliably finish
before the master's immediate follow-up arrives.

Real-hardware testing traced this to the actual root cause: the *master*
wasn't giving *any* node time to reprogram its address before verifying —
fine for a bare-metal register write (AVR, CH32V003), not fine for
anything with more reinit overhead. The fix landed on the master side
instead (`OgrMaster` now waits 50ms after `SET_ADDR` before verifying —
see [spec §3.1](https://github.com/vvapnik/open-garden-rack/blob/main/docs/03-enumeration.md)
and `ogr-main`'s `OgrMaster.cpp`), which should make *any* transport's
reinit latency a non-issue, Wire's included.

This package exists to confirm that: same shape as
`ogr-node-hal-ch32v003`/`ogr-node-hal-avr` (Arduino `Wire`, `onReceive`/
`onRequest`), now validated against a real head module running the
50ms-settle-delay fix.

## Pin plan

ESP32/ESP32-S3 I2C pins are fully remappable via the GPIO matrix — there is
no chip-fixed SDA/SCL pair the way CH32V003 has. `OgrEsp32NodeTransport`'s
constructor takes `sdaPin`/`sclPin` explicitly; pick pins free on your
specific board, avoiding strapping pins (`0, 3, 45, 46` on S3) and whatever
pins your board's own peripherals (native USB, PSRAM/flash on octal
modules) already claim.

## Status

Not yet validated on real hardware — that's the next step, now that the
master-side settle-delay fix is in place.

---

Development is assisted by [Claude Code](https://claude.com/claude-code) (Anthropic).
