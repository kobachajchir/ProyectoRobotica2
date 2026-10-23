/*
 * i2c_manager.h
 *
 *  Created on: Jun 7, 2025
 *  Updated on: Jan 12, 2026
 *      Author: kobac (updated w/ arbitration lease)
 *
 *  I2C Manager = bus arbiter + callback router
 *  - Devices register with callbacks.
 *  - A device requests the bus; if granted, manager calls on_granted(ctx).
 *  - Device keeps ownership until it calls ReleaseBus().
 *  - When DMA TX/RX completes, HAL callbacks must forward to I2C_Manager_OnTxCplt/OnRxCplt,
 *    which latch events. I2C_Manager_Tick dispatches them in main context.
 */

#ifndef INC_I2C_MANAGER_H_
#define INC_I2C_MANAGER_H_

#include "i2c_port.h"
#define DEVICE_ID_OLED ((I2C_DeviceID)1u)
#define DEVICE_ID_MPU ((I2C_DeviceID)2u)
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_MANAGER_MAX_DEVICES        2
#define I2C_MANAGER_DEVICE_NONE        0xFFu

// ---------- Device entry ----------
typedef struct {
    I2C_DeviceID id;           // internal ID
    uint16_t     addr_7bit;    // 7-bit address (0x3C, 0x68, etc.)
    uint8_t      priority;     // 0..15 (optional use)
    uint8_t      enabled;      // 0/1
    uint8_t      pending;      // 0/1 (wants bus)
    I2C_RequestPrio pending_prio; // accumulated pending prio (max)

    void        *ctx;          // user context (OLED handle*, MPU handle*, etc.)
    I2C_OnGrantedCb on_granted;
    I2C_OnTxDoneCb  on_tx_done;
    I2C_OnRxDoneCb  on_rx_done;
    I2C_OnErrorCb   on_error;
} I2C_DeviceEntry;

// ---------- Manager handle ----------
typedef struct {
    void *hi2c;
    const AM_I2C_Hardware *hardware;

    I2C_DeviceEntry devices[I2C_MANAGER_MAX_DEVICES];

    I2C_DeviceID owner_id;       // current bus owner (or NONE)
    I2C_DeviceID last_owner_id;  // last owner for fairness
    uint8_t      owner_index;    // cached index of owner in table (or 0xFF)

    uint32_t lease_start_tick;   // for timeout watchdog
    uint32_t lease_timeout_ms;   // 0 disables timeout
    I2C_PolicyMode policy;
    void (*recover_cb)(void *hi2c); // optional recovery hook

    /* IRQs only publish these latches; device callbacks run from Tick(). */
    volatile uint8_t tx_complete_pending;
    volatile uint8_t rx_complete_pending;
    volatile uint8_t error_pending;
    uint8_t bus_waiting;
    uint32_t bus_wait_started_tick;

    uint32_t spurious_tx;
    uint32_t spurious_rx;
    uint32_t spurious_err;

} I2C_ManagerHandle;

// ---------- API ----------
AM_Result I2C_Manager_Init(I2C_ManagerHandle *hmgr,
                                  const AM_I2C_Hardware *hardware,
                                  void *hi2c,
                                  uint32_t lease_timeout_ms);

AM_Result I2C_Manager_RegisterDevice(I2C_ManagerHandle *hmgr,
                                            I2C_DeviceID id,
                                            uint16_t addr_7bit,
                                            uint8_t priority,
                                            void *ctx,
                                            I2C_OnGrantedCb on_granted,
                                            I2C_OnTxDoneCb on_tx_done,
                                            I2C_OnRxDoneCb on_rx_done,
                                            I2C_OnErrorCb on_error);

AM_Result I2C_Manager_RequestBusPrio(I2C_ManagerHandle *hmgr,
                                             I2C_DeviceID id,
                                             I2C_RequestPrio prio);
AM_Result I2C_Manager_RequestBus(I2C_ManagerHandle *hmgr, I2C_DeviceID id);
AM_Result I2C_Manager_ReleaseBus(I2C_ManagerHandle *hmgr, I2C_DeviceID id);
AM_Result I2C_Manager_SetPolicy(I2C_ManagerHandle *hmgr, I2C_PolicyMode policy);
I2C_PolicyMode I2C_Manager_GetPolicy(const I2C_ManagerHandle *hmgr);

I2C_DeviceID I2C_Manager_GetOwner(I2C_ManagerHandle *hmgr);
uint16_t I2C_Manager_GetAddress7(I2C_ManagerHandle *hmgr, I2C_DeviceID id);

AM_Result I2C_Manager_IsAddressReady(I2C_ManagerHandle *hmgr, uint16_t addr_7bit);

// IRQ-safe event publication; these functions never call a device or start DMA.
void I2C_Manager_OnTxCplt(I2C_ManagerHandle *hmgr, void *hi2c);
void I2C_Manager_OnRxCplt(I2C_ManagerHandle *hmgr, void *hi2c);
void I2C_Manager_OnError(I2C_ManagerHandle *hmgr, void *hi2c);

// Required main-loop service: dispatch callbacks, arbitrate and recover the bus.
void I2C_Manager_Tick(I2C_ManagerHandle *hmgr);

#ifdef __cplusplus
}
#endif

#endif /* INC_I2C_MANAGER_H_ */
