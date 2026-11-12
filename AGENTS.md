# AGENTS.md

Guidance for AI agents working in this repo.

Project map: `ARCHITECTURE.takesix.json` — read it first. It shows which
class implements which ogr-node-core interface, with file paths and the
hardware findings behind the design.

## What this is

ESP32 / ESP32-S3 / ESP32-C3 node transport and EN-pin GPIO for ogr-node-core, on Arduino `Wire`. Sibling of `ogr-node-hal-esp32` (native ESP-IDF).

The protocol engine itself lives in
[ogr-node-core](https://github.com/vvapnik/ogr-node-core); the protocol is
specified in [open-garden-rack](https://github.com/vvapnik/open-garden-rack).

## Rules

- Keep everything that runs from the I2C callbacks fast: they run in ISR
  context. Slow work (storage, reconfiguration) belongs in `OgrNode::update()`.
- When files or responsibilities move, update `ARCHITECTURE.takesix.json`
  so its paths still match the code.

## AI assistance

This project is developed with the help of Claude Code (Anthropic).
