/*
 * ssd1306.h
 *
 *  Created on: 14/04/2018
 *  Update on: 10/04/2019
 *      Author: Andriy Honcharenko
 *      version: 2
 *
 *  Modify on: 22/10/2021
 *      Author: Roberto Benjami
 *  Added features in DMA mode:
 *  - ssd1306_UpdateScreen_H works without blocking
 *  - you can query that UpdateScreen is complete (if ssd1306_UpdateScreenCompleted_H(h, ) == 1)
 *  - callback function if UpdateScreen is complete (ssd1306_UpdateCompletedCallback)
 *  Added features in DMA mode with continuous display update:
 *  - enable continuous display update
 *  - disable continuous display update
 *  - enable raster interrupt(s) of PAGEx (you can set which PAGE(s) )
 */

#ifndef SSD1306_H_DATA
#define SSD1306_H_DATA

#include "ssd1306_defines.h"
#include "i2c_port.h"
#include "fonts.h"

#include <stdlib.h>
#include <string.h>

// I2c address
#define SSD1306_I2C_ADDR       SSD1306_ADDRESS << 1 // 0x3C << 1 = 0x78

#ifdef  SSD1306_128X64
#define SSD1306_GEOMETRY       GEOMETRY_128_64
// SSD1306 width in pixels
#define SSD1306_WIDTH          128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT         64
#elif defined(SSD1306_128X32)
#define SSD1306_GEOMETRY       GEOMETRY_128_32
// SSD1306 width in pixels
#define SSD1306_WIDTH          128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT         32
#endif

// SSD1306 LCD Buffer Size
#define SSD1306_BUFFER_SIZE   (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

// Display commands
#define CHARGEPUMP            0x8D
#define COLUMNADDR            0x21
#define COMSCANDEC            0xC8
#define COMSCANINC            0xC0
#define DISPLAYALLON          0xA5
#define DISPLAYALLON_RESUME   0xA4
#define DISPLAYOFF            0xAE
#define DISPLAYON             0xAF
#define EXTERNALVCC           0x01
#define INVERTDISPLAY         0xA7
#define MEMORYMODE            0x20
#define NORMALDISPLAY         0xA6
#define PAGEADDR              0x22
#define SEGREMAP              0xA0
#define SETCOMPINS            0xDA
#define SETCONTRAST           0x81
#define SETDISPLAYCLOCKDIV    0xD5
#define SETDISPLAYOFFSET      0xD3
#define SETHIGHCOLUMN         0x10
#define SETLOWCOLUMN          0x00
#define SETMULTIPLEX          0xA8
#define SETPRECHARGE          0xD9
#define SETSEGMENTREMAP       0xA1
#define SETSTARTLINE          0x40
#define SETVCOMDETECT         0xDB
#define SWITCHCAPVCC          0x02

#define SSD1306_CONTRAST_128X64_DEFAULT 0xCFu
#define SSD1306_CONTRAST_128X32_DEFAULT 0x8Fu

#define SWAP_INT16_T(a, b) { int16_t t = a; a = b; b = t; }
//
//  Enumeration for screen colors
//
typedef enum {
  Black = 0x00,  // Black color, no pixel
  White = 0x01,  // Pixel is set. Color depends on LCD
  Inverse = 0x02
} SSD1306_COLOR;

typedef enum {
  GEOMETRY_128_64 = 0,
  GEOMETRY_128_32 = 1
} SSD1306_Geometry;

typedef enum {
  OLED_ROTATION_0 = 0u,
  OLED_ROTATION_180 = 180u
} OledRotation_t;
//
//  Struct to store transformations
//
typedef struct {
  uint16_t      CurrentX;
  uint16_t      CurrentY;
  uint8_t       Inverted;
  SSD1306_COLOR Color;
  uint8_t       Initialized;
} SSD1306_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} SSD1306_VERTEX;

//  Definition of the i2c port in main



typedef enum {
  OLED_SM_IDLE = 0,
  OLED_SM_CMD_PAGE,
  OLED_SM_CMD_LOWCOL,
  OLED_SM_CMD_HIGHCOL,
  OLED_SM_DATA,
  OLED_SM_PAGE_DONE
} OLED_SM_State;

typedef struct {
  uint8_t used;
  uint8_t page;
  I2C_RequestPrio prio;
  uint8_t keep_bus;
  uint32_t seq;
  OLED_SM_State state;
  uint8_t pending_refresh;
} OLED_PageReq;

typedef struct {
    const AM_I2C_DevicePort *bus;
    void (*on_raster)(void *ctx, uint8_t raster);
    void *callback_ctx;
    void (*on_page)(void *ctx,uint8_t page);
    void (*on_presented)(void *ctx);
    void (*on_empty)(void *ctx);
    SSD1306_t SSD1306;
    uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
    volatile uint32_t oled_error_count;
    OledRotation_t oled_rotation;
    OLED_PageReq g_page_q[8];
    uint32_t g_seq;
    uint8_t oled_inflight_page;
    uint8_t oled_direct_pending;
    uint8_t oled_direct_inflight;
    uint8_t oled_direct_pending_len;
    uint8_t oled_direct_active_len;
    uint8_t oled_direct_pos;
    uint8_t oled_direct_pending_buf[4];
    uint8_t oled_direct_active_buf[4];
    volatile uint8_t oled_page;
    uint8_t oled_cmd_byte;
    volatile uint8_t ssd1306_updatestatus;
    volatile uint8_t ssd1306_updateend;
    volatile uint8_t ssd1306_command;
    volatile uint8_t i2c_command;
    volatile uint8_t ssd1306_ContUpdate;
    volatile uint8_t ssd1306_RasterIntRegs;
    SSD1306_Geometry display_geometry;
} SSD1306_Handle;

/* Private function prototypes -----------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif
