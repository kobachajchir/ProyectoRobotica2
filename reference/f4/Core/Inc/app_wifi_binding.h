#ifndef APP_WIFI_BINDING_H
#define APP_WIFI_BINDING_H
#include "wifi_bridge.h"
extern WifiBridge_Handle wifiBridge;
extern WifiBridge_Credentials wifiBridgeCredentials;
#define WifiBridge_SetRuntimeCredentials(...) WifiBridge_SetRuntimeCredentials_H(&wifiBridge, __VA_ARGS__)
#define WifiBridge_SetRuntimeSsid(...) WifiBridge_SetRuntimeSsid_H(&wifiBridge, __VA_ARGS__)
#define WifiBridge_SetRuntimePassword(...) WifiBridge_SetRuntimePassword_H(&wifiBridge, __VA_ARGS__)
#define WifiBridge_GetRuntimeCredentials() WifiBridge_GetRuntimeCredentials_H(&wifiBridge)
#endif
