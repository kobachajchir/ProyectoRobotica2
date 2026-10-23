#include "ws_bridge/private_context.h"

// Ordered parts of one library: preserve static linkage and the existing flow.
#include "ws_bridge/common.inc"
#include "ws_bridge/validation.inc"
#include "ws_bridge/oled_canvas.inc"
#include "ws_bridge/requests.inc"
#include "ws_bridge/esp_commands.inc"
#include "ws_bridge/stm_commands.inc"
#include "ws_bridge/debug_wifi.inc"
#include "ws_bridge/dispatch.inc"
#include "ws_bridge/reports.inc"
#include "ws_bridge/services.inc"
#include "ws_bridge/lifecycle.inc"
