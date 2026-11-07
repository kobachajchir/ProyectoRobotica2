/* AFR2-20261107-38. Red STA por defecto del firmware.
 * Los valores reales viven en wifi_defaults.local.h (ignorado por Git: *.local.*), para que no se
 * publiquen en el repositorio. Copiar wifi_defaults.example.h a wifi_defaults.local.h y editar.
 * Sin ese archivo no hay red por defecto y el comportamiento es el anterior. */
#pragma once
#if __has_include("wifi_defaults.local.h")
#include "wifi_defaults.local.h"
#endif
#ifndef AFR2_DEFAULT_STA_SSID
#define AFR2_DEFAULT_STA_SSID ""
#endif
#ifndef AFR2_DEFAULT_STA_PASSWORD
#define AFR2_DEFAULT_STA_PASSWORD ""
#endif
