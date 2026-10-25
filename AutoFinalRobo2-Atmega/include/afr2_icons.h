/* AFR2-20261024-08. Selected F4 XBM resources, AVR PROGMEM access. */
#ifndef AFR2_ICONS_H
#define AFR2_ICONS_H
#include "afr2_oled.h"
enum {
 AFR2_ICON_INFO, AFR2_ICON_WIFI, AFR2_ICON_BACK, AFR2_ICON_SENSORS,
 AFR2_ICON_CONFIG, AFR2_ICON_REFRESH, AFR2_ICON_LINK, AFR2_ICON_ENCODER,
 AFR2_ICON_TOOLS, AFR2_ICON_PHONE, AFR2_ICON_AP, AFR2_ICON_WARNING,
 AFR2_ICON_MOTOR, AFR2_ICON_PREFS, AFR2_ICON_COUNT
};
void Afr2Icon_Draw(Afr2Oled *o,uint8_t id,int16_t x,int16_t y,uint8_t size);
uint8_t Afr2Menu_Icon(uint8_t node,uint8_t target,uint8_t action,const char *label);
#endif
