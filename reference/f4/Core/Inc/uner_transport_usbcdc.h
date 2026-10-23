/*
 * uner_transport_usbcdc.h - UNER USB CDC transport.
 */
#ifndef INC_UNER_TRANSPORT_USBCDC_H_
#define INC_UNER_TRANSPORT_USBCDC_H_

#include <stdint.h>
#include "uner_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UNER_USB_CDC_RX_RING_SIZE   512u
#define UNER_USB_CDC_TX_BUFFER_SIZE (UNER_OVERHEAD + 255u)
#define UNER_USB_CDC_TX_QUEUE_SLOTS 6u

#define UNER_USB_CDC_TX_PRIORITY_LOW      0u
#define UNER_USB_CDC_TX_PRIORITY_EVENT    1u
#define UNER_USB_CDC_TX_PRIORITY_RESPONSE 2u

typedef struct {
    uint16_t len;
    uint8_t priority;
    uint8_t data[UNER_USB_CDC_TX_BUFFER_SIZE];
} UNER_UsbCdcTxFrame;

typedef struct {
    UNER_Transport base;
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
    volatile uint8_t tx_busy;
    volatile uint8_t tx_service_pending;
    uint8_t tx_queue_count;
    uint8_t next_tx_priority;
    volatile uint32_t rx_overflow_count;
    volatile uint32_t tx_queue_full_count;
    volatile uint32_t tx_queue_drop_count;
    volatile uint32_t tx_deferred_count;
    uint8_t rx_ring[UNER_USB_CDC_RX_RING_SIZE];
    uint8_t tx_buffer[UNER_USB_CDC_TX_BUFFER_SIZE];
    UNER_UsbCdcTxFrame tx_queue[UNER_USB_CDC_TX_QUEUE_SLOTS];
} UNER_TransportUsbCdc;

UNER_Status UNER_TransportUsbCdc_Init(UNER_TransportUsbCdc *transport, uint8_t transport_id);
void UNER_TransportUsbCdc_OnRx(const uint8_t *buf, uint32_t len);
void UNER_TransportUsbCdc_OnTxComplete(void);
void UNER_TransportUsbCdc_OnDisconnect(void);
void UNER_TransportUsbCdc_ServiceTx(void);
void UNER_TransportUsbCdc_SetNextTxPriority(uint8_t priority);
uint8_t UNER_TransportUsbCdc_HasPendingRx(void);
uint8_t UNER_TransportUsbCdc_HasPendingTx(void);
uint8_t UNER_TransportUsbCdc_GetQueuedTxCount(void);
uint32_t UNER_TransportUsbCdc_GetRxOverflowCount(void);
uint32_t UNER_TransportUsbCdc_GetTxQueueFullCount(void);
uint32_t UNER_TransportUsbCdc_GetTxQueueDropCount(void);
uint32_t UNER_TransportUsbCdc_GetTxDeferredCount(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_UNER_TRANSPORT_USBCDC_H_ */
