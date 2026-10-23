#include <Arduino.h>
#include "config.h"
#include "nvs_utils.h"
#include "wifi_utils.h"
#include "sensors.h"
#include "webserver_ws.h"
#include "timebase.h"
#include "globals.h"
#include "esp_system.h"
#include "app_bindings.h"
#include "ws_json_bridge.h"
#include "uner_app_handlers.h"
#include "boot_mode.h"
#include "wifi_status_led.h"


UNER_App g_unerApp;

static WifiStatusLedMode wifiStatusLedModeFor(WifiServiceState state)
{
  if (state == WIFI_SERVICE_AP_RUNNING)
    return WIFI_STATUS_LED_AP;
  if (state == WIFI_SERVICE_STA_CONNECTED)
    return WIFI_STATUS_LED_STA;
  return WIFI_STATUS_LED_INACTIVE;
}

void setup()
{
  AppConfig cfg{};
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(50);
//   Serial.println("\n[BOOT] ESP8266 ESP-01s");

  Timebase_Init10ms();

  if (!NVS_InitAndLoad(&cfg))
  {
//     Serial.println("[BOOT] Config inválida; se forzaron defaults");
  }

  g_espSystem.preferences.cached = NVS_GetCached();

  // GPIO2 is sampled during ESP8266 boot. Only take ownership after setup has
  // started successfully, keeping the active-low LED off until Wi-Fi is ready.
  WifiStatusLed_Begin();

  // El modo persistido representa la seleccion vigente, no un disparador de
  // reinicio. Arrancar nunca vuelve a escribirlo ni reinicia otra vez: solo un
  // nuevo REBOOT_ESP 0x00/0x01 cambia la seleccion.
  WifiRunMode wifi_mode = BootMode_GetRequested(&cfg) == APP_BOOT_MODE_AP_ONLY
                              ? WIFI_RUN_AP_ONLY
                              : WIFI_RUN_AP_STA;

  WifiUtils_Begin(cfg, wifi_mode);
  WifiStatusLed_Update(wifiStatusLedModeFor(WifiUtils_GetState()), millis());

  Sensors_Init();
  AppBindings_Configure(&g_unerApp);
  UNER_App_Init(&g_unerApp, UNER_NODE_WEB_APP, UNER_NODE_MCU);
  g_espSystem.handles.uner_app = &g_unerApp;
  WebServerWS_SetUNERApp(&g_unerApp);
  WebServerWS_Init();
  g_espSystem.handles.json_bridge = WSJsonBridge_GetContext();

//   Serial.println("[BOOT] Listo.");
}

void loop()
{
  Timebase_Update(); // ticks + timers + flags

  WifiUtils_Loop();
  WifiStatusLed_Update(wifiStatusLedModeFor(WifiUtils_GetState()), millis());
  WebServerWS_Loop();
  UNER_App_Loop(&g_unerApp);

  (void)Timebase_ConsumeTick();

  delay(0); // ceder al WDT/scheduler
}
