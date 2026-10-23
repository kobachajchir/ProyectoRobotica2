/*
 * uner_transport_usbcdc.c - UNER USB CDC transport.
 */
#include "uner_transport_usbcdc.h"

#include <string.h>

#include "usbd_cdc_if.h"

static UNER_TransportUsbCdc *active_usb_transport;

#define UNER_USB_CDC_TX_PRIORITY_AUTO 0xFFu
#define UNER_USB_CDC_CMD_PING         0x31u
#define UNER_USB_CDC_EVENT_FIRST      0x80u
#define UNER_USB_CDC_EVENT_LAST       0xDFu

static uint16_t UNER_UsbNextIndex(uint16_t index)
{
    index++;
    return (index >= UNER_USB_CDC_RX_RING_SIZE) ? 0u : index;
}

static uint8_t UNER_UsbRingPush(UNER_TransportUsbCdc *usb, uint8_t byte)
{
    uint16_t next;

    if (usb == NULL) {
        return 0u;
    }

    next = UNER_UsbNextIndex(usb->rx_head);
    if (next == usb->rx_tail) {
        return 0u;
    }

    usb->rx_ring[usb->rx_head] = byte;
    usb->rx_head = next;
    return 1u;
}

static uint8_t UNER_UsbRingPop(UNER_TransportUsbCdc *usb, uint8_t *byte)
{
    if (usb == NULL || byte == NULL || usb->rx_head == usb->rx_tail) {
        return 0u;
    }

    *byte = usb->rx_ring[usb->rx_tail];
    usb->rx_tail = UNER_UsbNextIndex(usb->rx_tail);
    return 1u;
}

static void UNER_UsbClearRx(UNER_TransportUsbCdc *usb)
{
    if (usb == NULL) {
        return;
    }

    usb->rx_head = 0u;
    usb->rx_tail = 0u;
}

static void UNER_UsbClearTx(UNER_TransportUsbCdc *usb)
{
    if (usb == NULL) {
        return;
    }

    usb->tx_busy = 0u;
    usb->tx_service_pending = 0u;
    usb->tx_queue_count = 0u;
    usb->next_tx_priority = UNER_USB_CDC_TX_PRIORITY_AUTO;
}

static uint8_t UNER_UsbPriorityFromFrame(UNER_TransportUsbCdc *usb,
                                         const uint8_t *buf,
                                         uint16_t len)
{
    uint8_t cmd;
    uint8_t priority;

    if (usb != NULL && usb->next_tx_priority != UNER_USB_CDC_TX_PRIORITY_AUTO) {
        priority = usb->next_tx_priority;
        usb->next_tx_priority = UNER_USB_CDC_TX_PRIORITY_AUTO;
        return priority;
    }

    if (buf == NULL || len <= UNER_OFF_CMD) {
        return UNER_USB_CDC_TX_PRIORITY_RESPONSE;
    }

    cmd = buf[UNER_OFF_CMD];
    if (cmd == UNER_USB_CDC_CMD_PING) {
        return UNER_USB_CDC_TX_PRIORITY_RESPONSE;
    }

    if (cmd >= UNER_USB_CDC_EVENT_FIRST && cmd <= UNER_USB_CDC_EVENT_LAST) {
        return UNER_USB_CDC_TX_PRIORITY_EVENT;
    }

    return UNER_USB_CDC_TX_PRIORITY_RESPONSE;
}

static void UNER_UsbQueueRemoveAt(UNER_TransportUsbCdc *usb, uint8_t index)
{
    if (usb == NULL || index >= usb->tx_queue_count) {
        return;
    }

    while ((uint8_t)(index + 1u) < usb->tx_queue_count) {
        usb->tx_queue[index] = usb->tx_queue[index + 1u];
        index++;
    }
    usb->tx_queue_count--;
}

