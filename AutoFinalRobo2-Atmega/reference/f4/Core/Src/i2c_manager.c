/*
 * i2c_manager.c
 *
 *  Created on: Jun 7, 2025
 *  Updated on: Jan 12, 2026
 *
 *  Arbitration model:
 *  - A device requests the bus.
 *  - The main-loop Tick assigns owner and calls device.on_granted(ctx).
 *  - Owner keeps lease until ReleaseBus().
 *  - Release only drops ownership; the next Tick arbitrates pending devices.
 *  - HAL callbacks only latch events, so HAL polling never runs from an IRQ.
 */

#include "i2c_manager.h"

#define IDX_INVALID 0xFFu

static uint8_t find_index(I2C_ManagerHandle *hmgr, I2C_DeviceID id)
{
    if (!hmgr) return IDX_INVALID;
    for (uint8_t i = 0; i < I2C_MANAGER_MAX_DEVICES; i++) {
        if (hmgr->devices[i].enabled && hmgr->devices[i].id == id) {
            return i;
        }
    }
    return IDX_INVALID;
}

static uint8_t effective_prio(I2C_ManagerHandle *hmgr, const I2C_DeviceEntry *dev)
{
    uint8_t prio;

    if (!hmgr || !dev) {
        return 0u;
    }

    prio = (uint8_t)dev->pending_prio;
    if (hmgr->policy == I2C_POLICY_PRIORITIZE_MPU && dev->id == DEVICE_ID_MPU) {
        prio++;
    } else if (hmgr->policy == I2C_POLICY_PRIORITIZE_OLED && dev->id == DEVICE_ID_OLED) {
        prio++;
    }

    return prio;
}

