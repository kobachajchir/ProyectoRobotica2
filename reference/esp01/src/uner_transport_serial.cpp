#include "uner_transport_serial.h"

uint16_t UNER_SerialPollRx(void *ctx, uint8_t *out, uint16_t max_len) {
  if (!ctx || !out || max_len == 0) return 0;
  UNER_SerialTransportCtx *sctx = static_cast<UNER_SerialTransportCtx *>(ctx);
  uint16_t count = 0;
  while (count < max_len && sctx->serial->available() > 0) {
    out[count++] = (uint8_t)sctx->serial->read();
  }
  if (sctx->serial->available() > 0) yield();
  return count;
}

int UNER_SerialTrySend(void *ctx, const uint8_t *data, uint16_t len) {
  if (!ctx || !data || len == 0) return -1;
  UNER_SerialTransportCtx *sctx = static_cast<UNER_SerialTransportCtx *>(ctx);
  if ((uint16_t)sctx->serial->availableForWrite() < len) return 0;
  size_t wr = sctx->serial->write(data, len);
  return wr == len ? 1 : -1;
}

void UNER_SerialTransport_InitWithContext(UNER_Transport *transport,
    UNER_SerialTransportCtx *ctx, uint8_t id, HardwareSerial &serial, uint8_t max_payload) {
  if (!transport || !ctx) return;
  ctx->serial = &serial;
  transport->id = id;
  transport->max_payload = max_payload;
  transport->ctx = ctx;
  transport->poll_rx = UNER_SerialPollRx;
  transport->try_send = UNER_SerialTrySend;
}
void UNER_SerialTransport_Init(UNER_Transport *transport, uint8_t id,
    HardwareSerial &serial, uint8_t max_payload) {
  static UNER_SerialTransportCtx ctx;
  UNER_SerialTransport_InitWithContext(transport, &ctx, id, serial, max_payload);
}
