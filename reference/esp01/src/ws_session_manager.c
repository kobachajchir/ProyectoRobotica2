#include "ws_session_manager.h"

#include <string.h>

static uint8_t time_reached(uint32_t now_ms, uint32_t deadline_ms)
{
    return (uint8_t)(((int32_t)(now_ms - deadline_ms)) >= 0);
}

static uint8_t generation_matches(const WsmSession *session,
                                  uint32_t client_id,
                                  uint32_t generation)
{
    return (uint8_t)(session &&
                     session->state != WSM_SLOT_FREE &&
                     session->client_id == client_id &&
                     session->generation == generation);
}

static uint32_t allocate_generation(WsSessionManager *manager)
{
    uint32_t generation = manager->next_generation++;
    if (generation == 0u) generation = manager->next_generation++;
    if (manager->next_generation == 0u) manager->next_generation = 1u;
    return generation;
}

static WsmRateBucket *rate_bucket(WsSessionManager *manager,
                                  uint8_t key_type,
                                  uint32_t key,
                                  uint32_t now_ms)
{
    uint8_t i;
    WsmRateBucket *candidate = NULL;
    WsmRateBucket *oldest = NULL;

    for (i = 0u; i < WSM_RATE_BUCKET_COUNT; ++i) {
        WsmRateBucket *bucket = &manager->rate_buckets[i];
        if (bucket->used && bucket->key_type == key_type && bucket->key == key) {
            if (time_reached(now_ms, bucket->window_started_ms + manager->rate_window_ms)) {
                bucket->window_started_ms = now_ms;
                bucket->attempts = 0u;
            }
            return bucket;
        }
        if (!bucket->used && !candidate) candidate = bucket;
        if (bucket->used &&
            time_reached(now_ms, bucket->window_started_ms + manager->rate_window_ms) &&
            !candidate) {
            candidate = bucket;
        }
        if (!oldest || (int32_t)(bucket->window_started_ms - oldest->window_started_ms) < 0) {
            oldest = bucket;
        }
    }

    if (!candidate) candidate = oldest;
    if (!candidate) return NULL;
    memset(candidate, 0, sizeof(*candidate));
    candidate->used = 1u;
    candidate->key_type = key_type;
    candidate->key = key;
    candidate->window_started_ms = now_ms;
    return candidate;
}

void wsm_init(WsSessionManager *manager,
              uint32_t rate_window_ms,
              uint16_t rate_max_attempts)
{
    uint8_t i;
    if (!manager) return;
    memset(manager, 0, sizeof(*manager));
    manager->next_generation = 1u;
    manager->rate_window_ms = rate_window_ms;
    manager->rate_max_attempts = rate_max_attempts;
    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        manager->sessions[i].node = (uint8_t)(WSM_NODE_MIN + i);
    }
}

WsmResult wsm_open(WsSessionManager *manager,
                   uint32_t client_id,
                   uint32_t ip_key,
                   WsmSession **out_session)
{
    uint8_t i;
    if (out_session) *out_session = NULL;
    if (!manager) return WSM_INVALID;

    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        if (manager->sessions[i].state != WSM_SLOT_FREE &&
            manager->sessions[i].client_id == client_id) {
            return WSM_ALREADY_EXISTS;
        }
    }
    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        WsmSession *session = &manager->sessions[i];
        if (session->state == WSM_SLOT_FREE) {
            uint8_t node = (uint8_t)(WSM_NODE_MIN + i);
            memset(session, 0, sizeof(*session));
            session->state = WSM_SLOT_ACTIVE;
            session->client_id = client_id;
            session->ip_key = ip_key;
            session->generation = allocate_generation(manager);
            session->node = node;
            manager->metrics.opened++;
            if (out_session) *out_session = session;
            return WSM_OK;
        }
    }

    manager->metrics.capacity_rejections++;
    return WSM_CAPACITY;
}

WsmSession *wsm_find(WsSessionManager *manager,
                     uint32_t client_id,
                     uint32_t generation)
{
    uint8_t i;
    if (!manager) return NULL;
    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        if (generation_matches(&manager->sessions[i], client_id, generation)) {
            return &manager->sessions[i];
        }
    }
    return NULL;
}

const WsmSession *wsm_find_const(const WsSessionManager *manager,
                                 uint32_t client_id,
                                 uint32_t generation)
{
    uint8_t i;
    if (!manager) return NULL;
    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        if (generation_matches(&manager->sessions[i], client_id, generation)) {
            return &manager->sessions[i];
        }
    }
    return NULL;
}

WsmSession *wsm_find_by_node(WsSessionManager *manager, uint8_t node)
{
    uint8_t index;
    if (!manager || node < WSM_NODE_MIN || node > WSM_NODE_MAX) return NULL;
    index = (uint8_t)(node - WSM_NODE_MIN);
    return manager->sessions[index].state == WSM_SLOT_FREE ? NULL : &manager->sessions[index];
}

