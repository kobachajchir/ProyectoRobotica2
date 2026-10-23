/*
 * osk.c
 *
 * On-Screen Keyboard — pure logic + layout + rendering.
 *
 * Created on: Aug 02, 2026
 */

#include "osk.h"
#include <string.h>
#include <stdlib.h>

/* ================================================================== */
/*  SPECIAL KEY CODES  (chars that are NOT printable ASCII)            */
/* ================================================================== */

#define KEY_SHIFT     '\x01'
#define KEY_BACKSPACE '\x02'
#define KEY_SYMBOLS   '\x03'
#define KEY_OK        '\x04'
#define KEY_SPACE     '\x05'
#define KEY_ABC       '\x06'
#define KEY_PLUSMINUS '\x07'
#define KEY_NONE      '\x00'

/* ================================================================== */
/*  LAYOUT GEOMETRY — all values in pixels for a 128 × 64 display     */
/* ================================================================== */

/* ---- Text bar ---- */
#define TEXT_BAR_X        1u
#define TEXT_BAR_Y        0u
#define TEXT_BAR_W      126u
#define TEXT_BAR_H       12u
#define TEXT_INNER_X      3u
#define TEXT_INNER_Y      2u

/* ---- Alpha keyboard ---- */
#define ALPHA_KEY_ROWS    4u
#define ALPHA_ROW_START_Y 13u
#define ALPHA_KEY_H       12u
#define ALPHA_KEY_W_STD   12u   /* standard single-char key width     */
#define ALPHA_KEY_GAP      1u

/* Row column counts for alpha layouts */
#define ALPHA_ROW0_COLS  10u  /* q w e r t y u i o p                */
#define ALPHA_ROW1_COLS   9u  /* a s d f g h j k l                  */
#define ALPHA_ROW2_COLS   9u  /* ⇧ z x c v b n m ⌫                 */
#define ALPHA_ROW3_COLS   3u  /* !@#$%  espacio  OK                 */

/* ---- Symbol keyboard  ---- */
#define SYM_KEY_ROWS      5u
#define SYM_ROW_START_Y  13u
#define SYM_KEY_H        10u
#define SYM_KEY_W_STD    12u

#define SYM_ROW0_COLS    10u  /* 1 2 3 4 5 6 7 8 9 0               */
#define SYM_ROW1_COLS    10u  /* ! @ # $ % ^ & * ? ~               */
#define SYM_ROW2_COLS    10u  /* - _ . , : ; " ' / \               */
#define SYM_ROW3_COLS    11u  /* ( ) [ ] { } < > + = ⌫             */
#define SYM_ROW4_COLS     3u  /* ABC  espacio  OK                   */

/* ---- Numeric keyboard ---- */
#define NUM_KEY_ROWS      3u
#define NUM_ROW_START_Y  14u
#define NUM_KEY_H        15u
#define NUM_KEY_W_STD    24u

#define NUM_ROW0_COLS     5u  /* 1 2 3 4 5 */
#define NUM_ROW1_COLS     5u  /* 6 7 8 9 0 */
#define NUM_ROW2_COLS     4u  /* .  ±  ⌫  OK  (or without ±)       */

/* ================================================================== */
/*  LAYOUT TABLES                                                     */
/* ================================================================== */

/* Each row is an array of key codes. Special keys use KEY_* defines. */

static const char kLowerRow0[] = "qwertyuiop";
static const char kLowerRow1[] = "asdfghjkl";
static const char kLowerRow2[] = { KEY_SHIFT, 'z','x','c','v','b','n','m', KEY_BACKSPACE, '\0' };

static const char kUpperRow0[] = "QWERTYUIOP";
static const char kUpperRow1[] = "ASDFGHJKL";
static const char kUpperRow2[] = { KEY_SHIFT, 'Z','X','C','V','B','N','M', KEY_BACKSPACE, '\0' };

static const char kSymRow0[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0' };
static const char kSymRow1[] = { '-', '/', ':', ';', '(', ')', '$', '&', '@', '"', '\0' };
static const char kSymRow2[] = { '.', ',', '?', '!', '\'', '\0' };
static const char kSymRow3[] = { '[', ']', '{', '}', '#', '%', '^', '*', '+', '=', '\0' };

static const char kNumRow0[] = "12345";
static const char kNumRow1[] = "67890";
/* Row 2 is built dynamically depending on allowNegative. */

/* ================================================================== */
/*  ROW DESCRIPTOR                                                    */
/* ================================================================== */

typedef struct {
    const char *keys;      /**< Array of key codes for this row.          */
    uint8_t     cols;      /**< Number of keys in this row.               */
    uint8_t     y;         /**< Top Y coordinate of the row (pixels).     */
    uint8_t     h;         /**< Row height (pixels).                      */
} RowDesc;

/* ------------------------------------------------------------------ */
/*  Helpers to build row descriptors for the current mode              */
/* ------------------------------------------------------------------ */

static void osk_BuildAlphaRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *row2Buf, char *row3Buf);
static void osk_BuildSymbolRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *row0Buf, char *row4Buf);
static void osk_BuildNumericRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *numRow2Buf);

/**
 * Fill rows[] for the current state.  Returns row count via *numRows.
 */
