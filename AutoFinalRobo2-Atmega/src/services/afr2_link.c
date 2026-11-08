#include "afr2_product_wire.h"
/* AFR2-20261023-15. ROBO2 endpoint, derived from UNER separation of
 * parser/transport/handlers. No hardware access and no historical F4 catalogue.
 */
#include "afr2_link.h"
#include <string.h>
void Afr2Link_Init(Afr2Link *l,const Afr2LinkOps *ops) {
    memset(l,0,sizeof(*l)); if (ops) l->ops=*ops;
}
/* AFR2-20261108-01. Diagnostics only; no effect on protocol state. */
static void notify(Afr2Link *l,uint8_t kind,uint8_t command,uint8_t result,const uint8_t *data,uint8_t length,uint32_t value) {
    if(!l->ops.event)return;
    Afr2LinkEvent e={kind,command,result,length,data,value};
    l->ops.event(l->ops.ctx,&e);
}
void Afr2Link_RxFault(Afr2Link *l) {
    ++l->faults;notify(l,AFR2_LINK_EVT_RX_FAULT,0,0,0,0,0);
    afr2_wire_reset(&l->parser); l->negotiated=0; l->reply_length=0;
    if(l->ops.halt_motion)l->ops.halt_motion(l->ops.ctx);else if(l->ops.stop)l->ops.stop(l->ops.ctx);
    if (l->ops.revoke) l->ops.revoke(l->ops.ctx);
}
void Afr2Link_Tick(Afr2Link *l,uint32_t now) {
    if (l->parser.state && (uint32_t)(now-l->last_rx_ms)>AFR2_WIRE_GAP_MS)
        afr2_wire_gap(&l->parser);
    if (l->negotiated && (uint32_t)(now-l->last_request_ms)>1500u) {
        notify(l,AFR2_LINK_EVT_SUPERVISION,0,0,0,0,(uint32_t)(now-l->last_request_ms));
        l->negotiated=0; l->supervised=1;
        if(l->ops.halt_motion)l->ops.halt_motion(l->ops.ctx);else if(l->ops.stop)l->ops.stop(l->ops.ctx);
        if (l->ops.revoke) l->ops.revoke(l->ops.ctx);
    }
}
static uint16_t capabilities(const Afr2Link *l) {
    uint16_t declared=l->ops.current_capabilities?l->ops.current_capabilities(l->ops.ctx):l->ops.capabilities;
    uint16_t caps=(declared&AFR2_CAP_MASK)|AFR2_CAP_UART;
    if (!l->ops.stop) caps&=(uint16_t)~AFR2_CAP_STOP;
    if (!l->ops.snapshot)
        caps&=(uint16_t)~(AFR2_CAP_STATE|AFR2_CAP_LINE|AFR2_CAP_ULTRASONIC|AFR2_CAP_ENCODERS);
    return caps;
}
static void handle(Afr2Link *l,uint32_t now) {
    Afr2WireParser *p=&l->parser;
    uint8_t out[AFR2_WIRE_PAYLOAD_MAX], n=9;
    uint16_t caps=capabilities(l);
    uint32_t session, transaction;
    Afr2LinkSnapshot s;
    if (!afr2_route_matches(p->route,AFR2_NODE_ESP,AFR2_NODE_MEGA) || p->length<8u) return;
    if(p->command==AFR2_EXT_BOOT && p->length==14 && !memcmp(p->payload,"BT01",4) && p->payload[4]==AFR2_NODE_ESP) {
        notify(l,AFR2_LINK_EVT_PEER_BOOT,p->command,0,p->payload,14,0);
        if(l->ops.halt_motion)l->ops.halt_motion(l->ops.ctx);else if(l->ops.stop)l->ops.stop(l->ops.ctx);
        if(l->ops.revoke)l->ops.revoke(l->ops.ctx);
        l->negotiated=0;l->session=l->last_transaction=0;return;
    }
    session=afr2_get_u32(p->payload); transaction=afr2_get_u32(p->payload+4);
    memcpy(out,p->payload,8); out[8]=AFR2_RESULT_OK;
    if (!session || !transaction) out[8]=AFR2_RESULT_BAD_REQUEST;
    else if (session==l->session && transaction<=l->last_transaction)
        out[8]=AFR2_RESULT_BAD_REQUEST; /* Never execute a duplicate/older request. */
    else if (p->command==AFR2_CMD_HELLO) {
        if (p->length!=13u || memcmp(p->payload+8,"AFR2",4) || p->payload[12]!=AFR2_WIRE_PROFILE)
            out[8]=AFR2_RESULT_BAD_REQUEST;
        else {
            if (l->session!=session) {
                notify(l,AFR2_LINK_EVT_SESSION,p->command,0,0,0,session);
                if(l->ops.halt_motion)l->ops.halt_motion(l->ops.ctx);else if(l->ops.stop)l->ops.stop(l->ops.ctx);
                if(l->ops.revoke)l->ops.revoke(l->ops.ctx);
            }
            l->session=session; l->last_transaction=transaction; l->negotiated=1;
            l->supervised=0; l->last_request_ms=now;
            memcpy(out+9,"AFR2",4); out[13]=AFR2_WIRE_PROFILE;
            out[14]=0; out[15]=4; out[16]=0;
            afr2_put_u16(out+17,caps); afr2_put_u32(out+19,now); n=23;
        }
    } else if (p->command>=0x10u && l->ops.command) {
        if(!l->negotiated || session!=l->session)out[8]=AFR2_RESULT_SESSION;
        else {
            uint8_t body=0;
            l->last_transaction=transaction;
            out[8]=l->ops.command(l->ops.ctx,p->command,p->payload+8,p->length-8,out+9,&body);
            if(body>55u || out[8]>AFR2_RESULT_INTERNAL){out[8]=AFR2_RESULT_INTERNAL;body=0;}
            if(out[8]==AFR2_RESULT_OK){n=9u+body;l->last_request_ms=now;}
        }
    } else if (p->length!=8u) out[8]=AFR2_RESULT_BAD_REQUEST;
    else if (!l->negotiated || session!=l->session) out[8]=AFR2_RESULT_SESSION;
    else if (p->command<AFR2_CMD_ALIVE || p->command>AFR2_CMD_STOP)
        out[8]=AFR2_RESULT_UNAVAILABLE;
    else {
        l->last_transaction=transaction; l->last_request_ms=now;
        memset(&s,0,sizeof(s)); s.mode=255; s.stopped=255;
        s.state_age_ms=s.line_age_ms=s.ultrasonic_age_ms=s.encoders_age_ms=AFR2_AGE_UNKNOWN;
        if (l->ops.snapshot && p->command>=AFR2_CMD_STATE && p->command<=AFR2_CMD_ENCODERS)
            l->ops.snapshot(l->ops.ctx,now,&s);
        if (s.mode>6u) s.mode=255;
        if (s.stopped>1u) s.stopped=255;
        switch (p->command) {
        case AFR2_CMD_ALIVE: afr2_put_u32(out+9,now); n=13; break;
        case AFR2_CMD_STATE:
            out[9]=(caps&AFR2_CAP_STATE)?s.mode:255;
            out[10]=(caps&AFR2_CAP_STATE)?s.stopped:255; out[11]=0;
            afr2_put_u32(out+12,(caps&AFR2_CAP_STATE)?s.state_age_ms:AFR2_AGE_UNKNOWN); n=16; break;
        case AFR2_CMD_SENSORS:
            out[9]=(caps&AFR2_CAP_LINE) && s.line_valid && s.line_age_ms!=AFR2_AGE_UNKNOWN;
            out[10]=s.line_bits&31u; afr2_put_u32(out+11,s.line_age_ms);
            out[15]=(caps&AFR2_CAP_ULTRASONIC) && s.ultrasonic_valid && s.ultrasonic_age_ms!=AFR2_AGE_UNKNOWN;
            afr2_put_u16(out+16,s.distance_mm); afr2_put_u32(out+18,s.ultrasonic_age_ms); n=22; break;
        case AFR2_CMD_ENCODERS:
            out[9]=s.encoders_wired!=0;
            out[10]=(caps&AFR2_CAP_ENCODERS) && s.encoders_wired && s.encoders_valid && s.encoders_age_ms!=AFR2_AGE_UNKNOWN;
            for (uint8_t i=0;i<4;++i) afr2_put_u32(out+11+4*i,(uint32_t)s.counts[i]);
            afr2_put_u32(out+27,s.encoders_age_ms); n=31; break;
        case AFR2_CMD_STOP:
            if ((caps&AFR2_CAP_STOP) && l->ops.stop && l->ops.stop(l->ops.ctx)) {
                if(l->ops.snapshot)l->ops.snapshot(l->ops.ctx,now,&s);
                out[9]=1; out[10]=s.mode==5?5:0; n=11;
            } else out[8]=AFR2_RESULT_UNAVAILABLE;
            break;
        default: out[8]=AFR2_RESULT_UNAVAILABLE; break;
        }
    }
    ++l->requests;
    if(out[8]!=AFR2_RESULT_OK){++l->rejects;notify(l,AFR2_LINK_EVT_REJECT,p->command,out[8],0,0,0);}
    l->reply_length=afr2_wire_build(l->reply,sizeof(l->reply),AFR2_ROUTE_RESPONSE,p->command,out,n);
}
void Afr2Link_Push(Afr2Link *l,uint8_t b,uint32_t now) {
    if (l->reply_length) return;
    Afr2Link_Tick(l,now); l->last_rx_ms=now;
    if (afr2_wire_push(&l->parser,b)) handle(l,now);
}
