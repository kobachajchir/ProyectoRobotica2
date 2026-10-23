#include "oled_canvas_transfer.h"

#include <string.h>

static void write_u16_le(uint16_t value, uint8_t *out)
{
    out[0] = (uint8_t)(value & 0xFFu);
    out[1] = (uint8_t)(value >> 8);
}

static void write_u32_le(uint32_t value, uint8_t *out)
{
    out[0] = (uint8_t)(value & 0xFFu);
    out[1] = (uint8_t)((value >> 8) & 0xFFu);
    out[2] = (uint8_t)((value >> 16) & 0xFFu);
    out[3] = (uint8_t)((value >> 24) & 0xFFu);
}

static uint16_t read_u16_le(const uint8_t *in)
{
    return (uint16_t)in[0] | ((uint16_t)in[1] << 8);
}

static uint32_t read_u32_le(const uint8_t *in)
{
    return (uint32_t)in[0] |
           ((uint32_t)in[1] << 8) |
           ((uint32_t)in[2] << 16) |
           ((uint32_t)in[3] << 24);
}

void OledCanvasAssembly_Init(OledCanvasAssembly *assembly)
{
    if (!assembly) return;
    memset(assembly, 0, sizeof(*assembly));
}

OledCanvasResult OledCanvasAssembly_Begin(OledCanvasAssembly *assembly,
                                          uint16_t transfer_id,
                                          uint32_t expected_crc32)
{
    if (!assembly || transfer_id == 0u) return OLED_CANVAS_RESULT_INVALID;
    memset(assembly->bytes, 0, sizeof(assembly->bytes));
    assembly->transfer_id = transfer_id;
    assembly->next_offset = 0u;
    assembly->expected_crc32 = expected_crc32;
    assembly->computed_crc32 = 0u;
    assembly->state = OLED_CANVAS_ASSEMBLY_RECEIVING;
    return OLED_CANVAS_RESULT_OK;
}

OledCanvasResult OledCanvasAssembly_Append(OledCanvasAssembly *assembly,
                                           uint16_t transfer_id,
                                           uint16_t offset,
                                           const uint8_t *data,
                                           size_t data_len)
{
    if (!assembly || !data || data_len != OLED_CANVAS_WEB_CHUNK_BYTES ||
        assembly->state != OLED_CANVAS_ASSEMBLY_RECEIVING)
        return OLED_CANVAS_RESULT_INVALID;
    if (transfer_id != assembly->transfer_id)
        return OLED_CANVAS_RESULT_WRONG_TRANSFER;
    if (offset != 0u && offset != OLED_CANVAS_WEB_CHUNK_BYTES)
        return OLED_CANVAS_RESULT_OUT_OF_ORDER;
    if ((size_t)offset + data_len > OLED_CANVAS_BYTES)
        return OLED_CANVAS_RESULT_INVALID;

    if (offset < assembly->next_offset)
    {
        return memcmp(&assembly->bytes[offset], data, data_len) == 0
                   ? OLED_CANVAS_RESULT_DUPLICATE
                   : OLED_CANVAS_RESULT_DATA_MISMATCH;
    }
    if (offset != assembly->next_offset)
        return OLED_CANVAS_RESULT_OUT_OF_ORDER;

    memcpy(&assembly->bytes[offset], data, data_len);
    assembly->next_offset = (uint16_t)(offset + data_len);
    return OLED_CANVAS_RESULT_OK;
}

OledCanvasResult OledCanvasAssembly_Commit(OledCanvasAssembly *assembly,
                                           uint16_t transfer_id)
{
    if (!assembly || assembly->state != OLED_CANVAS_ASSEMBLY_RECEIVING)
        return OLED_CANVAS_RESULT_INVALID;
    if (transfer_id != assembly->transfer_id)
        return OLED_CANVAS_RESULT_WRONG_TRANSFER;
    if (assembly->next_offset != OLED_CANVAS_BYTES)
        return OLED_CANVAS_RESULT_INCOMPLETE;

    assembly->computed_crc32 = OledCanvas_Crc32(assembly->bytes,
                                                sizeof(assembly->bytes));
    if (assembly->computed_crc32 != assembly->expected_crc32)
        return OLED_CANVAS_RESULT_CRC_MISMATCH;
    assembly->state = OLED_CANVAS_ASSEMBLY_READY;
    return OLED_CANVAS_RESULT_OK;
}

