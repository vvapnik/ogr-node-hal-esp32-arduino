// Single include for ESP32-as-Node sketches: pulls in the protocol engine
// plus this package's transport/GPIO implementations.
//
// No storage implementation yet (no anchor-role test target has needed
// PLANT_UID persistence so far) — see ogr-node-hal-avr's EEPROM storage for
// the shape a Preferences/NVS-backed one would take if/when needed.
#pragma once

#include <OgrNode.h>
#include "OgrEsp32NodeGpio.h"
#include "OgrEsp32NodeTransport.h"
