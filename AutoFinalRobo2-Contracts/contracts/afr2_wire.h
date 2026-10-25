/* AutoFinalRobo2, AFR2-20261023-15. ROBO2 shared C/C++ wire.
 * UNER F4 parser/framing concepts; authors and sources preserved in reference/
 * and provenance/files.json. Incompatible with UNER and AFR2 profile 1.
 */
#ifndef AFR2_WIRE_H
#define AFR2_WIRE_H
#include <stdint.h>
#include <string.h>
#define AFR2_WIRE_PROFILE 2u
#define AFR2_WIRE_VERSION 1u
#define AFR2_WIRE_TOKEN_A 0x3au
#define AFR2_WIRE_TOKEN_B 0xa5u
#define AFR2_WIRE_PAYLOAD_MAX 64u
#define AFR2_WIRE_OVERHEAD 12u
#define AFR2_WIRE_FRAME_MAX (AFR2_WIRE_OVERHEAD + AFR2_WIRE_PAYLOAD_MAX)
#define AFR2_WIRE_GAP_MS 50u
#define AFR2_NODE_MEGA 1u
#define AFR2_NODE_ESP 3u
#define AFR2_CMD_HELLO 0x01u
#define AFR2_CMD_ALIVE 0x02u
#define AFR2_CMD_STATE 0x03u
#define AFR2_CMD_SENSORS 0x04u
#define AFR2_CMD_ENCODERS 0x05u
#define AFR2_CMD_STOP 0x06u
#define AFR2_RESULT_OK 0u
#define AFR2_RESULT_BAD_REQUEST 1u
#define AFR2_RESULT_UNAVAILABLE 2u
#define AFR2_RESULT_SESSION 3u
#define AFR2_RESULT_INTERNAL 4u
#define AFR2_CAP_UART 0x0001u
#define AFR2_CAP_STATE 0x0002u
#define AFR2_CAP_LINE 0x0004u
#define AFR2_CAP_ULTRASONIC 0x0008u
#define AFR2_CAP_ENCODERS 0x0010u
#define AFR2_CAP_STOP 0x0020u
#define AFR2_CAP_MASK 0x003fu
#define AFR2_AGE_UNKNOWN UINT32_MAX
/* High nibble = sender, low nibble = receiver, exactly as in F4. */
#define AFR2_ROUTE(src,dst) ((uint8_t)((((src)&15u)<<4)|((dst)&15u)))
#define AFR2_ROUTE_REQUEST AFR2_ROUTE(AFR2_NODE_ESP,AFR2_NODE_MEGA)
#define AFR2_ROUTE_RESPONSE AFR2_ROUTE(AFR2_NODE_MEGA,AFR2_NODE_ESP)
static inline uint8_t afr2_route_source(uint8_t r) { return r>>4; }
static inline uint8_t afr2_route_destination(uint8_t r) { return r&15u; }
static inline uint8_t afr2_route_valid(uint8_t r) {
    return r==AFR2_ROUTE_REQUEST || r==AFR2_ROUTE_RESPONSE;
}
static inline uint8_t afr2_route_matches(uint8_t r,uint8_t sender,uint8_t receiver) {
    return afr2_route_source(r)==sender && afr2_route_destination(r)==receiver;
}
static inline uint8_t afr2_wire_response_length(uint8_t command) {
    switch (command) {
    case AFR2_CMD_HELLO: return 23;
    case AFR2_CMD_ALIVE: return 13;
    case AFR2_CMD_STATE: return 16;
    case AFR2_CMD_SENSORS: return 22;
    case AFR2_CMD_ENCODERS: return 31;
    case AFR2_CMD_STOP: return 11;
    default: return 0;
    }
}
static inline uint16_t afr2_get_u16(const uint8_t *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1]<<8);
}
static inline uint32_t afr2_get_u32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}
static inline void afr2_put_u16(uint8_t *p,uint16_t n) {
    p[0]=(uint8_t)n; p[1]=(uint8_t)(n>>8);
}
static inline void afr2_put_u32(uint8_t *p,uint32_t n) {
    p[0]=(uint8_t)n; p[1]=(uint8_t)(n>>8); p[2]=(uint8_t)(n>>16); p[3]=(uint8_t)(n>>24);
}
/* Local saturating diagnostics; never logs on the protocol UART. */
typedef struct {
    uint16_t frames, header, length, token, version, route, checksum, gap;
} Afr2WireStats;
typedef struct {
    uint8_t state, length, route, command, checksum, drop;
    uint16_t position;
    uint8_t payload[AFR2_WIRE_PAYLOAD_MAX];
    Afr2WireStats stats;
} Afr2WireParser;
static inline void afr2_wire_count(uint16_t *n) { if (*n!=UINT16_MAX) ++*n; }
/* Reset framing only. Zero-initialize the entire parser once at construction. */
static inline void afr2_wire_reset(Afr2WireParser *p) {
    p->state=0; p->position=0; p->checksum=0; p->drop=0;
}
static inline void afr2_wire_gap(Afr2WireParser *p) {
    if (p->state) afr2_wire_count(&p->stats.gap);
    afr2_wire_reset(p);
}
static inline void afr2_wire_resync(Afr2WireParser *p,uint8_t b) {
    afr2_wire_reset(p);
    if (b=='R') { p->state=1; p->checksum=b; }
}
/* Complete frame = 1; consume fields before next push. Oversized/wrong-route
 * frames drain through checksum; headers inside their data are not executed.
 * Caller enforces inter-byte timeout through afr2_wire_gap().
 */