WsmResult wsm_set_stm_auth(WsSessionManager *manager,
                           uint32_t client_id,
                           uint32_t generation,
                           uint8_t authenticated,
                           uint32_t ttl_ms,
                           uint32_t now_ms)
{
    WsmSession *session = wsm_find(manager, client_id, generation);
    if (!session) {
        if (manager) manager->metrics.stale_generation_rejections++;
        return WSM_STALE_GENERATION;
    }
    if (session->state != WSM_SLOT_ACTIVE) return WSM_BAD_STATE;

    if (authenticated && ttl_ms > 0u) {
        session->esp_debug_used = 0u;
        session->stm_authenticated = 1u;
        session->stm_ttl_ms = ttl_ms;
        session->stm_expires_at_ms = now_ms + ttl_ms;
        manager->metrics.stm_auth_granted++;
    } else {
        if (session->stm_authenticated) manager->metrics.stm_auth_revoked++;
        session->stm_authenticated = 0u;
        session->stm_ttl_ms = 0u;
        session->stm_expires_at_ms = 0u;
    }
    return WSM_OK;
}

WsmResult wsm_touch_stm_auth(WsSessionManager *manager,
                             uint32_t client_id,
                             uint32_t generation,
                             uint32_t now_ms)
{
    WsmSession *session = wsm_find(manager, client_id, generation);
    if (!session) {
        if (manager) manager->metrics.stale_generation_rejections++;
        return WSM_STALE_GENERATION;
    }
    if (session->state != WSM_SLOT_ACTIVE || !session->stm_authenticated ||
        session->stm_ttl_ms == 0u) {
        return WSM_BAD_STATE;
    }
    if (time_reached(now_ms, session->stm_expires_at_ms)) {
        session->stm_authenticated = 0u;
        session->stm_ttl_ms = 0u;
        session->stm_expires_at_ms = 0u;
        manager->metrics.stm_auth_expired++;
        return WSM_BAD_STATE;
    }
    session->stm_expires_at_ms = now_ms + session->stm_ttl_ms;
    manager->metrics.stm_auth_refreshed++;
    return WSM_OK;
}

WsmResult wsm_begin_cleanup(WsSessionManager *manager,
                            uint32_t client_id,
                            uint32_t generation)
{
    WsmSession *session = wsm_find(manager, client_id, generation);
    if (!session) return WSM_STALE_GENERATION;
    if (session->state != WSM_SLOT_ACTIVE) return WSM_BAD_STATE;
    session->state = WSM_SLOT_CLEANUP;
    session->stm_authenticated = 0u;
    session->stm_ttl_ms = 0u;
    session->stm_expires_at_ms = 0u;
    session->resume_until_ms = 0u;
    manager->metrics.cleanup_started++;
    return WSM_OK;
}

WsmResult wsm_suspend_for_reconnect(WsSessionManager *manager,
                                    uint32_t client_id,
                                    uint32_t generation,
                                    uint32_t now_ms,
                                    uint32_t grace_ms)
{
    WsmSession *session = wsm_find(manager, client_id, generation);
    if (!session) return WSM_STALE_GENERATION;
    if (session->state != WSM_SLOT_ACTIVE || grace_ms == 0u) return WSM_BAD_STATE;

    session->state = WSM_SLOT_CLEANUP;
    session->resume_until_ms = now_ms + grace_ms;
    manager->metrics.cleanup_started++;
    manager->metrics.reconnect_suspended++;
    return WSM_OK;
}

WsmResult wsm_resume_by_ip(WsSessionManager *manager,
                           uint32_t new_client_id,
                           uint32_t ip_key,
                           uint32_t now_ms,
                           WsmSession **out_session)
{
    uint8_t i;
    WsmSession *candidate = NULL;
    uint8_t active_same_peer = 0u;

    if (out_session) *out_session = NULL;
    if (!manager) return WSM_INVALID;

    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        WsmSession *session = &manager->sessions[i];
        if (session->state != WSM_SLOT_FREE && session->client_id == new_client_id) {
            return WSM_ALREADY_EXISTS;
        }
        if (session->state == WSM_SLOT_ACTIVE && session->ip_key == ip_key) {
            active_same_peer = 1u;
        }
        if (session->state != WSM_SLOT_CLEANUP ||
            session->ip_key != ip_key ||
            session->resume_until_ms == 0u ||
            time_reached(now_ms, session->resume_until_ms)) {
            continue;
        }
        /* Dos pestanas detras de la misma IP son ambiguas: no heredamos la
         * autoridad de ninguna y dejamos que ambas completen su logout. */
        if (candidate != NULL) return WSM_BAD_STATE;
        candidate = session;
    }

    /* Una IP puede alojar varias pestanas y, fuera del SoftAP, incluso varios
     * equipos detras de NAT. Si ya hay una sesion activa para ese peer, el
     * nuevo WebSocket debe abrir su propio slot sin heredar autoridad. */
    if (active_same_peer) return WSM_BAD_STATE;
    if (!candidate) return WSM_NOT_FOUND;
    if (candidate->stm_authenticated &&
        time_reached(now_ms, candidate->stm_expires_at_ms)) {
        candidate->stm_authenticated = 0u;
        candidate->stm_ttl_ms = 0u;
        candidate->stm_expires_at_ms = 0u;
        manager->metrics.stm_auth_expired++;
    }

    candidate->state = WSM_SLOT_ACTIVE;
    candidate->client_id = new_client_id;
    candidate->generation = allocate_generation(manager);
    candidate->resume_until_ms = 0u;
    manager->metrics.reconnect_resumed++;
    if (out_session) *out_session = candidate;
    return WSM_OK;
}