static void osk_GetRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *buf1, char *buf2)
{
    if (st->cfg.type == OSK_TYPE_NUMERIC) {
        osk_BuildNumericRows(st, rows, numRows, buf1);
    } else {
        switch (st->mode) {
            case OSK_MODE_UPPER:
            case OSK_MODE_LOWER:
                osk_BuildAlphaRows(st, rows, numRows, buf1, buf2);
                break;
            case OSK_MODE_SYMBOL:
                osk_BuildSymbolRows(st, rows, numRows, buf1, buf2);
                break;
        }
    }
}

static void osk_BuildAlphaRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *row2Buf, char *row3Buf)
{
    const bool upper = (st->mode == OSK_MODE_UPPER);
    rows[0].keys = upper ? kUpperRow0 : kLowerRow0;
    rows[0].cols = ALPHA_ROW0_COLS;
    rows[0].y    = ALPHA_ROW_START_Y;
    rows[0].h    = ALPHA_KEY_H;

    rows[1].keys = upper ? kUpperRow1 : kLowerRow1;
    rows[1].cols = ALPHA_ROW1_COLS;
    rows[1].y    = ALPHA_ROW_START_Y + ALPHA_KEY_H + 1u;
    rows[1].h    = ALPHA_KEY_H;

    /* Build row 2 dynamically based on allowUppercase */
    uint8_t c2 = 0;
    if (st->cfg.alpha.allowUppercase) {
        row2Buf[c2++] = KEY_SHIFT;
    }
    const char *srcRow2 = upper ? &kUpperRow2[1] : &kLowerRow2[1];
    while (*srcRow2 != '\0') {
        row2Buf[c2++] = *srcRow2++;
    }
    row2Buf[c2] = '\0';
    rows[2].keys = row2Buf;
    rows[2].cols = c2;
    rows[2].y    = ALPHA_ROW_START_Y + 2u * (ALPHA_KEY_H + 1u);
    rows[2].h    = ALPHA_KEY_H;

    /* Build row 3 dynamically based on allowSymbols / allowNumbers */
    uint8_t c3 = 0;
    if (st->cfg.alpha.allowSymbols || st->cfg.alpha.allowNumbers) {
        row3Buf[c3++] = KEY_SYMBOLS;
    }
    row3Buf[c3++] = KEY_SPACE;
    row3Buf[c3++] = KEY_OK;
    row3Buf[c3]   = '\0';
    rows[3].keys = row3Buf;
    rows[3].cols = c3;
    rows[3].y    = ALPHA_ROW_START_Y + 3u * (ALPHA_KEY_H + 1u);
    rows[3].h    = ALPHA_KEY_H;

    *numRows = ALPHA_KEY_ROWS;
}

static void osk_BuildSymbolRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *row0Buf, char *row4Buf)
{
    uint8_t r = 0;
    
    if (st->cfg.alpha.allowNumbers) {
        /* Usually kSymRow0 is numbers */
        rows[r].keys = kSymRow0;
        rows[r].cols = SYM_ROW0_COLS;
        rows[r].y    = SYM_ROW_START_Y + r * SYM_KEY_H;
        rows[r].h    = SYM_KEY_H;
        r++;
    }

    if (st->cfg.alpha.allowSymbols) {
        rows[r].keys = kSymRow1;
        rows[r].cols = SYM_ROW1_COLS;
        rows[r].y    = SYM_ROW_START_Y + r * SYM_KEY_H;
        rows[r].h    = SYM_KEY_H;
        r++;
        
        rows[r].keys = kSymRow2;
        rows[r].cols = SYM_ROW2_COLS;
        rows[r].y    = SYM_ROW_START_Y + r * SYM_KEY_H;
        rows[r].h    = SYM_KEY_H;
        r++;
        
        rows[r].keys = kSymRow3;
        rows[r].cols = SYM_ROW3_COLS;
        rows[r].y    = SYM_ROW_START_Y + r * SYM_KEY_H;
        rows[r].h    = SYM_KEY_H;
        r++;
    }

    /* Bottom row */
    uint8_t c4 = 0;
    if (st->cfg.alpha.allowLowercase || st->cfg.alpha.allowUppercase) {
        row4Buf[c4++] = KEY_ABC;
    }
    row4Buf[c4++] = KEY_SPACE;
    row4Buf[c4++] = KEY_OK;
    row4Buf[c4]   = '\0';

    rows[r].keys = row4Buf;
    rows[r].cols = c4;
    rows[r].y    = SYM_ROW_START_Y + r * SYM_KEY_H;
    rows[r].h    = SYM_KEY_H + 1u;
    r++;

    *numRows = r;
}