static inline uint8_t afr2_wire_push(Afr2WireParser *p,uint8_t b) {
    switch (p->state) {
    case 0: if (b=='R') { p->checksum=b; p->state=1; } return 0;
    case 1: if (b!='O') goto bad_header; break;
    case 2: if (b!='B') goto bad_header; break;
    case 3: if (b!='O') goto bad_header; break;
    case 4: if (b!='2') goto bad_header; break;
    case 5:
        p->length=b; p->position=0; p->drop=b>AFR2_WIRE_PAYLOAD_MAX;
        if (p->drop) afr2_wire_count(&p->stats.length);
        break;
    case 6: if (b!=AFR2_WIRE_TOKEN_A) goto bad_token; break;
    case 7: if (b!=AFR2_WIRE_TOKEN_B) goto bad_token; break;
    case 8:
        if (b!=AFR2_WIRE_VERSION) {
            afr2_wire_count(&p->stats.version); afr2_wire_resync(p,b); return 0;
        }
        break;
    case 9:
        p->route=b;
        if (!afr2_route_valid(b)) { p->drop=1; afr2_wire_count(&p->stats.route); }
        break;
    case 10:
        p->command=b; p->checksum^=b; p->state=p->length?11:12; return 0;
    case 11:
        if (!p->drop) p->payload[p->position]=b;
        ++p->position; p->checksum^=b;
        if (p->position==p->length) p->state=12;
        return 0;
    case 12:
        if (b!=p->checksum) {
            afr2_wire_count(&p->stats.checksum); afr2_wire_resync(p,b); return 0;
        }
        p->state=0;
        if (p->drop) return 0;
        afr2_wire_count(&p->stats.frames); return 1;
    default: afr2_wire_resync(p,b); return 0;
    }
    p->checksum^=b; ++p->state; return 0;
bad_header:
    afr2_wire_count(&p->stats.header); afr2_wire_resync(p,b); return 0;
bad_token:
    afr2_wire_count(&p->stats.token); afr2_wire_resync(p,b); return 0;
}
static inline uint8_t afr2_wire_build(uint8_t *out,uint16_t capacity,uint8_t route,
                                    uint8_t command,const uint8_t *data,uint8_t n) {
    uint8_t i,sum=0;
    if (!out || n>AFR2_WIRE_PAYLOAD_MAX || capacity<(uint16_t)(n+AFR2_WIRE_OVERHEAD) ||
        (n && !data) || !afr2_route_valid(route)) return 0;
    out[0]='R'; out[1]='O'; out[2]='B'; out[3]='O'; out[4]='2'; out[5]=n;
    out[6]=AFR2_WIRE_TOKEN_A; out[7]=AFR2_WIRE_TOKEN_B;
    out[8]=AFR2_WIRE_VERSION; out[9]=route; out[10]=command;
    if (n) memcpy(out+11,data,n);
    for (i=0;i<(uint8_t)(n+11u);++i) sum^=out[i];
    out[n+11u]=sum; return (uint8_t)(n+AFR2_WIRE_OVERHEAD);
}
#endif