static void clear_device_entry(I2C_DeviceEntry *e)
{
    if (!e) return;
    e->id = (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE;
    e->addr_7bit = 0;
    e->priority = 0;
    e->enabled = 0;
    e->pending = 0;
    e->pending_prio = I2C_REQ_PRIO_LOW;
    e->ctx = NULL;
    e->on_granted = NULL;
    e->on_tx_done = NULL;
    e->on_rx_done = NULL;
    e->on_error = NULL;
}

static uint8_t pick_next_pending_index(I2C_ManagerHandle *hmgr)
{
    if (!hmgr) return IDX_INVALID;

    uint8_t best_idx = IDX_INVALID;
    uint8_t best_prio = 0u;
    uint8_t best_fair = 0;

    for (uint8_t i = 0; i < I2C_MANAGER_MAX_DEVICES; i++) {
        if (!hmgr->devices[i].enabled || !hmgr->devices[i].pending) {
            continue;
        }

        uint8_t prio = effective_prio(hmgr, &hmgr->devices[i]);
        uint8_t fair = (hmgr->devices[i].id != hmgr->last_owner_id);

        if (best_idx == IDX_INVALID ||
            prio > best_prio ||
            (prio == best_prio && fair > best_fair) ||
            (prio == best_prio && fair == best_fair && i < best_idx)) {
            best_idx = i;
            best_prio = prio;
            best_fair = fair;
        }
    }

    return best_idx;
}

static void grant_to_index(I2C_ManagerHandle *hmgr, uint8_t idx)
{
    if (!hmgr || idx == IDX_INVALID) return;

    if (!hmgr->hardware->ready(hmgr->hi2c)) {
        hmgr->devices[idx].pending = 1;
        return;
    }

    hmgr->devices[idx].pending = 0;
    hmgr->devices[idx].pending_prio = I2C_REQ_PRIO_LOW;

    hmgr->owner_id = hmgr->devices[idx].id;
    hmgr->owner_index = idx;
    hmgr->lease_start_tick = hmgr->hardware->now_ms(hmgr->hi2c);

     // BREAKPOINT: bus I2C otorgado a un dispositivo
    if (hmgr->devices[idx].on_granted) {
        hmgr->devices[idx].on_granted(hmgr->devices[idx].ctx);
    }
}

AM_Result I2C_Manager_Init(I2C_ManagerHandle *hmgr,
                                  const AM_I2C_Hardware *hardware,
                                  void *hi2c,
                                  uint32_t lease_timeout_ms)
{
    if (!hmgr || !hi2c || !hardware || !hardware->ready || !hardware->now_ms || !hardware->probe) return AM_ERROR;

    hmgr->hi2c = hi2c;
    hmgr->hardware=hardware;
    hmgr->owner_id = (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE;
    hmgr->last_owner_id = (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE;
    hmgr->owner_index = IDX_INVALID;
    hmgr->lease_start_tick = 0;
    hmgr->lease_timeout_ms = lease_timeout_ms;
    hmgr->policy = I2C_POLICY_BALANCED;
    hmgr->recover_cb = NULL;
    hmgr->tx_complete_pending = 0u;
    hmgr->rx_complete_pending = 0u;
    hmgr->error_pending = 0u;
    hmgr->bus_waiting = 0u;
    hmgr->bus_wait_started_tick = 0u;
    hmgr->spurious_tx = 0;
    hmgr->spurious_rx = 0;
    hmgr->spurious_err = 0;

    for (uint8_t i = 0; i < I2C_MANAGER_MAX_DEVICES; i++) {
        clear_device_entry(&hmgr->devices[i]);
    }

    return AM_OK;
}

AM_Result I2C_Manager_RegisterDevice(I2C_ManagerHandle *hmgr,
                                            I2C_DeviceID id,
                                            uint16_t addr_7bit,
                                            uint8_t priority,
                                            void *ctx,
                                            I2C_OnGrantedCb on_granted,
                                            I2C_OnTxDoneCb on_tx_done,
                                            I2C_OnRxDoneCb on_rx_done,
                                            I2C_OnErrorCb on_error)
{
    if (!hmgr) return AM_ERROR;

    // prevent duplicates
    if (find_index(hmgr, id) != IDX_INVALID) {
        return AM_ERROR;
    }

    for (uint8_t i = 0; i < I2C_MANAGER_MAX_DEVICES; i++) {
        if (!hmgr->devices[i].enabled) {
            hmgr->devices[i].id = id;
            hmgr->devices[i].addr_7bit = addr_7bit;
            hmgr->devices[i].priority = priority;
            hmgr->devices[i].enabled = 1;
            hmgr->devices[i].pending = 0;
            hmgr->devices[i].pending_prio = I2C_REQ_PRIO_LOW;
            hmgr->devices[i].ctx = ctx;
            hmgr->devices[i].on_granted = on_granted;
            hmgr->devices[i].on_tx_done = on_tx_done;
            hmgr->devices[i].on_rx_done = on_rx_done;
            hmgr->devices[i].on_error = on_error;
            return AM_OK;
        }
    }

    return AM_ERROR;
}

AM_Result I2C_Manager_RequestBus(I2C_ManagerHandle *hmgr, I2C_DeviceID id)
{
    return I2C_Manager_RequestBusPrio(hmgr, id, I2C_REQ_PRIO_REGULAR);
}

AM_Result I2C_Manager_RequestBusPrio(I2C_ManagerHandle *hmgr,
                                             I2C_DeviceID id,
                                             I2C_RequestPrio prio)
{
    if (!hmgr) return AM_ERROR;

    uint8_t idx = find_index(hmgr, id);
    if (idx == IDX_INVALID) return AM_ERROR;

    // If already owner, nothing to do (device can continue its multi-transfer sequence)
    if (hmgr->owner_id == id) {
        return AM_OK;
    }

    // Mark pending
    hmgr->devices[idx].pending = 1;
    if (hmgr->devices[idx].pending_prio < prio) {
        hmgr->devices[idx].pending_prio = prio;
    }

    /* Accepted requests are dispatched exclusively by the main-loop Tick. */
    return hmgr->owner_id == (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE ? AM_OK : AM_BUSY;
}

AM_Result I2C_Manager_ReleaseBus(I2C_ManagerHandle *hmgr, I2C_DeviceID id)
{
    if (!hmgr) return AM_ERROR;

    if (hmgr->owner_id != id) {
        return AM_ERROR; // not owner
    }

    // Release
    
    hmgr->last_owner_id = hmgr->owner_id;
    hmgr->owner_id = (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE;
    hmgr->owner_index = IDX_INVALID;
    hmgr->lease_start_tick = 0;

    return AM_OK;
}

AM_Result I2C_Manager_SetPolicy(I2C_ManagerHandle *hmgr, I2C_PolicyMode policy)
{
    if (!hmgr) return AM_ERROR;

    if (policy != I2C_POLICY_BALANCED &&
        policy != I2C_POLICY_PRIORITIZE_MPU &&
        policy != I2C_POLICY_PRIORITIZE_OLED) {
        return AM_ERROR;
    }

    hmgr->policy = policy;
    return AM_OK;
}

I2C_PolicyMode I2C_Manager_GetPolicy(const I2C_ManagerHandle *hmgr)
{
    if (!hmgr) return I2C_POLICY_BALANCED;
    return hmgr->policy;
}

I2C_DeviceID I2C_Manager_GetOwner(I2C_ManagerHandle *hmgr)
{
    if (!hmgr) return (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE;
    return hmgr->owner_id;
}

uint16_t I2C_Manager_GetAddress7(I2C_ManagerHandle *hmgr, I2C_DeviceID id)
{
    if (!hmgr) return 0xFFFFu;
    uint8_t idx = find_index(hmgr, id);
    if (idx == IDX_INVALID) return 0xFFFFu;
    return hmgr->devices[idx].addr_7bit;
}

AM_Result I2C_Manager_IsAddressReady(I2C_ManagerHandle *hmgr, uint16_t addr_7bit)
{
    if (!hmgr || !hmgr->hi2c) return AM_ERROR;
    return hmgr->hardware->probe(hmgr->hi2c, (uint16_t)(addr_7bit << 1), 2, 10);
}

// ---------- IRQ event publication ----------
void I2C_Manager_OnTxCplt(I2C_ManagerHandle *hmgr, void *hi2c)
{
    if (!hmgr || !hi2c) return;
    if (hmgr->hi2c != hi2c) return;
    hmgr->tx_complete_pending = 1u;
}

void I2C_Manager_OnRxCplt(I2C_ManagerHandle *hmgr, void *hi2c)
{
    if (!hmgr || !hi2c) return;
    if (hmgr->hi2c != hi2c) return;

    hmgr->rx_complete_pending = 1u;
}

void I2C_Manager_OnError(I2C_ManagerHandle *hmgr, void *hi2c)
{
    if (!hmgr || !hi2c) return;
    if (hmgr->hi2c != hi2c) return;

    hmgr->error_pending = 1u;
}

static void recover_owner(I2C_ManagerHandle *hmgr, I2C_ManagerError error)
{
    I2C_DeviceEntry *dev = hmgr->owner_index < I2C_MANAGER_MAX_DEVICES
        ? &hmgr->devices[hmgr->owner_index] : NULL;

    /* No callback can grant another device while recovery is in progress. */
    hmgr->last_owner_id = hmgr->owner_id;
    hmgr->owner_id = (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE;
    hmgr->owner_index = IDX_INVALID;
    hmgr->lease_start_tick = 0u;
    if (hmgr->recover_cb) {
        hmgr->recover_cb(hmgr->hi2c);
    }
    /* Hardware recovery has stopped DMA before clients reuse their buffers. */
    hmgr->tx_complete_pending = 0u;
    hmgr->rx_complete_pending = 0u;
    hmgr->error_pending = 0u;
    hmgr->bus_waiting = 0u;
    if (dev && dev->enabled && dev->on_error) {
        dev->on_error(dev->ctx, error);
    }
}

// ---------- Main-loop service ----------
void I2C_Manager_Tick(I2C_ManagerHandle *hmgr)
{
    if (!hmgr) return;

    if (hmgr->error_pending) {
        if (hmgr->owner_index == IDX_INVALID) {
            hmgr->spurious_err++;
        }
        recover_owner(hmgr, I2C_MAN_ERR_INTERNAL);
        return;
    }

    if (hmgr->tx_complete_pending) {
        hmgr->tx_complete_pending = 0u;
        if (hmgr->owner_index < I2C_MANAGER_MAX_DEVICES) {
            I2C_DeviceEntry *dev = &hmgr->devices[hmgr->owner_index];
            hmgr->lease_start_tick = hmgr->hardware->now_ms(hmgr->hi2c);
            if (dev->enabled && dev->on_tx_done) dev->on_tx_done(dev->ctx);
        } else {
            hmgr->spurious_tx++;
        }
        return;
    }

    if (hmgr->rx_complete_pending) {
        hmgr->rx_complete_pending = 0u;
        if (hmgr->owner_index < I2C_MANAGER_MAX_DEVICES) {
            I2C_DeviceEntry *dev = &hmgr->devices[hmgr->owner_index];
            hmgr->lease_start_tick = hmgr->hardware->now_ms(hmgr->hi2c);
            if (dev->enabled && dev->on_rx_done) dev->on_rx_done(dev->ctx);
        } else {
            hmgr->spurious_rx++;
        }
        return;
    }

    const uint32_t now = hmgr->hardware->now_ms(hmgr->hi2c);
    if (hmgr->owner_id == (I2C_DeviceID)I2C_MANAGER_DEVICE_NONE) {
        uint8_t next = pick_next_pending_index(hmgr);
        if (next == IDX_INVALID) {
            hmgr->bus_waiting = 0u;
            return;
        }
        if (!hmgr->hardware->ready(hmgr->hi2c)) {
            if (!hmgr->bus_waiting) {
                hmgr->bus_waiting = 1u;
                hmgr->bus_wait_started_tick = now;
            } else if (hmgr->lease_timeout_ms != 0u &&
                       (uint32_t)(now - hmgr->bus_wait_started_tick) >= hmgr->lease_timeout_ms) {
                recover_owner(hmgr, I2C_MAN_ERR_TIMEOUT);
            }
            return;
        }
        hmgr->bus_waiting = 0u;
        grant_to_index(hmgr, next);
        return;
    }

    if (hmgr->lease_timeout_ms != 0u &&
        (uint32_t)(now - hmgr->lease_start_tick) >= hmgr->lease_timeout_ms) {
        recover_owner(hmgr, I2C_MAN_ERR_TIMEOUT);
    }
}
