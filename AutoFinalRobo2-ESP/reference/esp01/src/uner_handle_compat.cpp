#include "uner_handle.h"
#ifdef ARDUINO
#include "uner_platform_esp.h"
#endif

UNER_Status UNER_Handle_Init(UNER_Handle *h, const UNER_CoreConfig *cfg,
    const UNER_CommandSpec *table, uint8_t table_count, void *app_ctx) {
    UNER_HandleOps ops = {};
#ifdef ARDUINO
    ops.now_ms = EspPlatform_Millis;
#if defined(UNER_BRIDGE_DEBUG) && UNER_BRIDGE_DEBUG
    ops.log_packet = EspPlatform_LogUNERPacket;
#endif
#endif
    return UNER_Handle_InitWithOps(h,cfg,table,table_count,app_ctx,&ops);
}
