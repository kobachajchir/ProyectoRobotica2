#pragma once
#include <Arduino.h>
#include "uner_handle.h"

struct UNER_SerialTransportCtx {
  HardwareSerial *serial;
};

uint16_t UNER_SerialPollRx(void *ctx, uint8_t *out, uint16_t max_len);
int UNER_SerialTrySend(void *ctx, const uint8_t *data, uint16_t len);
void UNER_SerialTransport_Init(UNER_Transport *transport, uint8_t id, HardwareSerial &serial, uint8_t max_payload);

void UNER_SerialTransport_InitWithContext(UNER_Transport *transport,
    UNER_SerialTransportCtx *context, uint8_t id, HardwareSerial &serial,
    uint8_t max_payload);
