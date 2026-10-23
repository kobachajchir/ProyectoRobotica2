#ifndef SSD1306_H_
#define SSD1306_H_
#include "ssd1306_types.h"
#ifdef __cplusplus
extern "C" {
#endif
uint16_t ssd1306_GetWidth_H(SSD1306_Handle *h);
uint16_t ssd1306_GetHeight_H(SSD1306_Handle *h);
SSD1306_COLOR ssd1306_GetColor_H(SSD1306_Handle *h);
void ssd1306_SetColor_H(SSD1306_Handle *h, SSD1306_COLOR color);
uint8_t ssd1306_Init_H(SSD1306_Handle *h);
void ssd1306_Fill_H(SSD1306_Handle *h);
void ssd1306_DrawPixel_H(SSD1306_Handle *h, uint8_t x, uint8_t y);
void ssd1306_DrawBitmap_H(SSD1306_Handle *h, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP);
void ssd1306_DrawBitmap_MSB_H(SSD1306_Handle *h, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP);
void ssd1306_DrawQR_Fixed_H(SSD1306_Handle *h, uint8_t startX, uint8_t startY, const uint8_t* qrData);
void ssd1306_DrawLine_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void ssd1306_DrawVerticalLine_H(SSD1306_Handle *h, int16_t x, int16_t y, int16_t length);
void ssd1306_DrawHorizontalLine_H(SSD1306_Handle *h, int16_t x, int16_t y, int16_t length);
void ssd1306_DrawRect_H(SSD1306_Handle *h, int16_t x, int16_t y, int16_t width, int16_t height);
void ssd1306_DrawTriangle_H(SSD1306_Handle *h, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
void ssd1306_FillRect_H(SSD1306_Handle *h, int16_t xMove, int16_t yMove, int16_t width, int16_t height);
void ssd1306_DrawArc_H(SSD1306_Handle *h, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep);
void ssd1306_DrawCircle_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t radius);
void ssd1306_FillCircle_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t radius);
void ssd1306_Polyline_H(SSD1306_Handle *h, const SSD1306_VERTEX *par_vertex, uint16_t par_size);
void ssd1306_DrawCircleQuads_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t radius, uint8_t quads);
void ssd1306_DrawProgressBar_H(SSD1306_Handle *h, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress);
void ssd1306_DrawLinearProgress_H(SSD1306_Handle *h, uint8_t x,
                                uint8_t y,
                                uint8_t width,
                                uint8_t height,
                                uint32_t elapsed,
                                uint32_t total);
char ssd1306_WriteChar_H(SSD1306_Handle *h, char ch, FontDef Font);
char ssd1306_WriteString_H(SSD1306_Handle *h, char* str, FontDef Font);
void ssd1306_SetCursor_H(SSD1306_Handle *h, uint8_t x, uint8_t y);
void ssd1306_Clear_H(SSD1306_Handle *h);
AM_Result ssd1306_PresentFramebuffer_H(SSD1306_Handle *h, const uint8_t *framebuffer,
                                             uint16_t len);
uint32_t ssd1306_GetErrorCount_H(SSD1306_Handle *h);

void ssd1306_WriteCommand_H(SSD1306_Handle *h, uint8_t command);
AM_Result ssd1306_SetContrast_H(SSD1306_Handle *h, uint8_t contrast);
uint8_t ssd1306_GetDefaultContrast_H(SSD1306_Handle *h);
AM_Result ssd1306_SetRotation_H(SSD1306_Handle *h, OledRotation_t rotation);
OledRotation_t ssd1306_GetRotation_H(SSD1306_Handle *h);

#define ssd1306_DisplayOn(h)             ssd1306_WriteCommand_H(h, DISPLAYON)
#define ssd1306_DisplayOff(h)            ssd1306_WriteCommand_H(h, DISPLAYOFF)
#define ssd1306_InvertDisplay(h)         ssd1306_WriteCommand_H(h, INVERTDISPLAY)
#define ssd1306_NormalDisplay(h)         ssd1306_WriteCommand_H(h, NORMALDISPLAY)
#define ssd1306_ResetOrientation(h)      { ssd1306_WriteCommand_H(SSD1306_Handle *h, SEGREMAP); ssd1306_WriteCommand_H(SSD1306_Handle *h, COMSCANINC); }
#define ssd1306_FlipScreenVertically(h)  { ssd1306_WriteCommand_H(SSD1306_Handle *h, SEGREMAP | 0x01); ssd1306_WriteCommand_H(SSD1306_Handle *h, COMSCANDEC); }
#define ssd1306_MirrorScreen(h)          { ssd1306_WriteCommand_H(SSD1306_Handle *h, SEGREMAP | 0x01); ssd1306_WriteCommand_H(SSD1306_Handle *h, COMSCANINC); }
#define ssd1306_MirrorFlipScreen(h)      { ssd1306_WriteCommand_H(SSD1306_Handle *h, SEGREMAP); ssd1306_WriteCommand_H(SSD1306_Handle *h, COMSCANDEC); }

// Bind con el I2C Manager (una sola vez)
AM_Result ssd1306_BindPort_H(SSD1306_Handle *h, const AM_I2C_DevicePort *port);

AM_Result ssd1306_RequestPageUpdate_H(SSD1306_Handle *h, uint8_t page,
                                            I2C_RequestPrio prio,
                                            uint8_t keep_bus);

#if  SSD1306_USE_DMA == 0
void ssd1306_UpdateScreen_H(SSD1306_Handle *h);      /* copy the contents of the Screenbuffer (SSD1306_Buffer) to the display */
#define ssd1306_UpdateScreenCompleted_H(h) 1
#define ssd1306_ContUpdateEnable_H(h)
#define ssd1306_ContUpdateDisable_H(h)
#define ssd1306_SetRasterInt_H(h, r)
#elif SSD1306_USE_DMA == 1
#if   SSD1306_CONTUPDATE == 0
void ssd1306_UpdateScreen_H(SSD1306_Handle *h);      /* copy the contents of the Screenbuffer (SSD1306_Buffer) to the display */
char ssd1306_UpdateScreenCompleted_H(SSD1306_Handle *h); /* asks if the update is already complete (0:not completed, 1:completed) */
#define ssd1306_ContUpdateEnable_H(h)
#define ssd1306_ContUpdateDisable_H(h)
#define ssd1306_SetRasterInt_H(h, r)
#elif SSD1306_CONTUPDATE == 1
#define ssd1306_UpdateScreen_H(h)
#define ssd1306_UpdateScreenCompleted_H(h) 1
void ssd1306_ContUpdateEnable_H(SSD1306_Handle *h);  /* enable the continuous dsplay update in background (use DMA and interrupt) */
void ssd1306_ContUpdateDisable_H(SSD1306_Handle *h); /* disable the continuous dsplay update in background */
void ssd1306_SetRasterInt_H(SSD1306_Handle *h, uint8_t r); /* enable raster interrupt(s) of PAGEx (0:NONE, 1:PAGE0, 2:PAGE1, 4:PAGE2 ... 128:PAGE7, 255:All_PAGES) */
#endif
#endif

#endif /* SSD1306_H_ */
