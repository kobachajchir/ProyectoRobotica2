#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ws_limits.h"

#define WSM_MAX_SESSIONS WS_MAX_SESSIONS
#define WSM_NODE_MIN 0x05u
#define WSM_NODE_MAX 0x0Cu
#define WSM_RATE_BUCKET_COUNT 8u
#define WSM_RESUME_TOKEN_HEX_LEN 32u

typedef enum {
    WSM_SLOT_FREE = 0,
    WSM_SLOT_ACTIVE,
    WSM_SLOT_CLEANUP
} WsmSlotState;

typedef enum {
    WSM_OK = 0,
    WSM_INVALID,
    WSM_NOT_FOUND,
    WSM_ALREADY_EXISTS,
    WSM_CAPACITY,
    WSM_STALE_GENERATION,
    WSM_BAD_STATE
} WsmResult;

typedef enum {
    WSM_RATE_KEY_IP = 1,
    WSM_RATE_KEY_CLIENT = 2
} WsmRateKeyType;

typedef struct {
    WsmSlotState state;
    uint32_t client_id;
    uint32_t ip_key;
    uint32_t generation;
    uint8_t node;
    uint8_t stm_authenticated;
    uint8_t esp_debug_used; // Cleanup stays local after a debug logout.
    uint8_t esp_debug_wifi; // Local Wi-Fi permission; never grants F4 authority.
    uint32_t stm_ttl_ms;
    uint32_t stm_expires_at_ms;
    uint32_t resume_until_ms;
    char resume_token[WSM_RESUME_TOKEN_HEX_LEN + 1u];
} WsmSession;

typedef struct {
    uint8_t used;
    uint8_t key_type;
    uint32_t key;
    uint32_t window_started_ms;
    uint16_t attempts;
} WsmRateBucket;

typedef struct {
    uint32_t opened;
    uint32_t capacity_rejections;
    uint32_t cleanup_started;
    uint32_t released;
    uint32_t stale_generation_rejections;
    uint32_t rate_limited;
    uint32_t stm_auth_granted;
    uint32_t stm_auth_revoked;
    uint32_t stm_auth_expired;
    uint32_t stm_auth_refreshed;
    uint32_t reconnect_suspended;
    uint32_t reconnect_resumed;
} WsmMetrics;

typedef struct {
    WsmSession sessions[WSM_MAX_SESSIONS];
    WsmRateBucket rate_buckets[WSM_RATE_BUCKET_COUNT];
    uint32_t next_generation;
    uint32_t rate_window_ms;
    uint16_t rate_max_attempts;
    WsmMetrics metrics;
} WsSessionManager;

void wsm_init(WsSessionManager *manager,
              uint32_t rate_window_ms,
              uint16_t rate_max_attempts);

WsmResult wsm_open(WsSessionManager *manager,
                   uint32_t client_id,
                   uint32_t ip_key,
                   WsmSession **out_session);

WsmSession *wsm_find(WsSessionManager *manager,
                     uint32_t client_id,
                     uint32_t generation);

const WsmSession *wsm_find_const(const WsSessionManager *manager,
                                 uint32_t client_id,
                                 uint32_t generation);

WsmSession *wsm_find_by_node(WsSessionManager *manager, uint8_t node);

WsmResult wsm_set_stm_auth(WsSessionManager *manager,
                           uint32_t client_id,
                           uint32_t generation,
                           uint8_t authenticated,
                           uint32_t ttl_ms,
                           uint32_t now_ms);

/* Renueva exclusivamente una sesion STM ya activa. Si el TTL vencio, la
 * revoca y nunca la revive a partir de trafico tardio. */
WsmResult wsm_touch_stm_auth(WsSessionManager *manager,
                             uint32_t client_id,
                             uint32_t generation,
                             uint32_t now_ms);

WsmResult wsm_begin_cleanup(WsSessionManager *manager,
                            uint32_t client_id,
                            uint32_t generation);

/* Conserva temporalmente la autoridad STM para tolerar cortes breves del
 * radio (por ejemplo, un scan iniciado mientras el ESP opera como SoftAP).
 * La sesion queda inutilizable hasta que su cliente presenta su token propio. */
WsmResult wsm_suspend_for_reconnect(WsSessionManager *manager,
                                    uint32_t client_id,
                                    uint32_t generation,
                                    uint32_t now_ms,
                                    uint32_t grace_ms);

/* Legacy helper; production reconnection uses wsm_resume_by_token.
 * Reanuda solamente cuando existe un unico candidato suspendido para la IP.
 * Se asigna una generacion nueva para que requests de la conexion anterior
 * no puedan completar sobre el WebSocket nuevo. */
WsmResult wsm_resume_by_ip(WsSessionManager *manager,
                           uint32_t new_client_id,
                           uint32_t ip_key,
                           uint32_t now_ms,
                           WsmSession **out_session);

/* A token belongs to one connection, including peers sharing an IP.
 * A fresh generation rejects delayed replies from the previous socket. */
WsmResult wsm_resume_by_token(WsSessionManager *manager,
                              uint32_t new_client_id,
                              const char *resume_token,
                              uint32_t now_ms,
                              WsmSession **out_session);

WsmResult wsm_release(WsSessionManager *manager,
                      uint32_t client_id,
                      uint32_t generation);

uint8_t wsm_allow_auth_attempt(WsSessionManager *manager,
                               uint32_t client_id,
                               uint32_t generation,
                               uint32_t now_ms);

uint8_t wsm_is_authorized(const WsSessionManager *manager,
                          uint32_t client_id,
                          uint32_t generation);

uint8_t wsm_service(WsSessionManager *manager, uint32_t now_ms);
const WsmMetrics *wsm_metrics(const WsSessionManager *manager);

#ifdef __cplusplus
}
#endif