static UNER_Status UNER_UsbQueueFrame(UNER_TransportUsbCdc *usb,
                                      const uint8_t *buf,
                                      uint16_t len,
                                      uint8_t priority)
{
    uint8_t insert_at;

    if (usb == NULL || buf == NULL || len == 0u ||
        len > UNER_USB_CDC_TX_BUFFER_SIZE) {
        return UNER_ERR_LEN;
    }

    if (usb->tx_queue_count >= UNER_USB_CDC_TX_QUEUE_SLOTS) {
        uint8_t lowest_index = 0u;
        uint8_t lowest_priority = usb->tx_queue[0].priority;

        for (uint8_t i = 1u; i < usb->tx_queue_count; ++i) {
            if (usb->tx_queue[i].priority < lowest_priority) {
                lowest_priority = usb->tx_queue[i].priority;
                lowest_index = i;
            }
        }

        if (priority <= lowest_priority) {
            usb->tx_queue_full_count++;
            return UNER_ERR_BUSY;
        }

        UNER_UsbQueueRemoveAt(usb, lowest_index);
        usb->tx_queue_drop_count++;
    }

    insert_at = usb->tx_queue_count;
    while (insert_at > 0u &&
           usb->tx_queue[insert_at - 1u].priority < priority) {
        usb->tx_queue[insert_at] = usb->tx_queue[insert_at - 1u];
        insert_at--;
    }

    usb->tx_queue[insert_at].len = len;
    usb->tx_queue[insert_at].priority = priority;
    memcpy(usb->tx_queue[insert_at].data, buf, len);
    usb->tx_queue_count++;
    usb->tx_deferred_count++;
    return UNER_OK;
}

static UNER_Status UNER_UsbStartTx(UNER_TransportUsbCdc *usb,
                                   const uint8_t *buf,
                                   uint16_t len)
{
    uint8_t usb_status;

    if (usb == NULL || buf == NULL || len == 0u ||
        len > UNER_USB_CDC_TX_BUFFER_SIZE) {
        return UNER_ERR_LEN;
    }

    if (!CDC_IsDtrActive_FS()) {
        return UNER_ERR_BUSY;
    }

    memcpy(usb->tx_buffer, buf, len);
    usb->tx_busy = 1u;
    usb_status = CDC_Transmit_FS(usb->tx_buffer, len);
    if (usb_status == USBD_OK) {
        return UNER_OK;
    }

    usb->tx_busy = 0u;
    return (usb_status == USBD_BUSY) ? UNER_ERR_BUSY : UNER_ERR_LEN;
}

static UNER_Status UNER_TransportUsbCdc_PollRx(UNER_Transport *transport, UNER_Core *core)
{
    UNER_TransportUsbCdc *usb;
    uint8_t byte;

    if (transport == NULL || core == NULL || transport->ctx == NULL) {
        return UNER_ERR_LEN;
    }

    usb = (UNER_TransportUsbCdc *)transport->ctx;
    while (UNER_UsbRingPop(usb, &byte)) {
        (void)UNER_Core_PushByte(core, transport->id, transport->max_payload, byte);
    }

    return UNER_OK;
}

static UNER_Status UNER_TransportUsbCdc_TrySend(UNER_Transport *transport, const uint8_t *buf, uint16_t len)
{
    UNER_TransportUsbCdc *usb;
    uint8_t priority;
    UNER_Status status;

    if (transport == NULL || transport->ctx == NULL || buf == NULL ||
        len == 0u || len > UNER_USB_CDC_TX_BUFFER_SIZE) {
        return UNER_ERR_LEN;
    }

    usb = (UNER_TransportUsbCdc *)transport->ctx;
    if (!CDC_IsDtrActive_FS()) {
        return UNER_ERR_BUSY;
    }

    priority = UNER_UsbPriorityFromFrame(usb, buf, len);
    if (usb->tx_busy || usb->tx_queue_count > 0u) {
        return UNER_UsbQueueFrame(usb, buf, len, priority);
    }

    status = UNER_UsbStartTx(usb, buf, len);
    if (status == UNER_ERR_BUSY) {
        return UNER_UsbQueueFrame(usb, buf, len, priority);
    }

    return status;
}

