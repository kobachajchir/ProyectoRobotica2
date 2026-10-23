/*
 * oled_canvas.h
 *
 * Transactional 128x64 monochrome framebuffer receiver used by the
 * Testeo > Pantalla > OLED Canvas screen.
 */
#ifndef INC_OLED_CANVAS_H_
#define INC_OLED_CANVAS_H_

#include <stdint.h>

#include "types/userEvent_type.h"
#include "uner_core_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OLED_CANVAS_WIDTH                         128u
#define OLED_CANVAS_HEIGHT                         64u
#define OLED_CANVAS_FRAMEBUFFER_BYTES            1024u
#define OLED_CANVAS_UART_CHUNK_BYTES              112u
#define OLED_CANVAS_FORMAT_SSD1306_PAGE_LSB         0u
#define OLED_CANVAS_ASSEMBLY_TIMEOUT_MS          3000u
#define OLED_CANVAS_DISPLAY_TIMEOUT_MS           1000u
#define OLED_CANVAS_RESPONSE_BYTES                 10u

typedef enum {
    OLED_CANVAS_OP_BEGIN  = 0x01u,
    OLED_CANVAS_OP_CHUNK  = 0x02u,
    OLED_CANVAS_OP_COMMIT = 0x03u,
    OLED_CANVAS_OP_CANCEL = 0x04u,
} OledCanvasOperation_t;

typedef enum {
    OLED_CANVAS_STATUS_OK                   = 0u,
    OLED_CANVAS_STATUS_NOT_TEST_MODE        = 1u,
    OLED_CANVAS_STATUS_WRONG_SCREEN         = 2u,
    OLED_CANVAS_STATUS_BUSY                 = 3u,
    OLED_CANVAS_STATUS_BAD_METADATA         = 4u,
    OLED_CANVAS_STATUS_OUT_OF_ORDER         = 5u,
    OLED_CANVAS_STATUS_CRC_MISMATCH         = 6u,
    OLED_CANVAS_STATUS_TIMEOUT              = 7u,
    OLED_CANVAS_STATUS_DISPLAY_IO_ERROR     = 8u,
    OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI = 9u,
} OledCanvasStatus_t;

typedef uint8_t (*OledCanvasSendResponseFn)(uint8_t transport_id,
                                             uint8_t destination_node,
                                             const uint8_t *payload,
                                             uint8_t len,
                                             void *ctx);

void OledCanvas_Init(OledCanvasSendResponseFn send_response, void *ctx);
void OledCanvas_BeginFromMenu(void);
void OledCanvas_RenderReadyScreen(void);
void OledCanvas_HandleLocalEvent(UserEvent_t event);
void OledCanvas_HandlePacket(const UNER_Packet *packet);
void OledCanvas_Poll(void);
uint8_t OledCanvas_IsActiveScreen(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_OLED_CANVAS_H_ */
