#pragma once
#include <stdint.h>

// Bit helpers
#define SET_FLAG(bm, idx)    do { (bm).byte = (uint8_t)((bm).byte |  (uint8_t)(1u << (idx))); } while(0)
#define CLEAR_FLAG(bm, idx)  do { (bm).byte = (uint8_t)((bm).byte & ~(uint8_t)(1u << (idx))); } while(0)
#define IS_FLAG_SET(bm, idx) (((bm).byte & (uint8_t)(1u << (idx))) != 0)

#define SET_NIBBLE(bm, nibble, value) \
  do { if ((nibble)==0) { (bm).byte = (uint8_t)(((bm).byte & 0xF0u) | ((uint8_t)(value) & 0x0Fu)); } \
       else             { (bm).byte = (uint8_t)(((bm).byte & 0x0Fu) | ((((uint8_t)(value) & 0x0Fu) << 4))); } } while(0)
#define GET_NIBBLE(bm, nibble) ((uint8_t)(((nibble)==0) ? ((bm).byte & 0x0Fu) : (((bm).byte >> 4) & 0x0Fu)))

typedef union {
  struct {
    uint8_t b0:1; uint8_t b1:1; uint8_t b2:1; uint8_t b3:1;
    uint8_t b4:1; uint8_t b5:1; uint8_t b6:1; uint8_t b7:1;
  } bits;
  struct { uint8_t low:4; uint8_t high:4; } nibbles;
  uint8_t byte;
} t_bitMap;