UNER_Status UNER_TransportUsbCdc_Init(UNER_TransportUsbCdc *transport, uint8_t transport_id)
{
    if (!transport) {
        return UNER_ERR_LEN;
    }

    transport->base.id = transport_id;
    transport->base.max_payload = 255u;
    transport->base.poll_rx = UNER_TransportUsbCdc_PollRx;
    transport->base.try_send = UNER_TransportUsbCdc_TrySend;
    transport->base.ctx = transport;
    transport->rx_head = 0u;
    transport->rx_tail = 0u;
    transport->tx_busy = 0u;
    transport->tx_service_pending = 0u;
    transport->tx_queue_count = 0u;
    transport->next_tx_priority = UNER_USB_CDC_TX_PRIORITY_AUTO;
    transport->rx_overflow_count = 0u;
    transport->tx_queue_full_count = 0u;
    transport->tx_queue_drop_count = 0u;
    transport->tx_deferred_count = 0u;
    memset(transport->rx_ring, 0, sizeof(transport->rx_ring));
    memset(transport->tx_buffer, 0, sizeof(transport->tx_buffer));
    memset(transport->tx_queue, 0, sizeof(transport->tx_queue));
    active_usb_transport = transport;

    return UNER_OK;
}

void UNER_TransportUsbCdc_OnRx(const uint8_t *buf, uint32_t len)
{
    UNER_TransportUsbCdc *usb = active_usb_transport;

    if (usb == NULL || buf == NULL || len == 0u) {
        return;
    }

    for (uint32_t i = 0u; i < len; ++i) {
        if (!UNER_UsbRingPush(usb, buf[i])) {
            usb->rx_overflow_count++;
            UNER_UsbClearRx(usb);
            break;
        }
    }
}

void UNER_TransportUsbCdc_OnTxComplete(void)
{
    if (active_usb_transport != NULL) {
        active_usb_transport->tx_busy = 0u;
        active_usb_transport->tx_service_pending = 1u;
    }
}

void UNER_TransportUsbCdc_OnDisconnect(void)
{
    if (active_usb_transport != NULL) {
        UNER_UsbClearRx(active_usb_transport);
        UNER_UsbClearTx(active_usb_transport);
    }
}

void UNER_TransportUsbCdc_ServiceTx(void)
{
    UNER_TransportUsbCdc *usb = active_usb_transport;

    if (usb == NULL || usb->tx_busy) {
        return;
    }

    usb->tx_service_pending = 0u;
    if (usb->tx_queue_count == 0u) {
        return;
    }

    if (UNER_UsbStartTx(usb,
                        usb->tx_queue[0].data,
                        usb->tx_queue[0].len) == UNER_OK) {
        UNER_UsbQueueRemoveAt(usb, 0u);
        usb->tx_service_pending = (usb->tx_queue_count > 0u) ? 1u : 0u;
    } else {
        usb->tx_service_pending = 1u;
    }
}

void UNER_TransportUsbCdc_SetNextTxPriority(uint8_t priority)
{
    if (active_usb_transport == NULL) {
        return;
    }

    if (priority > UNER_USB_CDC_TX_PRIORITY_RESPONSE) {
        priority = UNER_USB_CDC_TX_PRIORITY_RESPONSE;
    }

    active_usb_transport->next_tx_priority = priority;
}

uint8_t UNER_TransportUsbCdc_HasPendingRx(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL && usb->rx_head != usb->rx_tail) ? 1u : 0u;
}

uint8_t UNER_TransportUsbCdc_HasPendingTx(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL &&
            (usb->tx_busy || usb->tx_service_pending || usb->tx_queue_count > 0u)) ? 1u : 0u;
}

uint8_t UNER_TransportUsbCdc_GetQueuedTxCount(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL) ? usb->tx_queue_count : 0u;
}

uint32_t UNER_TransportUsbCdc_GetRxOverflowCount(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL) ? usb->rx_overflow_count : 0u;
}

uint32_t UNER_TransportUsbCdc_GetTxQueueFullCount(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL) ? usb->tx_queue_full_count : 0u;
}

uint32_t UNER_TransportUsbCdc_GetTxQueueDropCount(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL) ? usb->tx_queue_drop_count : 0u;
}

uint32_t UNER_TransportUsbCdc_GetTxDeferredCount(void)
{
    const UNER_TransportUsbCdc *usb = active_usb_transport;

    return (usb != NULL) ? usb->tx_deferred_count : 0u;
}