WsmResult wsm_resume_by_token(WsSessionManager *manager,
                              uint32_t new_client_id,
                              const char *resume_token,
                              uint32_t now_ms,
                              WsmSession **out_session)
{
    uint8_t i;
    if (out_session) *out_session = NULL;
    if (!manager || !resume_token ||
        strlen(resume_token) != WSM_RESUME_TOKEN_HEX_LEN) return WSM_NOT_FOUND;
    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        WsmSession *session = &manager->sessions[i];
        if (session->state != WSM_SLOT_CLEANUP ||
            session->resume_until_ms == 0u ||
            time_reached(now_ms, session->resume_until_ms) ||
            strcmp(session->resume_token, resume_token) != 0) continue;
        if (session->stm_authenticated && time_reached(now_ms, session->stm_expires_at_ms)) {
            session->stm_authenticated = 0u;
            session->stm_ttl_ms = 0u;
            session->stm_expires_at_ms = 0u;
            manager->metrics.stm_auth_expired++;
        }
        session->state = WSM_SLOT_ACTIVE;
        session->client_id = new_client_id;
        session->generation = allocate_generation(manager);
        session->resume_until_ms = 0u;
        manager->metrics.reconnect_resumed++;
        if (out_session) *out_session = session;
        return WSM_OK;
    }
    return WSM_NOT_FOUND;
}

WsmResult wsm_release(WsSessionManager *manager,
                      uint32_t client_id,
                      uint32_t generation)
{
    WsmSession *session = wsm_find(manager, client_id, generation);
    uint8_t node;
    if (!session) return WSM_STALE_GENERATION;
    if (session->state != WSM_SLOT_CLEANUP) return WSM_BAD_STATE;
    node = session->node;
    memset(session, 0, sizeof(*session));
    session->node = node;
    manager->metrics.released++;
    return WSM_OK;
}

uint8_t wsm_allow_auth_attempt(WsSessionManager *manager,
                               uint32_t client_id,
                               uint32_t generation,
                               uint32_t now_ms)
{
    WsmSession *session = wsm_find(manager, client_id, generation);
    WsmRateBucket *ip_bucket;
    WsmRateBucket *client_bucket;
    if (!manager || !session || session->state != WSM_SLOT_ACTIVE) return 0u;
    if (manager->rate_max_attempts == 0u || manager->rate_window_ms == 0u) return 1u;

    ip_bucket = rate_bucket(manager, WSM_RATE_KEY_IP, session->ip_key, now_ms);
    client_bucket = rate_bucket(manager, WSM_RATE_KEY_CLIENT, client_id, now_ms);
    if (!ip_bucket || !client_bucket ||
        ip_bucket->attempts >= (uint32_t)manager->rate_max_attempts * WS_MAX_SESSIONS ||
        client_bucket->attempts >= manager->rate_max_attempts) {
        manager->metrics.rate_limited++;
        return 0u;
    }

    ip_bucket->attempts++;
    client_bucket->attempts++;
    return 1u;
}

uint8_t wsm_is_authorized(const WsSessionManager *manager,
                          uint32_t client_id,
                          uint32_t generation)
{
    const WsmSession *session = wsm_find_const(manager, client_id, generation);
    return (uint8_t)(session && session->state == WSM_SLOT_ACTIVE &&
                     session->stm_authenticated);
}

uint8_t wsm_service(WsSessionManager *manager, uint32_t now_ms)
{
    uint8_t i;
    uint8_t expired = 0u;
    if (!manager) return 0u;
    for (i = 0u; i < WSM_MAX_SESSIONS; ++i) {
        WsmSession *session = &manager->sessions[i];
        if (session->state != WSM_SLOT_FREE &&
            session->stm_authenticated &&
            time_reached(now_ms, session->stm_expires_at_ms)) {
            session->stm_authenticated = 0u;
            session->stm_ttl_ms = 0u;
            session->stm_expires_at_ms = 0u;
            manager->metrics.stm_auth_expired++;
            expired++;
        }
    }
    return expired;
}

const WsmMetrics *wsm_metrics(const WsSessionManager *manager)
{
    return manager ? &manager->metrics : NULL;
}