static void osk_BuildNumericRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *numRow2Buf)
{
    rows[0].keys = kNumRow0;
    rows[0].cols = NUM_ROW0_COLS;
    rows[0].y    = NUM_ROW_START_Y;
    rows[0].h    = NUM_KEY_H;

    rows[1].keys = kNumRow1;
    rows[1].cols = NUM_ROW1_COLS;
    rows[1].y    = NUM_ROW_START_Y + NUM_KEY_H + 1u;
    rows[1].h    = NUM_KEY_H;

    /* Build row2 dynamically: .  [±]  ⌫  OK */
    uint8_t idx = 0;
#if OSK_SUPPORT_FLOAT
    if (st->cfg.numeric.mode == OSK_NUMERIC_MODE_FLOAT && st->cfg.numeric.maxDecimalDigits > 0) {
        numRow2Buf[idx++] = '.';
    }
#endif
    if (st->cfg.numeric.allowNegative) {
        numRow2Buf[idx++] = KEY_PLUSMINUS;
    }
    numRow2Buf[idx++] = KEY_BACKSPACE;
    numRow2Buf[idx++] = KEY_OK;
    numRow2Buf[idx]   = '\0';

    rows[2].keys = numRow2Buf;
    rows[2].cols = idx;
    rows[2].y    = NUM_ROW_START_Y + 2u * (NUM_KEY_H + 1u);
    rows[2].h    = NUM_KEY_H;

    *numRows = NUM_KEY_ROWS;
}

/* ================================================================== */
/*  GEOMETRY HELPERS — compute key position & size                     */
/* ================================================================== */

/**
 * @brief Compute the X position and width for a key at (row, col).
 */
static void osk_GetKeyRect(const OSK_State *st,
                           const RowDesc *rows, uint8_t numRows,
                           uint8_t row, uint8_t col,
                           int16_t *outX, int16_t *outY,
                           int16_t *outW, int16_t *outH)
{
    if (row >= numRows) { *outX = 0; *outY = 0; *outW = 0; *outH = 0; return; }

    const RowDesc *rd = &rows[row];
    *outY = (int16_t)rd->y;
    *outH = (int16_t)rd->h;

    if (st->cfg.type == OSK_TYPE_NUMERIC) {
        /* Numeric: last row has variable-width keys */
        if (row == 2u) {
            uint8_t totalKeys = rd->cols;
            /* Distribute 128px among totalKeys keys with 1px gaps */
            uint8_t gapTotal = (totalKeys > 1u) ? (totalKeys - 1u) : 0u;
            uint8_t usable   = (uint8_t)(OSK_SCREEN_W - gapTotal);
            uint8_t baseW    = usable / totalKeys;
            uint8_t extra    = usable % totalKeys;

            int16_t x = 0;
            for (uint8_t c = 0; c < col; c++) {
                x += (int16_t)baseW + ((c < extra) ? 1 : 0) + 1;
            }
            *outX = x;
            *outW = (int16_t)baseW + ((col < extra) ? 1 : 0);
        } else {
            /* Rows 0-1: 5 equal-width keys */
            uint8_t kw = (OSK_SCREEN_W - 4u) / 5u;  /* 4 gaps × 1px */
            *outX = (int16_t)(col * (kw + 1u));
            *outW = (int16_t)kw;
        }
    } else {
        /* Alpha / Symbol layouts */
        const char *keys = rd->keys;
        (void)keys;
        uint8_t nk = rd->cols;

        /* Bottom row (last row) has wide keys */
        bool isBottomRow = (st->mode == OSK_MODE_SYMBOL)
                           ? (row == (SYM_KEY_ROWS - 1u))
                           : (row == (ALPHA_KEY_ROWS - 1u));

        if (isBottomRow) {
            /* 3 keys: [Symbols/ABC]  [espacio]  [OK] */
            /* Widths: ~30px, ~68px, ~28px  (= 126px + 2 gaps) */
            static const uint8_t bottomW[3] = { 30, 68, 28 };
            int16_t x = 0;
            for (uint8_t c = 0; c < col; c++) {
                x += (int16_t)bottomW[c] + 1;
            }
            *outX = x;
            *outW = (int16_t)bottomW[col < 3u ? col : 2u];
        } else {
            /* Standard rows: equal-width keys */
            uint8_t gapTotal = (nk > 1u) ? (nk - 1u) : 0u;
            uint8_t usable   = (uint8_t)(OSK_SCREEN_W - gapTotal);
            uint8_t baseW    = usable / nk;
            uint8_t extra    = usable % nk;

            int16_t x = 0;
            for (uint8_t c = 0; c < col; c++) {
                x += (int16_t)baseW + ((c < extra) ? 1 : 0) + 1;
            }
            *outX = x;
            *outW = (int16_t)baseW + ((col < extra) ? 1 : 0);
        }
    }
}

/* ================================================================== */
/*  KEY LABEL RENDERING                                               */
/* ================================================================== */

/**
 * @brief Write the label for a key at a given position, centred.
 */
