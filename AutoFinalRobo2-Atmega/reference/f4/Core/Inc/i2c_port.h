#ifndef AM_I2C_PORT_H
#define AM_I2C_PORT_H
#include "am_port.h"
// ---------- Device IDs (internal logical IDs) ----------
typedef uint8_t I2C_DeviceID;

// ---------- Errors ----------
typedef enum {
    I2C_MAN_ERR_NONE = 0,
    I2C_MAN_ERR_NOT_REGISTERED,
    I2C_MAN_ERR_NOT_OWNER,
    I2C_MAN_ERR_TIMEOUT,
    I2C_MAN_ERR_INTERNAL
} I2C_ManagerError;

// ---------- Callbacks ----------
typedef void (*I2C_OnGrantedCb)(void *ctx);
typedef void (*I2C_OnTxDoneCb)(void *ctx);
typedef void (*I2C_OnRxDoneCb)(void *ctx);
typedef void (*I2C_OnErrorCb)(void *ctx, I2C_ManagerError err);

// ---------- Request priority ----------
typedef enum {
    I2C_REQ_PRIO_LOW = 0,
    I2C_REQ_PRIO_REGULAR = 1,
    I2C_REQ_PRIO_HIGH = 2
} I2C_RequestPrio;

typedef enum {
    I2C_POLICY_BALANCED = 1,
    I2C_POLICY_PRIORITIZE_MPU = 2,
    I2C_POLICY_PRIORITIZE_OLED = 3
} I2C_PolicyMode;

#define I2C_MANAGER_DEFAULT_LEASE_TIMEOUT_MS 120u


/* The driver sees only this contract. Backend values match the existing bus API. */
typedef struct {
    uint32_t (*now_ms)(void *ctx);
    uint32_t (*now_us)(void *ctx);
    void (*delay_ms)(void *ctx,uint32_t ms); /* compatibility initialization only */
    bool (*ready)(void *ctx);
    AM_Result (*probe)(void *ctx,uint16_t address,uint32_t trials,uint32_t timeout);
    AM_Result (*mem_write)(void *ctx,uint16_t address,uint16_t reg,uint16_t reg_size,uint8_t *data,uint16_t len,uint32_t timeout);
    AM_Result (*mem_read)(void *ctx,uint16_t address,uint16_t reg,uint16_t reg_size,uint8_t *data,uint16_t len,uint32_t timeout);
    AM_Result (*mem_write_dma)(void *ctx,uint16_t address,uint16_t reg,uint16_t reg_size,uint8_t *data,uint16_t len);
    AM_Result (*tx_dma)(void *ctx,uint16_t address,uint8_t *data,uint16_t len);
    AM_Result (*rx_dma)(void *ctx,uint16_t address,uint8_t *data,uint16_t len);
} AM_I2C_Hardware;
typedef struct {
    void *ctx;
    const AM_I2C_Hardware *hardware;
    void *hardware_ctx;
    AM_Result (*bind)(void *ctx,uint16_t address,uint8_t priority,void *client,
                      I2C_OnGrantedCb,I2C_OnTxDoneCb,I2C_OnRxDoneCb,I2C_OnErrorCb);
    AM_Result (*request)(void *ctx,I2C_RequestPrio priority);
    AM_Result (*release)(void *ctx);
    bool (*is_owner)(void *ctx);
    I2C_PolicyMode (*policy)(void *ctx);
    void (*transfer_failed)(void *ctx); /* latch a failed DMA start for recovery */
} AM_I2C_DevicePort;
#endif
