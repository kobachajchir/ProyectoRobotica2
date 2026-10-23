#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OLED_CANVAS_COMMAND_ID 0x6Du
#define OLED_CANVAS_WIDTH 128u
#define OLED_CANVAS_HEIGHT 64u
#define OLED_CANVAS_BYTES 1024u
#define OLED_CANVAS_WEB_CHUNK_BYTES 512u
#define OLED_CANVAS_F4_CHUNK_BYTES 112u
#define OLED_CANVAS_F4_RESPONSE_BYTES 10u
#define OLED_CANVAS_SCREEN_CODE 0x00030503UL
#define OLED_CANVAS_FORMAT_PAGE_LSB 0u

typedef enum {
    OLED_CANVAS_OP_BEGIN = 0x01u,
    OLED_CANVAS_OP_CHUNK = 0x02u,
    OLED_CANVAS_OP_COMMIT = 0x03u,
    OLED_CANVAS_OP_CANCEL = 0x04u,
} OledCanvasOperation;

typedef enum {
    OLED_CANVAS_STATUS_OK = 0u,
    OLED_CANVAS_STATUS_NOT_TEST_MODE = 1u,
    OLED_CANVAS_STATUS_WRONG_SCREEN = 2u,
    OLED_CANVAS_STATUS_BUSY = 3u,
    OLED_CANVAS_STATUS_BAD_METADATA = 4u,
    OLED_CANVAS_STATUS_OUT_OF_ORDER = 5u,
    OLED_CANVAS_STATUS_CRC_MISMATCH = 6u,
    OLED_CANVAS_STATUS_TIMEOUT = 7u,
    OLED_CANVAS_STATUS_DISPLAY_IO_ERROR = 8u,
    OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI = 9u,
} OledCanvasStatus;

typedef enum {
    OLED_CANVAS_ASSEMBLY_IDLE = 0u,
    OLED_CANVAS_ASSEMBLY_RECEIVING,
    OLED_CANVAS_ASSEMBLY_READY,
} OledCanvasAssemblyState;

typedef enum {
    OLED_CANVAS_RESULT_OK = 0u,
    OLED_CANVAS_RESULT_DUPLICATE,
    OLED_CANVAS_RESULT_INVALID,
    OLED_CANVAS_RESULT_WRONG_TRANSFER,
    OLED_CANVAS_RESULT_OUT_OF_ORDER,
    OLED_CANVAS_RESULT_DATA_MISMATCH,
    OLED_CANVAS_RESULT_INCOMPLETE,
    OLED_CANVAS_RESULT_CRC_MISMATCH,
} OledCanvasResult;

typedef struct {
    uint8_t bytes[OLED_CANVAS_BYTES];
    uint16_t transfer_id;
    uint16_t next_offset;
    uint32_t expected_crc32;
    uint32_t computed_crc32;
    OledCanvasAssemblyState state;
} OledCanvasAssembly;

typedef struct {
    uint8_t operation;
    uint8_t status;
    uint16_t transfer_id;
    uint16_t next_offset;
    uint32_t computed_crc32;
} OledCanvasF4Response;

void OledCanvasAssembly_Init(OledCanvasAssembly *assembly);
OledCanvasResult OledCanvasAssembly_Begin(OledCanvasAssembly *assembly,
                                          uint16_t transfer_id,
                                          uint32_t expected_crc32);
OledCanvasResult OledCanvasAssembly_Append(OledCanvasAssembly *assembly,
                                           uint16_t transfer_id,
                                           uint16_t offset,
                                           const uint8_t *data,
                                           size_t data_len);
OledCanvasResult OledCanvasAssembly_Commit(OledCanvasAssembly *assembly,
                                           uint16_t transfer_id);
void OledCanvasAssembly_ScrubFramebuffer(OledCanvasAssembly *assembly);

uint32_t OledCanvas_Crc32(const uint8_t *data, size_t len);
int OledCanvas_ParseCrc32Hex(const char *text, uint32_t *value);
void OledCanvas_FormatCrc32Hex(uint32_t value, char output[9]);
int OledCanvas_DecodeBase64(const char *input, size_t input_len,
                            uint8_t *output, size_t output_capacity,
                            size_t *output_len);

size_t OledCanvas_BuildBeginPayload(const OledCanvasAssembly *assembly,
                                    uint8_t *output, size_t output_capacity);
size_t OledCanvas_BuildChunkPayload(const OledCanvasAssembly *assembly,
                                    uint16_t offset,
                                    uint8_t *output, size_t output_capacity);
size_t OledCanvas_BuildControlPayload(uint8_t operation, uint16_t transfer_id,
                                      uint8_t *output, size_t output_capacity);
int OledCanvas_ParseF4Response(const uint8_t *payload, size_t payload_len,
                               OledCanvasF4Response *response);
int OledCanvas_F4ResponseMatchesExpected(const uint8_t *payload,
                                         size_t payload_len,
                                         uint8_t expected_operation,
                                         uint16_t expected_transfer_id,
                                         uint16_t expected_next_offset);

#ifdef __cplusplus
}
#endif