static void osk_DrawKeyLabel(const OSK_State *st,
                             const OSK_DrawHandle *draw,
                             char key, int16_t kx, int16_t ky,
                             int16_t kw, int16_t kh,
                             bool isShiftActive)
{
    /* Character width/height for Font_7x10 */
    const uint8_t chW = 7u;
    const uint8_t chH = 10u;

    switch (key) {
        case KEY_SHIFT: {
            /* Draw a simple up-arrow glyph using lines */
            /* Arrow centred in the key box */
            int16_t cx = kx + kw / 2;
            int16_t cy = ky + kh / 2;
            /* Vertical stem */
            draw->drawVLine(cx, cy - 3, 7);
            /* Arrow head: two short diagonal lines approximated */
            draw->drawHLine(cx - 3, cy - 3, 7);
            draw->drawHLine(cx - 2, cy - 4, 5);
            draw->drawHLine(cx - 1, cy - 5, 3);
            (void)isShiftActive;
            break;
        }
        case KEY_BACKSPACE: {
            /* Draw a simple "x" with left arrow using chars */
            uint8_t lx = (uint8_t)(kx + (kw - chW) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeChar('<');
            break;
        }
        case KEY_SYMBOLS: {
            /* If no symbols allowed but numbers are, label is "123" */
            const char *lbl = (st->cfg.alpha.allowSymbols) ? "!@#" : "123";
            uint8_t tw = (uint8_t)(3u * chW);
            uint8_t lx = (uint8_t)(kx + (kw - tw) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeString((char *)lbl);
            break;
        }
        case KEY_ABC: {
            char lbl[] = "ABC";
            uint8_t tw = (uint8_t)(3u * chW);
            uint8_t lx = (uint8_t)(kx + (kw - tw) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeString(lbl);
            break;
        }
        case KEY_SPACE: {
            char lbl[] = "espacio";
            uint8_t tw = (uint8_t)(7u * chW);
            uint8_t lx = (uint8_t)(kx + (kw - tw) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeString(lbl);
            break;
        }
        case KEY_OK: {
            char lbl[] = "OK";
            uint8_t tw = (uint8_t)(2u * chW);
            uint8_t lx = (uint8_t)(kx + (kw - tw) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeString(lbl);
            break;
        }
        case KEY_PLUSMINUS: {
            char lbl[] = "+/-";
            uint8_t tw = (uint8_t)(3u * chW);
            uint8_t lx = (uint8_t)(kx + (kw - tw) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeString(lbl);
            break;
        }
        case KEY_NONE:
            break;
        default: {
            /* Printable character — centred */
            uint8_t lx = (uint8_t)(kx + (kw - chW) / 2);
            uint8_t ly = (uint8_t)(ky + (kh - chH) / 2);
            draw->setCursor(lx, ly);
            draw->writeChar(key);
            break;
        }
    }
}

/* ================================================================== */
/*  SINGLE KEY DRAWING                                                */
/* ================================================================== */

/**
 * @brief Draw a single key cell (border + label), highlighted or normal.
 */
static void osk_DrawKey(const OSK_State *st,
                        const RowDesc *rows, uint8_t numRows,
                        uint8_t row, uint8_t col,
                        bool highlighted)
{
    if (row >= numRows) return;
    const RowDesc *rd = &rows[row];
    if (col >= rd->cols) return;

    int16_t kx, ky, kw, kh;
    osk_GetKeyRect(st, rows, numRows, row, col, &kx, &ky, &kw, &kh);

    const OSK_DrawHandle *d = &st->cfg.draw;
    char key = rd->keys[col];

    if (highlighted) {
        /* Fill white, then draw label in black */
        d->setColor(OSK_COLOR_WHITE);
        d->fillRect(kx, ky, kw, kh);
        d->setColor(OSK_COLOR_BLACK);
        osk_DrawKeyLabel(st, d, key, kx, ky, kw, kh,
                         st->mode == OSK_MODE_UPPER);
        /* Restore color to white for subsequent drawing */
        d->setColor(OSK_COLOR_WHITE);
    } else {
        /* Clear interior (black), draw border (white) */
        d->setColor(OSK_COLOR_BLACK);
        d->fillRect(kx, ky, kw, kh);
        d->setColor(OSK_COLOR_WHITE);
        d->drawRect(kx, ky, kw, kh);
        osk_DrawKeyLabel(st, d, key, kx, ky, kw, kh,
                         st->mode == OSK_MODE_UPPER);
    }
}

/* ================================================================== */
/*  TEXT BAR DRAWING                                                  */
/* ================================================================== */

static void osk_DrawTextBar(const OSK_State *st)
{
    const OSK_DrawHandle *d = &st->cfg.draw;

    /* Clear text bar area */
    d->setColor(OSK_COLOR_BLACK);
    d->fillRect(TEXT_BAR_X, TEXT_BAR_Y, TEXT_BAR_W, TEXT_BAR_H);

    /* Draw border */
    d->setColor(OSK_COLOR_WHITE);
    d->drawRect(TEXT_BAR_X, TEXT_BAR_Y, TEXT_BAR_W, TEXT_BAR_H);

    /* Draw text content */
    d->setCursor(TEXT_INNER_X, TEXT_INNER_Y);

    /* For numeric keyboard, show the value with decimal point context */
    char displayBuf[OSK_MAX_TEXT_LEN + 4];
    if (st->cfg.type == OSK_TYPE_NUMERIC) {
        /* Show with a prefix if negative */
        if (st->isNegative && st->textLen > 0) {
            displayBuf[0] = '-';
            memcpy(&displayBuf[1], st->textBuf, st->textLen + 1u);
        } else {
            memcpy(displayBuf, st->textBuf, st->textLen + 1u);
        }
    } else {
        memcpy(displayBuf, st->textBuf, st->textLen + 1u);
    }
    d->writeString(displayBuf);

    /* Draw cursor bar '|' */
    d->writeChar('|');
}

/* ================================================================== */
/*  FULL SCREEN DRAWING                                               */
/* ================================================================== */

static void osk_DrawAll(OSK_State *st)
{
    const OSK_DrawHandle *d = &st->cfg.draw;
    RowDesc rows[OSK_MAX_ROWS];
    uint8_t numRows = 0;
    char b1[16], b2[16];
    osk_GetRows(st, rows, &numRows, b1, b2);

    /* Clear entire screen */
    d->clearScreen();

    /* Draw text bar */
    osk_DrawTextBar(st);

    /* Draw all keys */
    for (uint8_t r = 0; r < numRows; r++) {
        for (uint8_t c = 0; c < rows[r].cols; c++) {
            bool sel = (r == st->curRow && c == st->curCol);
            osk_DrawKey(st, rows, numRows, r, c, sel);
        }
    }

    /* Flush full screen */
    d->updateScreen();
}

/* ================================================================== */
/*  PARTIAL UPDATES                                                   */
/* ================================================================== */

static void osk_UpdateTextOnly(OSK_State *st)
{
    const OSK_DrawHandle *d = &st->cfg.draw;

    osk_DrawTextBar(st);

    /* Text bar occupies pages 0 and 1 (Y 0..12 → page 0 = Y0..7, page 1 = Y8..15) */
    d->requestPageUpdate(0);
    d->requestPageUpdate(1);
}

static void osk_UpdateKeyHighlight(OSK_State *st)
{
    const OSK_DrawHandle *d = &st->cfg.draw;
    RowDesc rows[OSK_MAX_ROWS];
    uint8_t numRows = 0;
    char b1[16], b2[16];
    osk_GetRows(st, rows, &numRows, b1, b2);

    /* Un-highlight previous key */
    if (st->prevRow < numRows && st->prevCol < rows[st->prevRow].cols) {
        osk_DrawKey(st, rows, numRows, st->prevRow, st->prevCol, false);

        /* Request page updates for the previous key region */
        int16_t kx, ky, kw, kh;
        osk_GetKeyRect(st, rows, numRows, st->prevRow, st->prevCol, &kx, &ky, &kw, &kh);
        uint8_t pageStart = (uint8_t)(ky / 8u);
        uint8_t pageEnd   = (uint8_t)((ky + kh - 1) / 8u);
        for (uint8_t p = pageStart; p <= pageEnd && p < 8u; p++) {
            d->requestPageUpdate(p);
        }
    }

    /* Highlight current key */
    if (st->curRow < numRows && st->curCol < rows[st->curRow].cols) {
        osk_DrawKey(st, rows, numRows, st->curRow, st->curCol, true);

        int16_t kx, ky, kw, kh;
        osk_GetKeyRect(st, rows, numRows, st->curRow, st->curCol, &kx, &ky, &kw, &kh);
        uint8_t pageStart = (uint8_t)(ky / 8u);
        uint8_t pageEnd   = (uint8_t)((ky + kh - 1) / 8u);
        for (uint8_t p = pageStart; p <= pageEnd && p < 8u; p++) {
            d->requestPageUpdate(p);
        }
    }
}

/* ================================================================== */
/*  CURSOR MOVEMENT — serpentine traversal                            */
/* ================================================================== */

/**
 * @brief Total number of keys across all rows for serpentine navigation.
 */
static uint16_t osk_TotalKeys(const RowDesc *rows, uint8_t numRows)
{
    uint16_t total = 0;
    for (uint8_t r = 0; r < numRows; r++) {
        total += rows[r].cols;
    }
    return total;
}

/**
 * @brief Convert (row, col) to a flat index for serpentine ordering.
 */
static uint16_t osk_ToFlat(const RowDesc *rows, uint8_t numRows,
                           uint8_t row, uint8_t col)
{
    uint16_t idx = 0;
    for (uint8_t r = 0; r < row && r < numRows; r++) {
        idx += rows[r].cols;
    }
    idx += col;
    return idx;
}

/**
 * @brief Convert flat index back to (row, col).
 */
static void osk_FromFlat(const RowDesc *rows, uint8_t numRows,
                         uint16_t flat, uint8_t *outRow, uint8_t *outCol)
{
    for (uint8_t r = 0; r < numRows; r++) {
        if (flat < rows[r].cols) {
            *outRow = r;
            *outCol = (uint8_t)flat;
            return;
        }
        flat -= rows[r].cols;
    }
    /* Fallback to last key */
    *outRow = numRows - 1u;
    *outCol = rows[numRows - 1u].cols - 1u;
}

static void osk_MoveCursor(OSK_State *st, int8_t direction)
{
    RowDesc rows[OSK_MAX_ROWS];
    uint8_t numRows = 0;
    char b1[16], b2[16];
    osk_GetRows(st, rows, &numRows, b1, b2);

    uint16_t total = osk_TotalKeys(rows, numRows);
    if (total == 0) return;

    uint16_t flat = osk_ToFlat(rows, numRows, st->curRow, st->curCol);

    st->prevRow = st->curRow;
    st->prevCol = st->curCol;

    if (direction > 0) {
        flat = (flat + 1u) % total;
    } else {
        flat = (flat == 0) ? (total - 1u) : (flat - 1u);
    }

    osk_FromFlat(rows, numRows, flat, &st->curRow, &st->curCol);
    st->needsKeyHighlight = true;
}

/* ================================================================== */
/*  TEXT BUFFER MANIPULATION                                          */
/* ================================================================== */

static void osk_AppendChar(OSK_State *st, char ch)
{
    uint8_t maxLen = st->cfg.maxLen;
    if (maxLen > OSK_MAX_TEXT_LEN) maxLen = OSK_MAX_TEXT_LEN;
    if (st->textLen >= maxLen) return;
    if (st->cfg.type == OSK_TYPE_ALPHA &&
        st->cfg.alpha.allowedChars != NULL &&
        strchr(st->cfg.alpha.allowedChars, ch) == NULL) return;

    st->textBuf[st->textLen++] = ch;
    st->textBuf[st->textLen]   = '\0';
    st->needsTextRedraw = true;
}

static void osk_DeleteChar(OSK_State *st)
{
    if (st->textLen == 0) return;
    st->textLen--;
    st->textBuf[st->textLen] = '\0';

    /* If we deleted the decimal point, clear the flag */
    if (st->cfg.type == OSK_TYPE_NUMERIC) {
        bool hasDot = false;
        for (uint8_t i = 0; i < st->textLen; i++) {
            if (st->textBuf[i] == '.') { hasDot = true; break; }
        }
        st->hasDecimalPoint = hasDot;
    }
    st->needsTextRedraw = true;
}

/* ================================================================== */
/*  NUMERIC VALIDATION                                                */
/* ================================================================== */

/**
 * @brief Check if adding a digit is valid for numeric constraints.
 */
static bool osk_NumericCanAddDigit(const OSK_State *st)
{
    const OSK_NumericConfig *nc = &st->cfg.numeric;

    if (st->hasDecimalPoint) {
        /* Count digits after the dot */
        uint8_t afterDot = 0;
        bool pastDot = false;
        for (uint8_t i = 0; i < st->textLen; i++) {
            if (st->textBuf[i] == '.') { pastDot = true; continue; }
            if (pastDot) afterDot++;
        }
        return (afterDot < nc->maxDecimalDigits);
    } else {
        /* Count integer digits (everything in buffer except dot) */
        uint8_t intDigits = 0;
        for (uint8_t i = 0; i < st->textLen; i++) {
            if (st->textBuf[i] != '.') intDigits++;
        }
        return (intDigits < nc->maxIntegerDigits);
    }
}

static bool osk_NumericCanAddDot(const OSK_State *st)
{
    if (st->hasDecimalPoint) return false;
    if (st->cfg.numeric.maxDecimalDigits == 0) return false;
    return true;
}

/**
 * @brief Validate the current numeric buffer against range constraints.
 * @return true if value is within [minValue, maxValue].
 */
static bool osk_NumericValidateRange(const OSK_State *st)
{
    if (st->cfg.type != OSK_TYPE_NUMERIC) return true;

#if OSK_SUPPORT_FLOAT
    if (st->cfg.numeric.mode == OSK_NUMERIC_MODE_FLOAT) {
        float val = OSK_GetNumericValueFloat(st);
        return (val >= st->cfg.numeric.minFloatValue && val <= st->cfg.numeric.maxFloatValue);
    }
#endif

    int32_t val = OSK_GetNumericValueInt(st);
    return (val >= st->cfg.numeric.minIntValue && val <= st->cfg.numeric.maxIntValue);
}

/* ================================================================== */
/*  KEY ACTION                                                        */
/* ================================================================== */

static void osk_HandleKeyPress(OSK_State *st)
{
    RowDesc rows[OSK_MAX_ROWS];
    uint8_t numRows = 0;
    char b1[16], b2[16];
    osk_GetRows(st, rows, &numRows, b1, b2);

    if (st->curRow >= numRows) return;
    if (st->curCol >= rows[st->curRow].cols) return;

    char key = rows[st->curRow].keys[st->curCol];

    switch (key) {
        case KEY_SHIFT:
            st->mode = (st->mode == OSK_MODE_UPPER) ? OSK_MODE_LOWER : OSK_MODE_UPPER;
            /* Try to preserve cursor position: clamp col to new row width */
            {
                RowDesc newRows[OSK_MAX_ROWS];
                uint8_t newNumRows = 0;
                char b1[16], b2[16];
                osk_GetRows(st, newRows, &newNumRows, b1, b2);
                if (st->curRow < newNumRows && st->curCol >= newRows[st->curRow].cols) {
                    st->curCol = newRows[st->curRow].cols - 1u;
                }
            }
            st->needsFullRedraw = true;
            break;

        case KEY_SYMBOLS:
            st->mode = OSK_MODE_SYMBOL;
            st->curRow = 0;
            st->curCol = 0;
            st->needsFullRedraw = true;
            break;

        case KEY_ABC:
            st->mode = OSK_MODE_LOWER;
            st->curRow = 0;
            st->curCol = 0;
            st->needsFullRedraw = true;
            break;

        case KEY_BACKSPACE:
            osk_DeleteChar(st);
            break;

        case KEY_SPACE:
            osk_AppendChar(st, ' ');
            break;

        case KEY_OK:
            if (st->cfg.type == OSK_TYPE_NUMERIC) {
                if (!osk_NumericValidateRange(st)) {
                    st->validationError = true;
                    st->needsTextRedraw = true;
                    return;
                }
            }
            st->active = false;
            if (st->cfg.onComplete) {
                /* The negative sign must be applied in the buffer if it was selected. */
                if (st->cfg.type == OSK_TYPE_NUMERIC && st->isNegative && st->textLen > 0) {
                    char tmpBuf[OSK_MAX_TEXT_LEN + 1];
                    memcpy(tmpBuf, st->textBuf, st->textLen + 1u);
                    
                    st->textBuf[0] = '-';
                    memcpy(&st->textBuf[1], tmpBuf, st->textLen + 1u);
                    st->textLen++;
                }
                
                st->cfg.onComplete(st, st->cfg.callbackCtx);
            }
            break;

        case KEY_PLUSMINUS:
            st->isNegative = !st->isNegative;
            st->needsTextRedraw = true;
            break;

        default:
            /* Printable character */
            if (st->cfg.type == OSK_TYPE_NUMERIC) {
                if (key == '.') {
                    if (osk_NumericCanAddDot(st)) {
                        st->hasDecimalPoint = true;
                        osk_AppendChar(st, '.');
                    }
                } else if (key >= '0' && key <= '9') {
                    if (osk_NumericCanAddDigit(st)) {
                        osk_AppendChar(st, key);
                    }
                }
            } else {
                osk_AppendChar(st, key);
            }
            st->validationError = false;
            break;
    }
}

/* ================================================================== */
/*  PUBLIC API                                                        */
/* ================================================================== */

void OSK_Open(OSK_State *state, const OSK_Config *cfg)
{
    memset(state, 0, sizeof(OSK_State));
    memcpy(&state->cfg, cfg, sizeof(OSK_Config));

    state->mode = OSK_MODE_LOWER;
    state->curRow = 0;
    state->curCol = 0;
    state->active = true;
    state->needsFullRedraw = true;

    /* Apply max length limit */
    if (state->cfg.maxLen == 0 || state->cfg.maxLen > OSK_MAX_TEXT_LEN) {
        state->cfg.maxLen = OSK_MAX_TEXT_LEN;
    }

    /* Copy initial text */
    if (cfg->type == OSK_TYPE_NUMERIC) {
        
        bool hasInit = false;
        
#if OSK_SUPPORT_FLOAT
        if (cfg->numeric.mode == OSK_NUMERIC_MODE_FLOAT) {
            float initVal = cfg->numeric.initialFloatValue;
            if (initVal != 0.0f || cfg->numeric.prefillZero) hasInit = true;
            state->isNegative = (initVal < 0.0f);
            if (state->isNegative) initVal = -initVal;

            if (hasInit && cfg->numeric.maxDecimalDigits > 0) {
                /* Build a formatted string manually to avoid snprintf/float */
                uint32_t intPart = (uint32_t)initVal;
                uint32_t mul = 1;
                for (uint8_t d = 0; d < cfg->numeric.maxDecimalDigits; d++) mul *= 10u;
                uint32_t fracPart = (uint32_t)((initVal - (float)intPart) * (float)mul + 0.5f);

                /* Integer part */
                char tmpBuf[16];
                uint8_t pos = 0;
                if (intPart == 0) {
                    tmpBuf[pos++] = '0';
                } else {
                    char digits[12];
                    uint8_t nd = 0;
                    uint32_t v = intPart;
                    while (v > 0) { digits[nd++] = (char)('0' + (v % 10u)); v /= 10u; }
                    for (int8_t i = (int8_t)nd - 1; i >= 0; i--) tmpBuf[pos++] = digits[i];
                }
                tmpBuf[pos++] = '.';
                state->hasDecimalPoint = true;

                /* Fractional part with leading zeros */
                {
                    char fdigits[8];
                    uint8_t fd = 0;
                    uint32_t fv = fracPart;
                    for (uint8_t d = 0; d < cfg->numeric.maxDecimalDigits; d++) {
                        fdigits[fd++] = (char)('0' + (fv % 10u));
                        fv /= 10u;
                    }
                    for (int8_t i = (int8_t)fd - 1; i >= 0; i--) tmpBuf[pos++] = fdigits[i];
                }
                tmpBuf[pos] = '\0';

                /* Strip trailing zeros after decimal point */
                while (pos > 0 && tmpBuf[pos - 1] == '0') { pos--; tmpBuf[pos] = '\0'; }
                if (pos > 0 && tmpBuf[pos - 1] == '.') { pos--; tmpBuf[pos] = '\0'; state->hasDecimalPoint = false; }

                memcpy(state->textBuf, tmpBuf, pos + 1u);
                state->textLen = pos;
            } else if (hasInit) {
                /* Integer-only float mode */
                uint32_t v = (uint32_t)initVal;
                char digits[12];
                uint8_t nd = 0;
                if (v == 0) {
                    digits[nd++] = '0';
                } else {
                    while (v > 0) { digits[nd++] = (char)('0' + (v % 10u)); v /= 10u; }
                }
                for (uint8_t i = 0; i < nd; i++) {
                    state->textBuf[i] = digits[nd - 1u - i];
                }
                state->textBuf[nd] = '\0';
                state->textLen = nd;
            }
        }
#endif
        if (cfg->numeric.mode == OSK_NUMERIC_MODE_INT) {
            int32_t initVal = cfg->numeric.initialIntValue;
            if (initVal != 0 || cfg->numeric.prefillZero) hasInit = true;
            state->isNegative = (initVal < 0);
            if (state->isNegative) initVal = -initVal;

            if (hasInit) {
                uint32_t v = (uint32_t)initVal;
                char digits[12];
                uint8_t nd = 0;
                if (v == 0) {
                    digits[nd++] = '0';
                } else {
                    while (v > 0) { digits[nd++] = (char)('0' + (v % 10u)); v /= 10u; }
                }
                for (uint8_t i = 0; i < nd; i++) {
                    state->textBuf[i] = digits[nd - 1u - i];
                }
                state->textBuf[nd] = '\0';
                state->textLen = nd;
            }
        }

        /* Override maxLen for numeric based on digit constraints */
        uint8_t numMaxLen = cfg->numeric.maxIntegerDigits;
#if OSK_SUPPORT_FLOAT
        if (cfg->numeric.mode == OSK_NUMERIC_MODE_FLOAT && cfg->numeric.maxDecimalDigits > 0) {
            numMaxLen += 1u + cfg->numeric.maxDecimalDigits; /* +1 for '.' */
        }
#endif
        if (numMaxLen > OSK_MAX_TEXT_LEN) numMaxLen = OSK_MAX_TEXT_LEN;
        state->cfg.maxLen = numMaxLen;
    } else {
        /* Alpha: copy initial text if provided */
        if (cfg->alpha.initialTextMode == OSK_INITIAL_TEXT_PREFILL &&
            cfg->initialText != NULL) {
            uint8_t len = 0;
            while (cfg->initialText[len] != '\0' && len < state->cfg.maxLen) {
                state->textBuf[len] = cfg->initialText[len];
                len++;
            }
            state->textBuf[len] = '\0';
            state->textLen = len;
        }
    }
}

void OSK_Close(OSK_State *state)
{
    state->active = false;
}

void OSK_OnLeft(OSK_State *state)
{
    if (!state->active) return;
    osk_MoveCursor(state, -1);
}

void OSK_OnRight(OSK_State *state)
{
    if (!state->active) return;
    osk_MoveCursor(state, +1);
}

void OSK_OnSelect(OSK_State *state)
{
    if (!state->active) return;
    osk_HandleKeyPress(state);
}

void OSK_OnBack(OSK_State *state)
{
    if (!state->active) return;
    state->active = false;
    if (state->cfg.onCancel) {
        state->cfg.onCancel(state->cfg.callbackCtx);
    }
}

void OSK_Render(OSK_State *state)
{
    if (!state->active) return;

    if (state->needsFullRedraw) {
        osk_DrawAll(state);
        state->needsFullRedraw  = false;
        state->needsTextRedraw  = false;
        state->needsKeyHighlight = false;
        return;
    }

    if (state->needsTextRedraw) {
        osk_UpdateTextOnly(state);
        state->needsTextRedraw = false;
    }

    if (state->needsKeyHighlight) {
        osk_UpdateKeyHighlight(state);
        state->needsKeyHighlight = false;
    }
}

bool OSK_IsActive(const OSK_State *state)
{
    return state->active;
}

const char *OSK_GetText(const OSK_State *state)
{
    return state->textBuf;
}

int32_t OSK_GetNumericValueInt(const OSK_State *state)
{
    if (state->textLen == 0) return 0;
    
    int32_t result = 0;
    for (uint8_t i = 0; i < state->textLen; i++) {
        char ch = state->textBuf[i];
        if (ch == '-') continue;
        if (ch == '.') break; /* Ignore fractional part */
        if (ch >= '0' && ch <= '9') {
            result = result * 10 + (ch - '0');
        }
    }
    
    if (state->isNegative || (state->textLen > 0 && state->textBuf[0] == '-')) {
        result = -result;
    }
    
    return result;
}

#if OSK_SUPPORT_FLOAT
float OSK_GetNumericValueFloat(const OSK_State *state)
{
    if (state->textLen == 0) return 0.0f;

    /* Simple manual float parser to avoid pulling in strtof/atof */
    float result = 0.0f;
    float frac   = 0.0f;
    float div    = 1.0f;
    bool pastDot = false;

    for (uint8_t i = 0; i < state->textLen; i++) {
        char ch = state->textBuf[i];
        if (ch == '-') continue;
        if (ch == '.') {
            pastDot = true;
            continue;
        }
        if (ch >= '0' && ch <= '9') {
            uint8_t digit = (uint8_t)(ch - '0');
            if (pastDot) {
                div *= 10.0f;
                frac += (float)digit / div;
            } else {
                result = result * 10.0f + (float)digit;
            }
        }
    }

    result += frac;
    if (state->isNegative || (state->textLen > 0 && state->textBuf[0] == '-')) {
        result = -result;
    }
    return result;
}
#endif
