#include <stdint.h>
#include <string.h>
#include <stdio.h>
#define PROGMEM
#define PSTR(x) (x)
#define pgm_read_byte(p) (*(const uint8_t *)(p))
#define pgm_read_word(p) (*(const uint16_t *)(p))
#define memcpy_P memcpy
#define strcpy_P strcpy
#define strncpy_P strncpy
#define strlen_P strlen
#define snprintf_P snprintf
#define strcmp_P strcmp

#define strncmp_P strncmp