void OledCanvasAssembly_ScrubFramebuffer(OledCanvasAssembly *assembly)
{
    if (!assembly) return;
    memset(assembly->bytes, 0, sizeof(assembly->bytes));
    assembly->computed_crc32 = 0u;
}

uint32_t OledCanvas_Crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    size_t i;
    uint8_t bit;
    if (!data && len != 0u) return 0u;
    for (i = 0u; i < len; ++i)
    {
        crc ^= data[i];
        for (bit = 0u; bit < 8u; ++bit)
        {
            if ((crc & 1u) != 0u) crc = (crc >> 1) ^ 0xEDB88320u;
            else crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

int OledCanvas_ParseCrc32Hex(const char *text, uint32_t *value)
{
    uint32_t parsed = 0u;
    size_t i;
    if (!text || !value || strlen(text) != 8u) return 0;
    for (i = 0u; i < 8u; ++i)
    {
        uint8_t digit;
        char ch = text[i];
        if (ch >= '0' && ch <= '9') digit = (uint8_t)(ch - '0');
        else if (ch >= 'A' && ch <= 'F') digit = (uint8_t)(ch - 'A' + 10);
        else return 0;
        parsed = (parsed << 4) | digit;
    }
    *value = parsed;
    return 1;
}

void OledCanvas_FormatCrc32Hex(uint32_t value, char output[9])
{
    static const char hex[] = "0123456789ABCDEF";
    int8_t i;
    if (!output) return;
    for (i = 7; i >= 0; --i)
    {
        output[i] = hex[value & 0x0Fu];
        value >>= 4;
    }
    output[8] = '\0';
}

static int base64_value(char ch)
{
    if (ch >= 'A' && ch <= 'Z') return ch - 'A';
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 26;
    if (ch >= '0' && ch <= '9') return ch - '0' + 52;
    if (ch == '+') return 62;
    if (ch == '/') return 63;
    return -1;
}

int OledCanvas_DecodeBase64(const char *input, size_t input_len,
                            uint8_t *output, size_t output_capacity,
                            size_t *output_len)
{
    size_t in_pos;
    size_t out_pos = 0u;
    if (output_len) *output_len = 0u;
    if (!input || !output || !output_len || input_len == 0u ||
        (input_len % 4u) != 0u)
        return 0;

    for (in_pos = 0u; in_pos < input_len; in_pos += 4u)
    {
        int a = base64_value(input[in_pos]);
        int b = base64_value(input[in_pos + 1u]);
        int c = input[in_pos + 2u] == '=' ? -2 : base64_value(input[in_pos + 2u]);
        int d = input[in_pos + 3u] == '=' ? -2 : base64_value(input[in_pos + 3u]);
        int final_group = (in_pos + 4u == input_len);
        size_t produced;

        if (a < 0 || b < 0 || c == -1 || d == -1) return 0;
        if (!final_group && (c == -2 || d == -2)) return 0;
        if (c == -2 && d != -2) return 0;
        produced = c == -2 ? 1u : (d == -2 ? 2u : 3u);
        if (out_pos + produced > output_capacity) return 0;

        output[out_pos++] = (uint8_t)((a << 2) | (b >> 4));
        if (produced > 1u)
            output[out_pos++] = (uint8_t)(((b & 0x0F) << 4) | (c >> 2));
        if (produced > 2u)
            output[out_pos++] = (uint8_t)(((c & 0x03) << 6) | d);

        if (c == -2 && (b & 0x0F) != 0) return 0;
        if (d == -2 && c >= 0 && (c & 0x03) != 0) return 0;
    }
    *output_len = out_pos;
    return 1;
}

size_t OledCanvas_BuildBeginPayload(const OledCanvasAssembly *assembly,
                                    uint8_t *output, size_t output_capacity)
{
    if (!assembly || !output || output_capacity < 12u ||
        assembly->state != OLED_CANVAS_ASSEMBLY_READY)
        return 0u;
    output[0] = OLED_CANVAS_OP_BEGIN;
    write_u16_le(assembly->transfer_id, &output[1]);
    output[3] = OLED_CANVAS_WIDTH;
    output[4] = OLED_CANVAS_HEIGHT;
    output[5] = OLED_CANVAS_FORMAT_PAGE_LSB;
    write_u16_le(OLED_CANVAS_BYTES, &output[6]);
    write_u32_le(assembly->expected_crc32, &output[8]);
    return 12u;
}

size_t OledCanvas_BuildChunkPayload(const OledCanvasAssembly *assembly,
                                    uint16_t offset,
                                    uint8_t *output, size_t output_capacity)
{
    size_t remaining;
    size_t data_len;
    if (!assembly || !output || assembly->state != OLED_CANVAS_ASSEMBLY_READY ||
        offset >= OLED_CANVAS_BYTES)
        return 0u;
    remaining = OLED_CANVAS_BYTES - offset;
    data_len = remaining < OLED_CANVAS_F4_CHUNK_BYTES
             ? remaining : OLED_CANVAS_F4_CHUNK_BYTES;
    if (output_capacity < 6u + data_len) return 0u;
    output[0] = OLED_CANVAS_OP_CHUNK;
    write_u16_le(assembly->transfer_id, &output[1]);
    write_u16_le(offset, &output[3]);
    output[5] = (uint8_t)data_len;
    memcpy(&output[6], &assembly->bytes[offset], data_len);
    return 6u + data_len;
}

size_t OledCanvas_BuildControlPayload(uint8_t operation, uint16_t transfer_id,
                                      uint8_t *output, size_t output_capacity)
{
    if (!output || output_capacity < 3u || transfer_id == 0u ||
        (operation != OLED_CANVAS_OP_COMMIT && operation != OLED_CANVAS_OP_CANCEL))
        return 0u;
    output[0] = operation;
    write_u16_le(transfer_id, &output[1]);
    return 3u;
}

int OledCanvas_ParseF4Response(const uint8_t *payload, size_t payload_len,
                               OledCanvasF4Response *response)
{
    if (!payload || !response || payload_len != OLED_CANVAS_F4_RESPONSE_BYTES)
        return 0;
    response->operation = payload[0];
    response->status = payload[1];
    response->transfer_id = read_u16_le(&payload[2]);
    response->next_offset = read_u16_le(&payload[4]);
    response->computed_crc32 = read_u32_le(&payload[6]);
    return response->operation >= OLED_CANVAS_OP_BEGIN &&
           response->operation <= OLED_CANVAS_OP_CANCEL &&
           response->status <= OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI;
}

int OledCanvas_F4ResponseMatchesExpected(const uint8_t *payload,
                                         size_t payload_len,
                                         uint8_t expected_operation,
                                         uint16_t expected_transfer_id,
                                         uint16_t expected_next_offset)
{
    OledCanvasF4Response response;
    if (!OledCanvas_ParseF4Response(payload, payload_len, &response)) return 0;
    if (response.operation != expected_operation ||
        response.transfer_id != expected_transfer_id) return 0;

    /* A non-OK response belongs to the current operation even when F4 reports
     * the offset at which it stopped.  Successful CHUNK responses, however,
     * need next_offset in the correlation key: every fragment uses command
     * 0x6D and the same transfer id, so this is what keeps a delayed duplicate
     * from completing the following UART transaction. */
    return response.status != OLED_CANVAS_STATUS_OK ||
           response.next_offset == expected_next_offset;
}
