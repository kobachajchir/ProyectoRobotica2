#include "afr2_i2c_manager.h"
#include <string.h>
bool Afr2I2c_Init(Afr2I2cManager *m,const Afr2I2cHardware *hw) {
    if (!m) return false;
    memset(m,0,sizeof(*m));
    if (!hw || !hw->start || !hw->poll || !hw->abort) return false;
    m->hardware=*hw; m->configured=true; return true;
}
Afr2I2cResult Afr2I2c_Submit(Afr2I2cManager *m,uint8_t addr,const uint8_t *data,
        uint16_t size,uint32_t now,uint16_t timeout,Afr2I2cDone done,void *client) {
    if (!m || !m->configured || !data || !size || addr>0x7fu || !timeout) return AFR2_I2C_ERROR;
    if (m->busy) return AFR2_I2C_BUSY;
    Afr2I2cResult result=m->hardware.start(m->hardware.ctx,addr,data,size);
    if (result!=AFR2_I2C_OK) return result;
    m->busy=true; m->started_ms=now; m->timeout_ms=timeout; m->done=done; m->client=client;
    return AFR2_I2C_OK;
}
void Afr2I2c_Tick(Afr2I2cManager *m,uint32_t now) {
    if (!m || !m->busy) return;
    Afr2I2cResult result=m->hardware.poll(m->hardware.ctx);
    if (result==AFR2_I2C_BUSY && (uint32_t)(now-m->started_ms)<m->timeout_ms) return;
    if (result==AFR2_I2C_BUSY) result=AFR2_I2C_TIMEOUT;
    if (result!=AFR2_I2C_OK) m->hardware.abort(m->hardware.ctx);
    Afr2I2cDone cb=m->done; void *ctx=m->client;
    m->busy=false; m->done=0; m->client=0;
    if (cb) cb(ctx,result); /* main: nunca dentro de la ISR TWI */
}
