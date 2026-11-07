/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
#include "afr2_build_info.h"
// AFR2-20261023-15. ROBO2 profile 2, shared parser and explicit nibble routing.
#include "mega_link.h"
extern "C" {
#include <user_interface.h>
}
void MegaLink::begin() {
    epoch_=os_random(); if (!epoch_) epoch_=1;
    Serial.setDebugOutput(false); Serial.setRxBufferSize(256); Serial.begin(115200,SERIAL_8N1);
    afr2_wire_reset(&parser_);ownBoot_=epoch_;
    uint8_t boot[14]={0};memcpy(boot,"BT01",4);boot[4]=AFR2_NODE_ESP;afr2_put_u32(boot+5,ownBoot_);
    boot[9]=ESP.getResetInfoPtr()?ESP.getResetInfoPtr()->reason:255;boot[10]=AFR2_FIRMWARE_MAJOR;
    boot[11]=AFR2_FIRMWARE_MINOR;boot[12]=AFR2_FIRMWARE_PATCH;boot[13]=AFR2_EXT_VERSION;
    txLength_=afr2_wire_build(tx_,sizeof(tx_),AFR2_ROUTE_REQUEST,AFR2_EXT_BOOT,boot,14);txOffset_=0;
}
bool MegaLink::start(uint8_t command,uint32_t now,bool user,bool priority,const uint8_t *body,uint8_t size) {
    if(size>56 || (size && !body))return false;
    if (!priority && busy()) return false;
    if (nextLength_) return false;
    if (sequence_==UINT32_MAX) {
        uint32_t candidate=os_random();
        epoch_=candidate && candidate!=epoch_?candidate:(epoch_==UINT32_MAX?1:epoch_+1);
        sequence_=0; ready_=false; capabilities_=0;
        if (command!=AFR2_CMD_HELLO) return false;
    }
    uint8_t payload[64], frame[AFR2_WIRE_FRAME_MAX];
    afr2_put_u32(payload,epoch_); afr2_put_u32(payload+4,++sequence_);
    uint8_t length=8+size;
    requestLength_=size;if(size){memcpy(payload+8,body,size);memcpy(requestBody_,body,size);}
    if (command==AFR2_CMD_HELLO) {
        memcpy(payload+8,"AFR2",4); payload[12]=AFR2_WIRE_PROFILE; length=13;
        handshaking_=true;
    }
    uint8_t n=afr2_wire_build(frame,sizeof(frame),AFR2_ROUTE_REQUEST,command,payload,length);
    if (!n) return false;
    if (txLength_) { memcpy(next_,frame,n); nextLength_=n; }
    else { memcpy(tx_,frame,n); txLength_=n; txOffset_=0; }
    result_={}; command_=command; user_=user; active_=true;
    started_=lastAttempt_=now; return true;
}
bool MegaLink::request(uint8_t command,uint32_t now,bool priority) {
    if (!ready_ || command<AFR2_CMD_ALIVE || command>AFR2_CMD_STOP ||
        (priority && command!=AFR2_CMD_STOP)) return false;
    return start(command,now,true,priority);
}
bool MegaLink::requestProduct(uint8_t command,const uint8_t *body,uint8_t size,uint32_t now,bool priority) {
    if(!ready_ || command<0x10 || command>AFR2_EXT_MOTION_HALT || (priority && command!=AFR2_EXT_SEQ_CANCEL && command!=AFR2_EXT_MOTION_HALT))return false;
    return start(command,now,true,priority,body,size);
}
// AFR2-20261106-12: PWM only. AVR replies are drained/ignored; ALIVE remains correlated.
bool MegaLink::streamPwm(const uint8_t *body,uint32_t now) {
    if(!ready_ || busy())return false;
    uint32_t previousAttempt=lastAttempt_;
    if(!start(AFR2_EXT_DRIVE_PWM,now,false,false,body,12))return false;
    active_=false;lastAttempt_=previousAttempt;return true;
}
bool MegaLink::take(MegaResult &r) {
    if (!result_.available) return false;
    r=result_; result_.available=false; return true;
}
static bool bodyValid(uint8_t command,const uint8_t *p,uint16_t caps) {
    switch (command) {
    case AFR2_CMD_HELLO: {
        uint16_t advertised=afr2_get_u16(p+17);
        return !memcmp(p+9,"AFR2",4) && p[13]==AFR2_WIRE_PROFILE &&
            (advertised&AFR2_CAP_UART) && !(advertised&(uint16_t)~AFR2_CAP_MASK);
    }
    case AFR2_CMD_ALIVE: return true;
    case AFR2_CMD_STATE:
        if (!(caps&AFR2_CAP_STATE))
            return p[9]==255 && p[10]==255 && p[11]==0 && afr2_get_u32(p+12)==AFR2_AGE_UNKNOWN;
        return (p[9]<=6 || p[9]==255) && (p[10]<=1 || p[10]==255) && p[11]==0;
    case AFR2_CMD_SENSORS:
        return p[9]<=1 && !(p[10]&0xe0u) && p[15]<=1 &&
            (!p[9] || ((caps&AFR2_CAP_LINE) && afr2_get_u32(p+11)!=AFR2_AGE_UNKNOWN)) &&
            (!p[15] || ((caps&AFR2_CAP_ULTRASONIC) && afr2_get_u32(p+18)!=AFR2_AGE_UNKNOWN));
    case AFR2_CMD_ENCODERS:
        return p[9]<=1 && p[10]<=1 && (!p[10] ||
            (p[9] && (caps&AFR2_CAP_ENCODERS) && afr2_get_u32(p+27)!=AFR2_AGE_UNKNOWN));
    case AFR2_CMD_STOP: return (caps&AFR2_CAP_STOP) && p[9]==1 && (p[10]==0 || p[10]==5);
    default: return false;
    }
}
// Explicit product reply validation; never accept an arbitrary successful body.
static bool productReply(uint8_t cmd,const uint8_t *p,uint8_t n,const uint8_t *q,uint8_t qn) {
    if(cmd>=AFR2_EXT_US_GET && cmd<=AFR2_EXT_MOTION_HALT)return n>=9 && afr2_sensor_reply_valid(cmd,p+9,n-9,q,qn);
    switch(cmd) {
    case AFR2_EXT_BATTERY:return qn==1 && q[0]<=1 && n==32 && afr2_battery_reply_valid(p+9,n-9);
    case AFR2_EXT_NETWORK_STATE:return qn>=8 && n==16 && !memcmp(p+9,q,7);
    case AFR2_EXT_NETWORK_EVENT:return qn==10 && n==13 && afr2_get_u32(p+9)==afr2_get_u32(q+4);
    case AFR2_EXT_INFO:return qn==1 && n>=16 && p[9]==q[0] && p[9]<5 && p[15]<=48 && n==16+p[15] && afr2_get_u32(p+10);
    case AFR2_EXT_REBOOT:return qn==1 && n==14 && p[9]==1 && afr2_get_u32(p+10);
    case AFR2_EXT_HELLO:return n==19 && !memcmp(p+9,"EX01",4) && afr2_get_u16(p+13)==AFR2_EXT_VERSION && !(afr2_get_u32(p+15)&~AFR2_EXT_FEATURE_MASK);
    case AFR2_EXT_STATE:return n==28 && p[25]<=1 && afr2_get_u16(p+26)>=800 && afr2_get_u16(p+26)<=3000 && p[9]<=7 && p[10]<=2 && p[11]<=1 && p[13]<=1 && p[14]<=1 && p[15]<=4 && p[16]<=3 && !(afr2_get_u32(p+17)&~AFR2_EXT_FEATURE_MASK);
    case AFR2_EXT_LIMITS:return n==17 && afr2_get_u16(p+9)>=10 && afr2_get_u16(p+9)<=1000 && p[11]>0 && p[12]==10;
    case AFR2_EXT_CONFIG:return n==13;
    case AFR2_EXT_PREFS:return n==9;
    case AFR2_EXT_LINE_CAL:return n==10 && p[9]<=1;
    case AFR2_EXT_HALL_CAL:return n==15 && p[9]<=1 && p[10]<=1;
    case AFR2_EXT_WIFI_UPDATE:return n==9;
    case AFR2_EXT_WIFI_POLL:return qn==1 && n>=23 && p[9]<=1 && p[14]<=12 && p[15]<=96 && p[16]==q[0] && p[17]<=40 && n==23+p[17] && (uint16_t)p[16]+p[17]<=p[15] && p[18]>=1 && p[18]<=3;
    case AFR2_EXT_WEB_PULSE:return qn==2 && n==11 && afr2_get_u16(p+9)==afr2_get_u16(q);
    case AFR2_EXT_DRIVE_PWM:case AFR2_EXT_DRIVE:case AFR2_EXT_WHEEL:case AFR2_EXT_MODE:case AFR2_EXT_GOTO:return n==9;
    case AFR2_EXT_CANCEL:return n==10 && p[9]<=4;
    case AFR2_EXT_POSE:return n==34 && p[9]<=1 && (!p[9] ||
        ((int32_t)afr2_get_u32(p+18)>=-3142 && (int32_t)afr2_get_u32(p+18)<=3142 && afr2_get_u32(p+22)!=AFR2_AGE_UNKNOWN && afr2_get_u32(p+26) && afr2_get_u32(p+30)));
    case AFR2_EXT_UI_META:return n==23 && afr2_get_u16(p+19)==1024 && p[21]<=1;
    case AFR2_EXT_UI_PAGE:return qn==7 && q[6]>0 && q[6]<=40 && n==16+q[6] &&
        afr2_get_u32(p+9)==afr2_get_u32(q) && afr2_get_u16(p+13)==afr2_get_u16(q+4) && p[15]==q[6];
    case AFR2_EXT_TOUCH_UI:return qn==4 && n==10 && p[9]<=100;
    case AFR2_EXT_DIAG_POLL:return qn==0 && n==18 && p[9]<=1;
    case AFR2_EXT_DIAG_RESULT:return qn==8 && n==9;
    case AFR2_EXT_UI_RELEASE:return n==9;
    case AFR2_EXT_UI_ACTION:return n==14;
    case AFR2_EXT_SEQ_BEGIN:case AFR2_EXT_SEQ_COMMIT:
        return (qn==13 || (cmd==AFR2_EXT_SEQ_BEGIN && qn==14)) && n==19 && afr2_get_u32(p+9)==afr2_get_u32(q+4) && p[13]==(cmd==AFR2_EXT_SEQ_BEGIN?1:2) && p[14]<=32 && afr2_get_u32(p+15)<=120000;
    case AFR2_EXT_SEQ_APPEND:return (qn==30 || qn==28) && n==15 && afr2_get_u32(p+9)==afr2_get_u32(q+4) && p[13]==q[8] && p[14]==1;
    case AFR2_EXT_SEQ_START:return qn==12 && n==23 && afr2_get_u32(p+9)==afr2_get_u32(q+4) && afr2_get_u32(p+13)==afr2_get_u32(q+8) && p[17]==3 && p[18]==0 && afr2_get_u32(p+19)==0;
    case AFR2_EXT_SEQ_CANCEL:return qn==13 && n==19 && afr2_get_u32(p+9)==afr2_get_u32(q+4) && afr2_get_u32(p+13)==afr2_get_u32(q+8) && p[17]>=4 && p[17]<=6 && p[18]==1;
    case AFR2_EXT_SEQ_STATUS:return qn==8 && n==28 && afr2_get_u32(p+9)==afr2_get_u32(q) && afr2_get_u32(p+13)==afr2_get_u32(q+4) && p[17]<=6 && p[19]<=32 &&
        (p[18]==255 || p[18]<p[19]) && afr2_get_u32(p+20)<=afr2_get_u32(p+24) && afr2_get_u32(p+24)<=120000;
    default:return false;
    }
}
void MegaLink::receive(uint32_t now) {
    if(afr2_route_matches(parser_.route,AFR2_NODE_MEGA,AFR2_NODE_ESP) && parser_.command==AFR2_EXT_BOOT && parser_.length==14 && !memcmp(parser_.payload,"BT01",4) && parser_.payload[4]==AFR2_NODE_MEGA) {
        peerBoot_=afr2_get_u32(parser_.payload+5);peerCause_=parser_.payload[9];bootPending_=true;
        if(active_ && user_){result_={};result_.available=true;result_.transportFault=true;result_.command=command_;}
        active_=false;ready_=false;handshaking_=false;capabilities_=0;stale_=everReady_;lastAttempt_=now;return;
    }
    if (!afr2_route_matches(parser_.route,AFR2_NODE_MEGA,AFR2_NODE_ESP) ||
        parser_.length<9 || !active_ || (uint32_t)(now-started_)>=800u || parser_.command!=command_ ||
        afr2_get_u32(parser_.payload)!=epoch_ || afr2_get_u32(parser_.payload+4)!=sequence_) return;
    uint8_t status=parser_.payload[8], expected=afr2_wire_response_length(command_);
    if(command_>=0x10 && status==AFR2_RESULT_OK) {
        if(!productReply(command_,parser_.payload,parser_.length,requestBody_,requestLength_))return;
        expected=parser_.length;
    }
    if (status>AFR2_RESULT_INTERNAL ||
        (status==AFR2_RESULT_OK && (!expected || parser_.length!=expected)) ||
        (status!=AFR2_RESULT_OK && parser_.length!=9)) return;
    if (status==AFR2_RESULT_OK && command_<0x10 && !bodyValid(command_,parser_.payload,capabilities_)) return;
    if (command_==AFR2_CMD_HELLO) {
        handshaking_=false;
        if (status==AFR2_RESULT_OK) {
            capabilities_=afr2_get_u16(parser_.payload+17);
            ready_=everReady_=true; stale_=false;
        } else { ready_=false; capabilities_=0; stale_=everReady_; }
    }
    if (status==AFR2_RESULT_SESSION) { ready_=false; capabilities_=0; }
    else if (ready_) lastGood_=now;
    if (user_) {
        result_.available=true; result_.command=command_; result_.length=parser_.length;
        memcpy(result_.payload,parser_.payload,parser_.length);
    }
    active_=false;
}
void MegaLink::tick(uint32_t now) {
    if (parser_.state && (uint32_t)(now-lastRx_)>AFR2_WIRE_GAP_MS) afr2_wire_gap(&parser_);
    bool overrun=Serial.hasOverrun(), rxError=Serial.hasRxError();
    if (overrun || rxError) {
        afr2_wire_reset(&parser_); ready_=false; capabilities_=0; handshaking_=false; stale_=everReady_;
        if (active_ && user_) {
            result_={}; result_.available=true; result_.transportFault=true; result_.command=command_;
        }
        active_=false; lastAttempt_=now;
        // Discard uncertain backlog; drain is bounded by the configured RX ring.
        for (uint16_t budget=256;budget && Serial.available();--budget) (void)Serial.read();
    } else {
        for (uint16_t budget=192;budget && Serial.available();--budget) {
            uint8_t b=(uint8_t)Serial.read(); lastRx_=now;
            if (afr2_wire_push(&parser_,b)) receive(now);
        }
    }
    if (txLength_) {
        int room=Serial.availableForWrite();
        if (room>0) {
            size_t left=txLength_-txOffset_;
            size_t n=Serial.write(tx_+txOffset_,left<(size_t)room?left:(size_t)room);
            txOffset_+=(uint8_t)n;
            if (txOffset_==txLength_) {
                txLength_=0; txOffset_=0;
                if (nextLength_) { memcpy(tx_,next_,nextLength_); txLength_=nextLength_; nextLength_=0; }
            }
        }
    }
    if (active_ && (uint32_t)(now-started_)>=800u) {
        if (user_) { result_={}; result_.available=true; result_.timeout=true; result_.command=command_; }
        active_=false; ready_=false; handshaking_=false; stale_=everReady_; capabilities_=0;
        lastAttempt_=now;
    }
    if (ready_ && (uint32_t)(now-lastGood_)>=1500u) { ready_=false; capabilities_=0; stale_=true; }
    if (!busy()) {
        if (!ready_ && (uint32_t)(now-lastAttempt_)>=1000u) start(AFR2_CMD_HELLO,now,false,false);
        else if (ready_ && (uint32_t)(now-lastAttempt_)>=500u) start(AFR2_CMD_ALIVE,now,false,false);
    }
}
