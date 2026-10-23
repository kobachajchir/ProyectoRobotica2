#ifndef INC_APP_AP_CONFIG_H_
#define INC_APP_AP_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "app_ap_config_data.h"
void AppApConfig_Init(void);
void AppApConfig_Get(AppApConfig_t *out);
AppApStatus_t AppApConfig_ApplyPatch(const AppApConfigPatch_t *patch,
                                     uint8_t *changedMask);
AppApStatus_t AppApConfig_ParseIpv4(const char *text, uint8_t out[4]);
bool AppApConfig_Ipv4IsValid(const uint8_t ipv4[4]);
void AppApConfig_Task(void);
void AppApConfig_OnEspApplyResponse(uint8_t status);
void AppApConfig_RequestRetry(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_APP_AP_CONFIG_H_ */
