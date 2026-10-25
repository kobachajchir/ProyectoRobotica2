// AFR2-20261023-15. ROBO2 broker. Frame ownership survives partial TX writes.
#pragma once
#include <Arduino.h>
#include "afr2_product_wire.h"
struct MegaResult {
    bool available=false, timeout=false, transportFault=false;
    uint8_t command=0, length=0, payload[AFR2_WIRE_PAYLOAD_MAX]={};
};
class MegaLink {
public:
    void begin();
    void tick(uint32_t now);
    bool request(uint8_t command, uint32_t now, bool priority=false);
    bool requestProduct(uint8_t command,const uint8_t *body,uint8_t length,uint32_t now,bool priority=false);
    bool take(MegaResult &result);
    bool takeBoot(){bool value=bootPending_;bootPending_=false;return value;}
    uint32_t bootId() const { return peerBoot_; }
    uint8_t bootCause() const { return peerCause_; }
    uint32_t ownBootId() const { return ownBoot_; }
    bool ready() const { return ready_; }
    bool busy() const { return active_ || result_.available || txLength_ || nextLength_; }
    const char *state() const { return ready_?"ready":handshaking_?"handshaking":stale_?"stale":"disconnected"; }
    uint16_t capabilities() const { return ready_?capabilities_:0; }
    bool hasReply() const { return everReady_; }
    uint32_t age(uint32_t now) const { return (uint32_t)(now-lastGood_); }
private:
    uint32_t ownBoot_=0,peerBoot_=0;
    uint8_t peerCause_=0;bool bootPending_=false;
    Afr2WireParser parser_={};
    MegaResult result_;
    uint8_t tx_[AFR2_WIRE_FRAME_MAX]={}, next_[AFR2_WIRE_FRAME_MAX]={};
    uint8_t txLength_=0, txOffset_=0, nextLength_=0, command_=0;
    uint32_t epoch_=0, sequence_=0, started_=0, lastGood_=0, lastRx_=0, lastAttempt_=0;
    uint16_t capabilities_=0;
    bool active_=false, user_=false, ready_=false, everReady_=false, handshaking_=false, stale_=false;
    uint8_t requestBody_[56]={}, requestLength_=0;
    bool start(uint8_t command, uint32_t now, bool user, bool priority,const uint8_t *body=nullptr,uint8_t size=0);
    void receive(uint32_t now);
};
