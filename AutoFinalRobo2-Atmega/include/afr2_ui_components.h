#ifndef AFR2_UI_COMPONENTS_H
#define AFR2_UI_COMPONENTS_H
#include "afr2_oled.h"
void Afr2Component_Header(Afr2Oled *,uint8_t,const char *,uint8_t,uint8_t);
void Afr2Component_Panel(Afr2Oled *,uint8_t,uint8_t,uint8_t,uint8_t);
void Afr2Component_Progress(Afr2Oled *,uint8_t,uint8_t,uint8_t,uint32_t);
void Afr2Component_Mini(Afr2Oled *,uint8_t,uint8_t,uint16_t);
void Afr2Component_Metric(Afr2Oled *,uint8_t,uint8_t,const char *);
#endif
