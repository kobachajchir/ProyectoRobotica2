#ifndef BINARY_ASSET_TYPES_H
#define BINARY_ASSET_TYPES_H
#include <stdint.h>
typedef struct
{
    uint8_t asset_id;
    uint8_t width;
    uint8_t height;
    uint16_t len;
    const uint8_t *data;
} BinaryAssetView_t;

#endif
