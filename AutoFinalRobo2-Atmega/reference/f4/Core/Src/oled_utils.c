#include "screens_internal.h"
/*
 * oled_utils.c
 *
 *  Created on: Jun 16, 2025
 *      Author: kobac
 */

#include "fonts.h"
#include "oled_utils.h"
#include "oled_canvas.h"
#include "screen_lock.h"
#include "screenWrappers.h"
#include "automicro_boot_contract.h"
#include <math.h>
#include <stdio.h>
#include <string.h>









enum {
    OLED_SIMPLE_ICON_FLAG_NONE = 0u,
    OLED_SIMPLE_ICON_FLAG_MSB = 1u << 0,
    OLED_SIMPLE_ICON_FLAG_INVERSE = 1u << 1,
};



#define OLED_MPU_PANEL_Y          0u
#define OLED_MPU_PANEL_H          64u
#define OLED_MPU_PANEL_W          42u
#define OLED_MPU_PANEL_GAP        1u
#define OLED_MPU_PANEL0_X         0u
#define OLED_MPU_PANEL1_X         (OLED_MPU_PANEL0_X + OLED_MPU_PANEL_W + OLED_MPU_PANEL_GAP)
#define OLED_MPU_PANEL2_X         (OLED_MPU_PANEL1_X + OLED_MPU_PANEL_W + OLED_MPU_PANEL_GAP)
#define OLED_MPU_RAD_PER_DEG      0.0174532925f

static void OledUtils_RenderSimpleNotification(void);

static uint8_t Oled_TextPixelWidth(const char *text);
static uint8_t Oled_CenterTextX(const char *text, uint8_t areaX, uint8_t areaW);
static bool Oled_SsidIsUseful(const char *ssid);
static const char *Oled_GetDashboardStaSsid(void);
static void Oled_DrawCenteredLine(const char *text, uint8_t areaX, uint8_t areaW, uint8_t baselineY);
static void Oled_DrawTitleLine(const char *text, uint8_t areaX, uint8_t areaW, uint8_t baselineY);
static void OledUtils_DrawIRGroupFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *title);
static void OledUtils_DrawIRBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t value);
static bool OledUtils_AreRouteIrLightsOn(void);
static void Oled_FormatRouteValues(char *buf, size_t len, const char *labelA, uint16_t valueA, const char *labelB, uint16_t valueB);
static void OledUtils_DrawDashboardActionTimeoutBar(void);
static bool OLED_ReadXbmPixel(const uint8_t *bits, uint8_t w, uint8_t x, uint8_t y);
static void OLED_DrawRotatedBitmap(int cx, int cy, uint8_t w, uint8_t h, const uint8_t *bits, float angle_deg);
static void OledUtils_DrawRouteAlignmentVisual(const TCRT5000_RouteSample_t *route);

static void OledUtils_SetScreenCode(ScreenCode_t screen_code, ScreenReportSource_t source)
{
    MenuSys_SetCurrentScreenCode(&menuSystem, screen_code, source);
}

static void Oled_SetFont(const FontDef *font)
{
    oled_font = font;
}

static uint8_t Oled_FontHeight(void)
{
    return oled_font->FontHeight;
}

static uint8_t Oled_FontWidth(void)
{
    return oled_font->FontWidth;
}

static void Oled_DrawStr(const char *text)
{
    (void)0; // BREAKPOINT: impresión de texto en buffer
    ssd1306_SetColor(White);
    ssd1306_WriteString((char *)text, *oled_font);
}

static void Oled_DrawStrMax(const char *text, uint8_t maxChars)
{
    char buf[22];
    uint8_t i = 0u;

    if (!text) {
        text = "";
    }

    while (text[i] != '\0' && i < maxChars && i < (uint8_t)(sizeof(buf) - 1u)) {
        buf[i] = text[i];
        i++;
    }
    buf[i] = '\0';
    Oled_DrawStr(buf);
}

static void Oled_ClearBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ssd1306_SetColor(Black);
    ssd1306_FillRect(x, y, w, h);
    ssd1306_SetColor(White);
}

void OledUtils_DrawActionTimeoutBar(uint32_t startedMs, uint32_t timeoutMs)
{
    const uint32_t delayMs = 1000u;
    const uint8_t barHeight = 2u;
    uint32_t elapsedMs;
    uint32_t denominator;
    uint32_t remainingMs;
    uint8_t barWidth;

    Oled_ClearBox(0u, 0u, SSD1306_WIDTH, barHeight);

    if (timeoutMs <= delayMs) {
        return;
    }

    elapsedMs = Screens_NowMs() - startedMs;
    if (elapsedMs < delayMs || elapsedMs >= timeoutMs) {
        return;
    }

    denominator = timeoutMs - delayMs;
    remainingMs = timeoutMs - elapsedMs;
    if (remainingMs > denominator) {
        remainingMs = denominator;
    }

    barWidth = (uint8_t)((remainingMs * SSD1306_WIDTH + denominator - 1u) /
                         denominator);
    if (barWidth > SSD1306_WIDTH) {
        barWidth = SSD1306_WIDTH;
    }

    ssd1306_SetColor(White);
    ssd1306_FillRect(0u, 0u, barWidth, barHeight);
}

static void OledUtils_DrawDashboardActionTimeoutBar(void)
{
    uint32_t startedMs = 0u;
    uint32_t timeoutMs = 0u;

    if (Dashboard_GetActionTimeoutContext(&startedMs, &timeoutMs)) {
        OledUtils_DrawActionTimeoutBar(startedMs, timeoutMs);
    }
}

static const OledSimpleNotificationDef oled_simple_notifications[] = {
    [OLED_SIMPLE_NOTIFICATION_NONE] = { 0 },
    [OLED_SIMPLE_NOTIFICATION_WIFI_SEARCH_CANCELED] = {
        SCREEN_CODE_CONNECTIVITY_WIFI_SEARCH_CANCELED,
        Icon_Wifi_NotConnected_bits, "Busqueda", "cancelada",
        19u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_COMMAND_RECEIVED] = {
        SCREEN_CODE_DIAG_COMMAND_RECEIVED,
        Icon_Wifi_100_bits, "ECHO", "recibido",
        19u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PING_RECEIVED] = {
        SCREEN_CODE_DIAG_PING_RECEIVED,
        Icon_Info_bits, "PING", "recibido",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_BOOT_RECEIVED] = {
        SCREEN_CODE_CONNECTIVITY_ESP_BOOT_RECEIVED,
        Icon_Info_bits, "ESP", "iniciada",
        19u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_MSB, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_UNRESPONSIVE] = {
        SCREEN_CODE_CONNECTIVITY_ESP_UNRESPONSIVE,
        Icon_OperationWarning_bits, "ESP", "sin respuesta",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_WATCHDOG_RESET] = {
        SCREEN_CODE_CONNECTIVITY_ESP_WATCHDOG_RESET,
        Icon_OperationWarning_bits, "Reinicio ESP", "por watchdog",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_EXCEPTION_RESET] = {
        SCREEN_CODE_CONNECTIVITY_ESP_EXCEPTION_RESET,
        Icon_OperationWarning_bits, "Reinicio ESP", "por excepcion",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_MODE_CHANGED] = {
        SCREEN_CODE_CONNECTIVITY_ESP_MODE_CHANGED,
        Icon_Info_bits, "Modo", "actualizado",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_USB_CONNECTED] = {
        SCREEN_CODE_CONNECTIVITY_USB_CONNECTED,
        Icon_USB_bits, "USB", "conectado",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_USB_DISCONNECTED] = {
        SCREEN_CODE_CONNECTIVITY_USB_DISCONNECTED,
        Icon_USB_bits, "USB", "desconect",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 2u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_WEB_SERVER_UP] = {
        SCREEN_CODE_CONNECTIVITY_WEB_SERVER_UP,
        Icon_Info_bits, "WEB", "lista",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_WEB_CLIENT_CONNECTED] = {
        SCREEN_CODE_CONNECTIVITY_WEB_CLIENT_CONNECTED,
        Icon_Link_bits, "Web", "conectada",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_WEB_CLIENT_DISCONNECTED] = {
        SCREEN_CODE_CONNECTIVITY_WEB_CLIENT_DISCONNECTED,
        Icon_Link_bits, "Web", "desconectada",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 2u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_AP_DEVICE_CONNECTED] = {
        SCREEN_CODE_CONNECTIVITY_AP_DEVICE_CONNECTED,
        Icon_Link_bits, "Cliente", "conectado",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_AP_DEVICE_DISCONNECTED] = {
        SCREEN_CODE_CONNECTIVITY_AP_DEVICE_DISCONNECTED,
        Icon_Link_bits, "Cliente", "desconectado",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 2u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION] = {
        SCREEN_CODE_CONNECTIVITY_ESP_CHECKING,
        Icon_OperationWarning_bits, "Chequeando", "conexion ESP",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_CHECK_CONNECTION_REQUIRED] = {
        SCREEN_CODE_CONNECTIVITY_ESP_CHECK_REQUIRED,
        Icon_OperationWarning_bits, "Chequear", "conexion ESP",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED] = {
        SCREEN_CODE_DIAG_ESP_CONN_FAILED,
        Icon_OperationWarning_bits, "Fallo conexion", "a ESP",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PERMISSION_DENIED] = {
        SCREEN_CODE_WARNING_PERMISSION_DENIED,
        Icon_Lock_bits, "PIN no verificado", "Accion restringida",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PIN_TIMEOUT] = {
        SCREEN_CODE_WARNING_PIN_TIMEOUT,
        Icon_Lock_bits, "PIN sin resp.", "Valide otra vez",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_REMOTE_AUTHENTICATED] = {
        SCREEN_CODE_WARNING_REMOTE_AUTHENTICATED,
        Icon_Lock_bits, "Autenticado", "remoto",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PIN_CORRECT] = {
        SCREEN_CODE_WARNING_PIN_CORRECT,
        Icon_Lock_bits, "PIN correcto", "Acceso concedido",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PIN_INCORRECT] = {
        SCREEN_CODE_WARNING_PIN_INCORRECT,
        Icon_Lock_bits, "PIN incorrecto", "Acceso rechazado",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PIN_BYPASSED] = {
        SCREEN_CODE_WARNING_PIN_BYPASSED,
        Icon_Lock_bits, "PIN desactivado", "Acceso concedido",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_PIN_MODIFIED] = {
        SCREEN_CODE_WARNING_PIN_MODIFIED,
        Icon_Lock_bits, "PIN", "modificado",
        LOCK_ICON_W, LOCK_ICON_H, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_CONTROLLER_CONNECTED] = {
        SCREEN_CODE_DIAG_CONTROLLER_CONNECTED,
        Icon_Controller_bits, "Control", "conectado",
        37u, 27u, OLED_SIMPLE_LAYOUT_CENTER_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_CONTROLLER_DISCONNECTED] = {
        SCREEN_CODE_DIAG_CONTROLLER_DISCONNECTED,
        Icon_Controller_bits, "Control", "desconectado",
        37u, 27u, OLED_SIMPLE_LAYOUT_CENTER_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 3u
    },
    [OLED_SIMPLE_NOTIFICATION_MPU_STREAM_STARTED] = {
        SCREEN_CODE_SENSORS_MPU_STREAM_STARTED,
        Icon_Info_bits, "MPU stream", "iniciado",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_MPU_STREAM_STOPPED] = {
        SCREEN_CODE_SENSORS_MPU_STREAM_STOPPED,
        Icon_Info_bits, "MPU stream", "detenido",
        16u, 16u, OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_REMOTE_MODE_CHANGED] = {
        SCREEN_CODE_CORE_REMOTE_MODE_CHANGED,
        Icon_Info_bits, "Cambio remoto", "",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_UNER_FORWARDING_ENABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_UNER_FORWARDING_ENABLED,
        Icon_Link_bits, "Port forward", "activado",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_UNER_FORWARDING_DISABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_UNER_FORWARDING_DISABLED,
        Icon_Link_bits, "Port forward", "desactivado",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 2u
    },
    [OLED_SIMPLE_NOTIFICATION_EXT_DEFAULT] = {
        SCREEN_CODE_NONE,
        Icon_Puzzle_bits, "Ext. Normal", "cargada",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_EXT_BEEPER] = {
        SCREEN_CODE_NONE,
        Icon_Puzzle_bits, "Ext. Beeper", "cargada",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_EXT_NRF24] = {
        SCREEN_CODE_NONE,
        Icon_Puzzle_bits, "Ext. NRF24", "cargada",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_FAVORITE_ADDED] = {
        SCREEN_CODE_NONE,
        Icon_ChoiceBulletOn_bits, "Agregado", "a favoritos",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_FAVORITE_REMOVED] = {
        SCREEN_CODE_NONE,
        Icon_ChoiceBulletOff_bits, "Eliminado", "de favoritos",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_FAVORITE_FULL] = {
        SCREEN_CODE_NONE,
        Icon_OperationWarning_bits, "Favoritos", "lleno (max 3)",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_AP_CONFIG_SAVED] = {
        SCREEN_CODE_NONE, Icon_ChoiceBulletOn_bits, "Config AP", "guardada",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_AP_CONFIG_INVALID] = {
        SCREEN_CODE_NONE, Icon_OperationWarning_bits, "Config AP", "invalida",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_IR_OBJECTS_ENABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_IR_OBJECTS,
        Icon_Sensors_bits, "IR Objetos", "encendidos",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_IR_OBJECTS_DISABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_IR_OBJECTS,
        Icon_Sensors_bits, "IR Objetos", "apagados",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 2u
    },
    [OLED_SIMPLE_NOTIFICATION_IR_LINE_ENABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_IR_LINE,
        Icon_Sensors_bits, "IR Linea", "encendidos",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_IR_LINE_DISABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_IR_LINE,
        Icon_Sensors_bits, "IR Linea", "apagados",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 2u
    },
    [OLED_SIMPLE_NOTIFICATION_IR_LIGHTS_ENABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_IR_LIGHTS,
        Icon_Sensors_bits, "Luz IR", "encendidos",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_NONE, 0u
    },
    [OLED_SIMPLE_NOTIFICATION_IR_LIGHTS_DISABLED] = {
        SCREEN_CODE_SETTINGS_PREFS_IR_LIGHTS,
        Icon_Sensors_bits, "Luz IR", "apagados",
        16u, 16u, OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2, OLED_SIMPLE_ICON_FLAG_INVERSE, 2u
    }
};

void OledUtils_RenderMPUCompassScene(void) {
    // Stub
}

void OledUtils_UpdateMPUCompass(MPU9250_Handle_t *mpu) {
    (void)mpu;
    // Stub
}


static uint8_t Oled_IpIsValid(const volatile IPStruct_t *ip)
{
    return ip &&
           (ip->block1 != 0u ||
            ip->block2 != 0u ||
            ip->block3 != 0u ||
            ip->block4 != 0u);
}

static void Oled_Terminate(char *buf, size_t len, size_t pos)
{
    if (!buf || len == 0u) {
        return;
    }

    buf[(pos < len) ? pos : (len - 1u)] = '\0';
}

static size_t Oled_AppendChar(char *buf, size_t len, size_t pos, char ch)
{
    if (buf && pos + 1u < len) {
        buf[pos] = ch;
    }

    return pos + 1u;
}

static size_t Oled_AppendText(char *buf, size_t len, size_t pos, const char *text)
{
    if (!text) {
        return pos;
    }

    while (*text != '\0') {
        pos = Oled_AppendChar(buf, len, pos, *text);
        text++;
    }

    return pos;
}

static size_t Oled_AppendUnsigned(char *buf, size_t len, size_t pos, uint32_t value)
{
    char tmp[10];
    uint8_t count = 0u;

    do {
        tmp[count++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u && count < (uint8_t)sizeof(tmp));

    while (count > 0u) {
        pos = Oled_AppendChar(buf, len, pos, tmp[--count]);
    }

    return pos;
}

static void Oled_CopyText(char *buf, size_t len, const char *text)
{
    size_t pos = Oled_AppendText(buf, len, 0u, text);
    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatUnsigned(char *buf, size_t len, uint32_t value)
{
    size_t pos = Oled_AppendUnsigned(buf, len, 0u, value);
    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatSigned(char *buf, size_t len, int32_t value)
{
    size_t pos = 0u;
    uint32_t magnitude;

    if (value < 0) {
        pos = Oled_AppendChar(buf, len, pos, '-');
        magnitude = (uint32_t)(-(value + 1)) + 1u;
    } else {
        magnitude = (uint32_t)value;
    }

    pos = Oled_AppendUnsigned(buf, len, pos, magnitude);
    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatFixed(char *buf, size_t len, float value, uint8_t decimals)
{
    size_t pos = 0u;
    int32_t scaled;
    uint32_t magnitude;
    uint32_t scale = 1u;

    if (decimals > 3u) {
        decimals = 3u;
    }

    for (uint8_t i = 0u; i < decimals; i++) {
        scale *= 10u;
    }

    scaled = (int32_t)((value >= 0.0f)
        ? (value * (float)scale + 0.5f)
        : (value * (float)scale - 0.5f));

    if (scaled < 0) {
        pos = Oled_AppendChar(buf, len, pos, '-');
        magnitude = (uint32_t)(-(scaled + 1)) + 1u;
    } else {
        magnitude = (uint32_t)scaled;
    }

    pos = Oled_AppendUnsigned(buf, len, pos, magnitude / scale);

    if (decimals > 0u) {
        uint32_t frac = magnitude % scale;
        uint32_t pad = scale / 10u;

        pos = Oled_AppendChar(buf, len, pos, '.');
        while (pad > 1u && frac < pad) {
            pos = Oled_AppendChar(buf, len, pos, '0');
            pad /= 10u;
        }
        pos = Oled_AppendUnsigned(buf, len, pos, frac);
    }

    Oled_Terminate(buf, len, pos);
}

static size_t Oled_AppendHex2(char *buf, size_t len, size_t pos, uint8_t value)
{
    static const char hex[] = "0123456789ABCDEF";

    pos = Oled_AppendChar(buf, len, pos, '0');
    pos = Oled_AppendChar(buf, len, pos, 'x');
    pos = Oled_AppendChar(buf, len, pos, hex[(value >> 4) & 0x0Fu]);
    pos = Oled_AppendChar(buf, len, pos, hex[value & 0x0Fu]);
    return pos;
}

static size_t Oled_AppendHexByteCompact(char *buf, size_t len, size_t pos, uint8_t value)
{
    static const char hex[] = "0123456789ABCDEF";

    pos = Oled_AppendChar(buf, len, pos, hex[(value >> 4) & 0x0Fu]);
    pos = Oled_AppendChar(buf, len, pos, hex[value & 0x0Fu]);
    return pos;
}

static void Oled_FormatBytesCompact(char *buf, size_t len, uint32_t bytes)
{
    size_t pos = 0u;

    if (bytes >= 1024u) {
        const uint32_t kb = (bytes + 512u) / 1024u;
        pos = Oled_AppendUnsigned(buf, len, pos, kb);
        pos = Oled_AppendText(buf, len, pos, "KB");
    } else {
        pos = Oled_AppendUnsigned(buf, len, pos, bytes);
        pos = Oled_AppendChar(buf, len, pos, 'B');
    }

    Oled_Terminate(buf, len, pos);
}

static float OLED_MPU_Normalize180(float angle)
{
    while (angle > 180.0f) {
        angle -= 360.0f;
    }
    while (angle < -180.0f) {
        angle += 360.0f;
    }

    return angle;
}

static float OLED_MPU_Clamp(float value, float minValue, float maxValue)
{
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

static int16_t OLED_MPU_Round(float value)
{
    return (int16_t)((value >= 0.0f) ? (value + 0.5f) : (value - 0.5f));
}

static void OLED_DrawPixelSafe(int16_t x, int16_t y)
{
    if (x < 0 || y < 0 || x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    ssd1306_DrawPixel((uint8_t)x, (uint8_t)y);
}

static bool OLED_ReadXbmPixel(const uint8_t *bits, uint8_t w, uint8_t x, uint8_t y)
{
    const uint8_t bytesPerRow = (uint8_t)((w + 7u) / 8u);
    const uint16_t byteIndex = (uint16_t)y * bytesPerRow + (uint16_t)(x / 8u);
    const uint8_t bitIndex = (uint8_t)(x % 8u);

    return bits && ((bits[byteIndex] & (uint8_t)(1u << bitIndex)) != 0u);
}

static void OLED_DrawRotatedBitmap(int cx, int cy, uint8_t w, uint8_t h, const uint8_t *bits, float angle_deg)
{
    const float angle_rad = angle_deg * OLED_MPU_RAD_PER_DEG;
    const float c = cosf(angle_rad);
    const float s = sinf(angle_rad);
    const float srcCx = ((float)w - 1.0f) * 0.5f;
    const float srcCy = ((float)h - 1.0f) * 0.5f;

    if (!bits || w == 0u || h == 0u) {
        return;
    }

    for (uint8_t sy = 0u; sy < h; ++sy) {
        for (uint8_t sx = 0u; sx < w; ++sx) {
            if (!OLED_ReadXbmPixel(bits, w, sx, sy)) {
                continue;
            }

            const float rx = (float)sx - srcCx;
            const float ry = (float)sy - srcCy;
            const int16_t dx = OLED_MPU_Round((float)cx + (c * rx) - (s * ry));
            const int16_t dy = OLED_MPU_Round((float)cy + (s * rx) + (c * ry));
            OLED_DrawPixelSafe(dx, dy);
        }
    }
}

static uint8_t OLED_MiniGlyphRow(char c, uint8_t row)
{
    static const uint8_t digits[10][5] = {
        { 0x07u, 0x05u, 0x05u, 0x05u, 0x07u },
        { 0x02u, 0x06u, 0x02u, 0x02u, 0x07u },
        { 0x07u, 0x01u, 0x07u, 0x04u, 0x07u },
        { 0x07u, 0x01u, 0x07u, 0x01u, 0x07u },
        { 0x05u, 0x05u, 0x07u, 0x01u, 0x01u },
        { 0x07u, 0x04u, 0x07u, 0x01u, 0x07u },
        { 0x07u, 0x04u, 0x07u, 0x05u, 0x07u },
        { 0x07u, 0x01u, 0x02u, 0x02u, 0x02u },
        { 0x07u, 0x05u, 0x07u, 0x05u, 0x07u },
        { 0x07u, 0x05u, 0x07u, 0x01u, 0x07u },
    };

    if (row >= 5u) {
        return 0u;
    }

    if (c >= '0' && c <= '9') {
        return digits[(uint8_t)(c - '0')][row];
    }

    switch (c) {
        case 'A': { static const uint8_t glyph[5] = { 0x07u, 0x05u, 0x07u, 0x05u, 0x05u }; return glyph[row]; }
        case 'B': { static const uint8_t glyph[5] = { 0x06u, 0x05u, 0x06u, 0x05u, 0x06u }; return glyph[row]; }
        case 'C': { static const uint8_t glyph[5] = { 0x07u, 0x04u, 0x04u, 0x04u, 0x07u }; return glyph[row]; }
        case 'D': { static const uint8_t glyph[5] = { 0x06u, 0x05u, 0x05u, 0x05u, 0x06u }; return glyph[row]; }
        case 'E': { static const uint8_t glyph[5] = { 0x07u, 0x04u, 0x06u, 0x04u, 0x07u }; return glyph[row]; }
        case 'F': { static const uint8_t glyph[5] = { 0x07u, 0x04u, 0x06u, 0x04u, 0x04u }; return glyph[row]; }
        case 'I': { static const uint8_t glyph[5] = { 0x07u, 0x02u, 0x02u, 0x02u, 0x07u }; return glyph[row]; }
        case 'J': { static const uint8_t glyph[5] = { 0x01u, 0x01u, 0x01u, 0x05u, 0x07u }; return glyph[row]; }
        case 'K': { static const uint8_t glyph[5] = { 0x05u, 0x05u, 0x06u, 0x05u, 0x05u }; return glyph[row]; }
        case 'L': { static const uint8_t glyph[5] = { 0x04u, 0x04u, 0x04u, 0x04u, 0x07u }; return glyph[row]; }
        case 'M': { static const uint8_t glyph[5] = { 0x05u, 0x07u, 0x07u, 0x05u, 0x05u }; return glyph[row]; }
        case 'N': { static const uint8_t glyph[5] = { 0x05u, 0x07u, 0x07u, 0x07u, 0x05u }; return glyph[row]; }
        case 'O': { static const uint8_t glyph[5] = { 0x07u, 0x05u, 0x05u, 0x05u, 0x07u }; return glyph[row]; }
        case 'P': { static const uint8_t glyph[5] = { 0x06u, 0x05u, 0x06u, 0x04u, 0x04u }; return glyph[row]; }
        case 'R': { static const uint8_t glyph[5] = { 0x06u, 0x05u, 0x06u, 0x05u, 0x05u }; return glyph[row]; }
        case 'S': { static const uint8_t glyph[5] = { 0x07u, 0x04u, 0x07u, 0x01u, 0x07u }; return glyph[row]; }
        case 'T': { static const uint8_t glyph[5] = { 0x07u, 0x02u, 0x02u, 0x02u, 0x02u }; return glyph[row]; }
        case 'U': { static const uint8_t glyph[5] = { 0x05u, 0x05u, 0x05u, 0x05u, 0x07u }; return glyph[row]; }
        case 'V': { static const uint8_t glyph[5] = { 0x05u, 0x05u, 0x05u, 0x05u, 0x02u }; return glyph[row]; }
        case 'X': { static const uint8_t glyph[5] = { 0x05u, 0x05u, 0x02u, 0x05u, 0x05u }; return glyph[row]; }
        case 'Y': { static const uint8_t glyph[5] = { 0x05u, 0x05u, 0x02u, 0x02u, 0x02u }; return glyph[row]; }
        case 'Z': { static const uint8_t glyph[5] = { 0x07u, 0x01u, 0x02u, 0x04u, 0x07u }; return glyph[row]; }
        case '+': { static const uint8_t glyph[5] = { 0x00u, 0x02u, 0x07u, 0x02u, 0x00u }; return glyph[row]; }
        case '-': { static const uint8_t glyph[5] = { 0x00u, 0x00u, 0x07u, 0x00u, 0x00u }; return glyph[row]; }
        case ':': { static const uint8_t glyph[5] = { 0x00u, 0x02u, 0x00u, 0x02u, 0x00u }; return glyph[row]; }
        default:
            return 0u;
    }
}

static uint8_t OLED_MiniTextWidth(const char *text)
{
    uint8_t count = 0u;

    if (!text) {
        return 0u;
    }

    while (text[count] != '\0') {
        count++;
    }

    return (count == 0u) ? 0u : (uint8_t)((count * 4u) - 1u);
}

static void OLED_DrawMiniChar(uint8_t x, uint8_t y, char c)
{
    for (uint8_t row = 0u; row < 5u; ++row) {
        const uint8_t bits = OLED_MiniGlyphRow(c, row);

        for (uint8_t col = 0u; col < 3u; ++col) {
            if ((bits & (uint8_t)(1u << (2u - col))) != 0u) {
                OLED_DrawPixelSafe((int16_t)(x + col), (int16_t)(y + row));
            }
        }
    }
}

static void OLED_DrawMiniText(uint8_t x, uint8_t y, const char *text)
{
    uint8_t cursorX = x;

    if (!text) {
        return;
    }

    while (*text != '\0') {
        OLED_DrawMiniChar(cursorX, y, *text);
        cursorX = (uint8_t)(cursorX + 4u);
        text++;
    }
}

static void OLED_DrawMiniTextCentered(const char *text, uint8_t x, uint8_t w, uint8_t y)
{
    const uint8_t textWidth = OLED_MiniTextWidth(text);
    const uint8_t textX = (textWidth >= w) ? x : (uint8_t)(x + ((w - textWidth) / 2u));

    OLED_DrawMiniText(textX, y, text);
}

static void OLED_DrawMiniDegree(uint8_t x, uint8_t y)
{
    OLED_DrawPixelSafe(x, y);
    OLED_DrawPixelSafe((int16_t)(x + 1u), y);
    OLED_DrawPixelSafe(x, (int16_t)(y + 1u));
    OLED_DrawPixelSafe((int16_t)(x + 1u), (int16_t)(y + 1u));
}

static void OLED_FormatPanelAngle(char *buf, size_t len, const char *axis, float angle_deg)
{
    size_t pos = 0u;
    uint32_t magnitude;
    int16_t rounded = OLED_MPU_Round(angle_deg);

    if (!buf || len == 0u) {
        return;
    }

    pos = Oled_AppendChar(buf, len, pos, (axis && axis[0] != '\0') ? axis[0] : '?');
    pos = Oled_AppendChar(buf, len, pos, ':');

    if (rounded < 0) {
        pos = Oled_AppendChar(buf, len, pos, '-');
        magnitude = (uint32_t)(-(int32_t)rounded);
    } else {
        pos = Oled_AppendChar(buf, len, pos, '+');
        magnitude = (uint32_t)rounded;
    }

    if (magnitude < 10u) {
        pos = Oled_AppendChar(buf, len, pos, '0');
    }
    pos = Oled_AppendUnsigned(buf, len, pos, magnitude);
    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatWiFiSignal(char *buf, size_t len, int8_t signal_strength, uint8_t detail_valid)
{
    size_t pos = 0u;

    if (!buf || len == 0u) {
        return;
    }

    if (!detail_valid) {
        Oled_CopyText(buf, len, "Pidiendo detalle");
        return;
    }

    pos = Oled_AppendText(buf, len, pos, "RSSI ");
    if (signal_strength < 0) {
        pos = Oled_AppendChar(buf, len, pos, '-');
        pos = Oled_AppendUnsigned(buf, len, pos, (uint32_t)(-(int32_t)signal_strength));
    } else {
        pos = Oled_AppendUnsigned(buf, len, pos, (uint32_t)signal_strength);
    }
    pos = Oled_AppendText(buf, len, pos, " dBm");
    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatWiFiMeta(char *buf, size_t len, const char *encryption, uint8_t channel, uint8_t detail_valid)
{
    size_t pos = 0u;

    if (!buf || len == 0u) {
        return;
    }

    if (!detail_valid) {
        Oled_CopyText(buf, len, "Sin detalle");
        return;
    }

    if (encryption && encryption[0] != '\0') {
        pos = Oled_AppendText(buf, len, pos, encryption);
    } else {
        pos = Oled_AppendText(buf, len, pos, "ENC?");
    }

    if (channel != 0u) {
        pos = Oled_AppendText(buf, len, pos, "  ch ");
        pos = Oled_AppendUnsigned(buf, len, pos, channel);
    }

    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatPercent3(char *buf, size_t len, uint8_t value)
{
    size_t pos = 0u;

    if (value < 100u) {
        pos = Oled_AppendChar(buf, len, pos, ' ');
    }
    if (value < 10u) {
        pos = Oled_AppendChar(buf, len, pos, ' ');
    }

    pos = Oled_AppendUnsigned(buf, len, pos, value);
    pos = Oled_AppendChar(buf, len, pos, '%');
    Oled_Terminate(buf, len, pos);
}

static void Oled_FormatIp(const volatile IPStruct_t *ip, char *buf, size_t len)
{
    size_t pos;

    if (!buf || len == 0u) {
        return;
    }

    if (!Oled_IpIsValid(ip)) {
        Oled_CopyText(buf, len, "Sin IP");
        return;
    }

    pos = Oled_AppendUnsigned(buf, len, 0u, ip->block1);
    pos = Oled_AppendChar(buf, len, pos, '.');
    pos = Oled_AppendUnsigned(buf, len, pos, ip->block2);
    pos = Oled_AppendChar(buf, len, pos, '.');
    pos = Oled_AppendUnsigned(buf, len, pos, ip->block3);
    pos = Oled_AppendChar(buf, len, pos, '.');
    pos = Oled_AppendUnsigned(buf, len, pos, ip->block4);
    Oled_Terminate(buf, len, pos);
}

static void Oled_DrawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ssd1306_SetColor(White);
    ssd1306_FillRect(x, y, w, h);
}

static void Oled_DrawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ssd1306_SetColor(White);
    ssd1306_DrawRect(x, y, w, h);
}

static void Oled_DrawXBM(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bits)
{
    (void)0; // BREAKPOINT: impresión de bitmap en buffer
    ssd1306_SetColor(White);
    ssd1306_DrawBitmap(x, y, w, h, bits);
}

static void Oled_DrawXBM_MSB(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bits)
{
    (void)0; // BREAKPOINT: impresión de bitmap en buffer
    ssd1306_SetColor(White);
    ssd1306_DrawBitmap_MSB(x, y, w, h, bits);
}

// ============================================================================
// Wrappers de render
// ============================================================================

// Activa refresco continuo: permitimos que el MainTask siga solicitando render
void OledUtils_EnableContinuousRender(void) {
    menuSystem.allowPeriodicRefresh = true;
}

// Desactiva refresco continuo: bloquea el periodic
void OledUtils_DisableContinuousRender(void) {
    menuSystem.allowPeriodicRefresh = false;
}

static void OledUtils_RenderNotificationProgress(const OledNotificationData *data, uint8_t count)
{
    if (!data || data->totalTicks == 0) {
        return;
    }

    /* --- Progress bar (top 2px) --- */
    const uint16_t elapsed = (data->totalTicks > data->timeoutTicks)
        ? (uint16_t)(data->totalTicks - data->timeoutTicks)
        : 0U;
    ssd1306_DrawLinearProgress(0u,
                               0u,
                               SSD1306_WIDTH,
                               2u,
                               elapsed,
                               data->totalTicks);

    /* --- Queue counter "1/N" (bottom-right, black box background) --- */
    if (count > 1) {
        char buf[8];
        uint8_t queueIdx = 1;  /* always showing the first in queue */
        snprintf(buf, sizeof(buf), "%u/%u", queueIdx, count);

        Oled_SetFont(&Font_7x10);
        const uint8_t textW = Oled_TextPixelWidth(buf);
        const uint8_t textH = Oled_FontHeight();
        const uint8_t pad = 2;
        const uint8_t boxW = textW + pad * 2;
        const uint8_t boxH = textH + pad * 2;
        /* margin exactly 0 from each screen edge */
        const uint8_t margin = 0;
        const uint8_t boxX = (uint8_t)(SSD1306_WIDTH - boxW - margin);
        const uint8_t boxY = (uint8_t)(SSD1306_HEIGHT - boxH - margin);

        /* Draw black background */
        Oled_ClearBox(boxX, boxY, boxW, boxH);
        /* Draw text centered in box */
        ssd1306_SetColor(White);
        ssd1306_SetCursor((uint8_t)(boxX + pad), (uint8_t)(boxY + pad));
        ssd1306_WriteString(buf, Font_7x10);
    }
}

static void OledUtils_ExecuteNotificationHook(OledNotificationHook hook)
{
    if (hook) {
        hook();
    }
}

static const OledSimpleNotificationDef *OledUtils_GetSimpleNotificationDef(uint8_t simpleId)
{
    if (simpleId == 0u || simpleId >= (uint8_t)(sizeof(oled_simple_notifications) / sizeof(oled_simple_notifications[0]))) {
        return NULL;
    }

    return &oled_simple_notifications[simpleId];
}

static void OledUtils_DrawSimpleNotificationIcon(const OledSimpleNotificationDef *def, uint8_t x, uint8_t y)
{
    if (!def || !def->icon) {
        return;
    }

    if ((def->iconFlags & OLED_SIMPLE_ICON_FLAG_INVERSE) != 0u) {
        ssd1306_SetColor(Inverse);
    }

    if ((def->iconFlags & OLED_SIMPLE_ICON_FLAG_MSB) != 0u) {
        Oled_DrawXBM_MSB(x, y, def->iconW, def->iconH, def->icon);
    } else {
        Oled_DrawXBM(x, y, def->iconW, def->iconH, def->icon);
    }

    ssd1306_SetColor(White);
}

static void OledUtils_DrawSimpleNotificationStrike(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t strikeCount)
{
    for (uint8_t i = 0u; i < strikeCount; ++i) {
        ssd1306_DrawLine((int16_t)(x + i),
                         (int16_t)y,
                         (int16_t)(x + w + i),
                         (int16_t)(y + h));
    }
}

static void OledUtils_RenderSimpleNotification(void)
{
    const OledSimpleNotificationDef *def = OledUtils_GetSimpleNotificationDef(oledHandle.notification.queue[oledHandle.notification.head].simpleId);
    uint8_t iconX;
    uint8_t iconY;
    char numberedLine[16];
    const char *line1;

    if (!def) {
        return;
    }

    line1 = def->line1;
    const uint8_t clientNumber = oledHandle.notification.queue[oledHandle.notification.head].clientNumber;
    if (clientNumber != 0u &&
        (def->screenCode == SCREEN_CODE_CONNECTIVITY_WEB_CLIENT_CONNECTED ||
         def->screenCode == SCREEN_CODE_CONNECTIVITY_WEB_CLIENT_DISCONNECTED ||
         def->screenCode == SCREEN_CODE_CONNECTIVITY_AP_DEVICE_CONNECTED ||
         def->screenCode == SCREEN_CODE_CONNECTIVITY_AP_DEVICE_DISCONNECTED)) {
        snprintf(numberedLine, sizeof(numberedLine), "%s %u", def->line1, clientNumber);
        line1 = numberedLine;
    }

    OledUtils_SetScreenCode(def->screenCode, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    switch ((OledSimpleNotificationLayout)def->layout) {
        case OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2:
            OledUtils_DrawSimpleNotificationIcon(def, 2u, 12u);
            if (def->strikeCount != 0u) {
                OledUtils_DrawSimpleNotificationStrike(2u, 12u, def->iconW, def->iconH, def->strikeCount);
            }
            Oled_SetFont(&OLED_LARGE_FONT);
            Oled_DrawCenteredLine(line1, 24u, (uint8_t)(SSD1306_WIDTH - 24u), 30u);
            Oled_DrawCenteredLine((def->screenCode == SCREEN_CODE_CORE_REMOTE_MODE_CHANGED ? oled_remote_mode_notification_line : def->line2), 24u, (uint8_t)(SSD1306_WIDTH - 24u), 50u);
            break;

        case OLED_SIMPLE_LAYOUT_NO_ICON_LARGE_2:
            Oled_SetFont(&OLED_LARGE_FONT);
            Oled_DrawCenteredLine(line1, 0u, SSD1306_WIDTH, 30u);
            Oled_DrawCenteredLine((def->screenCode == SCREEN_CODE_CORE_REMOTE_MODE_CHANGED ? oled_remote_mode_notification_line : def->line2), 0u, SSD1306_WIDTH, 50u);
            break;

        case OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2:
            iconX = (uint8_t)((SSD1306_WIDTH - def->iconW) / 2u);
            iconY = 3u;
            OledUtils_DrawSimpleNotificationIcon(def, iconX, iconY);
            if (def->strikeCount != 0u) {
                OledUtils_DrawSimpleNotificationStrike(iconX, iconY, def->iconW, def->iconH, def->strikeCount);
            }
            Oled_SetFont(&Font_7x10);
            Oled_DrawCenteredLine(line1, 0u, SSD1306_WIDTH, 34u);
            Oled_DrawCenteredLine((def->screenCode == SCREEN_CODE_CORE_REMOTE_MODE_CHANGED ? oled_remote_mode_notification_line : def->line2), 0u, SSD1306_WIDTH, 52u);
            break;

        case OLED_SIMPLE_LAYOUT_CENTER_ICON_LARGE_2:
        default:
            iconX = (uint8_t)((SSD1306_WIDTH - def->iconW) / 2u);
            iconY = 4u;
            OledUtils_DrawSimpleNotificationIcon(def, iconX, iconY);
            if (def->strikeCount != 0u) {
                OledUtils_DrawSimpleNotificationStrike(iconX, iconY, def->iconW, def->iconH, def->strikeCount);
            }
            Oled_SetFont(&OLED_LARGE_FONT);
            Oled_DrawCenteredLine(line1, 0u, SSD1306_WIDTH, 48u);
            Oled_DrawCenteredLine((def->screenCode == SCREEN_CODE_CORE_REMOTE_MODE_CHANGED ? oled_remote_mode_notification_line : def->line2), 0u, SSD1306_WIDTH, 63u);
            break;
    }
}

static void OledUtils_RenderNotification_Wrapper(void)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active || notif->count == 0) return;

    OledNotificationData *data = &notif->queue[notif->head];

    if (data->renderFn && data->needsFullRender) {
        data->renderFn();
        data->needsFullRender = false;
    }

    OledUtils_RenderNotificationProgress(data, notif->count);

    if (data->timeoutTicks == data->totalTicks || (data->timeoutTicks % 10u) == 0u) {
        (void)UNER_App_ReportNotificationChanged(
            (uint32_t)MenuSys_GetCurrentScreenCode(&menuSystem),
            SCREEN_REPORT_SOURCE_NOTIFICATION,
            (uint16_t)(data->timeoutTicks * 10u),
            (uint16_t)(data->totalTicks * 10u));
    }
}

static void OledUtils_ClearDeferredNotification(OledNotificationState *notif)
{
    if (notif == NULL) {
        return;
    }

    notif->suspended.valid = false;
}

void OledUtils_NotificationRestore(void)
{
    OledNotificationState *notif = &oledHandle.notification;
    RenderFunction previousRenderFn = notif->previousRenderFn;
    UserEventManagerFn previousEventManagerFn = notif->previousEventManagerFn;
    bool previousAllowPeriodicRefresh = notif->previousAllowPeriodicRefresh;
    ScreenCode_t previousScreenCode = notif->previousScreenCode;
    uint8_t previousScreenSource = notif->previousScreenSource;
    ScreenCode_t notificationScreenCode = MenuSys_GetCurrentScreenCode(&menuSystem);
    uint8_t notificationScreenSource = MenuSys_GetCurrentScreenSource(&menuSystem);

    if (!notif->active) {
        OledUtils_ClearDeferredNotification(notif);
        return;
    }

    if (notif->count > 0 && notif->queue[notif->head].onHide != NULL) {
        OledUtils_ExecuteNotificationHook(notif->queue[notif->head].onHide);
    }

    notif->active = false;
    notif->head = 0;
    notif->tail = 0;
    notif->count = 0;
    OledUtils_ClearDeferredNotification(notif);

    menuSystem.renderFn = previousRenderFn;
    menuSystem.userEventManagerFn = previousEventManagerFn;
    menuSystem.allowPeriodicRefresh = previousAllowPeriodicRefresh;
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }

    if (previousScreenCode != SCREEN_CODE_NONE &&
        MenuSys_GetCurrentScreenCode(&menuSystem) == notificationScreenCode &&
        MenuSys_GetCurrentScreenSource(&menuSystem) == notificationScreenSource) {
        MenuSys_SetCurrentScreenCode(&menuSystem,
                                     previousScreenCode,
                                     (ScreenReportSource_t)previousScreenSource);
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static ScreenCode_t OledUtils_NotificationScreenCodeForRenderer(RenderFunction renderFn)
{
    if (renderFn == OledUtils_ShowWifiResults) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS;
    }
    if (renderFn == OledUtils_RenderWiFiNotConnected) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_NOT_CONNECTED;
    }
    if (renderFn == OledUtils_RenderESPWifiConnectingNotification) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_CONNECTING;
    }
    if (renderFn == OledUtils_RenderESPWifiConnectedNotification) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_CONNECTED;
    }
    if (renderFn == OledUtils_RenderWiFiCredentialsWebNotification) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_WEB;
    }
    if (renderFn == OledUtils_RenderWiFiCredentialsSucceededNotification) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_SUCCEEDED;
    }
    if (renderFn == OledUtils_RenderWiFiCredentialsFailedNotification) {
        return SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_FAILED;
    }
    if (renderFn == OledUtils_RenderESPFirmwareRequestNotification) {
        return SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_REQUEST;
    }
    if (renderFn == OledUtils_RenderESPResetSentNotification) {
        return SCREEN_CODE_CONNECTIVITY_ESP_RESET_SENT;
    }
    if (renderFn == OledUtils_RenderESPAPStartedNotification) {
        return SCREEN_CODE_CONNECTIVITY_ESP_AP_STARTED;
    }
    return SCREEN_CODE_NONE;
}

static void OledUtils_ShowNotificationTicks10msInternal(RenderFunction renderFn,
                                                        uint8_t simpleId,
                                                        uint8_t clientNumber,
                                                        uint16_t timeout_ticks,
                                                        OledNotificationHook onShow,
                                                        OledNotificationHook onHide)
{
    OledNotificationState *notif = &oledHandle.notification;

    if (!renderFn || oledNotificationsBlocked) {
        return;
    }

    if (timeout_ticks == 0) {
        timeout_ticks = 1;
    }

    if (OledCanvas_IsActiveScreen()) {
        notif->suspended.valid = true;
        notif->suspended.data.renderFn = renderFn;
        notif->suspended.data.simpleId = simpleId;
        notif->suspended.data.clientNumber = clientNumber;
        notif->suspended.data.timeoutTicks = timeout_ticks;
        notif->suspended.data.totalTicks = timeout_ticks;
        notif->suspended.data.needsFullRender = true;
        notif->suspended.data.onShow = onShow;
        notif->suspended.data.onHide = onHide;
        return;
    }

    OledUtils_ClearDeferredNotification(notif);

    if (notif->count >= OLED_MAX_NOTIFICATIONS) {
        return;
    }

    notif->queue[notif->tail].renderFn = renderFn;
    notif->queue[notif->tail].simpleId = simpleId;
    notif->queue[notif->tail].clientNumber = clientNumber;
    notif->queue[notif->tail].timeoutTicks = timeout_ticks;
    notif->queue[notif->tail].totalTicks = timeout_ticks;
    notif->queue[notif->tail].needsFullRender = true;
    notif->queue[notif->tail].onShow = onShow;
    notif->queue[notif->tail].onHide = onHide;

    notif->tail = (notif->tail + 1) % OLED_MAX_NOTIFICATIONS;
    notif->count++;

    if (!notif->active) {
        notif->previousRenderFn = menuSystem.renderFn;
        notif->previousEventManagerFn = menuSystem.userEventManagerFn;
        notif->previousAllowPeriodicRefresh = menuSystem.allowPeriodicRefresh;
        notif->previousRenderFlag = menuSystem.renderFlag;
        notif->previousScreenCode = MenuSys_GetCurrentScreenCode(&menuSystem);
        notif->previousScreenSource = MenuSys_GetCurrentScreenSource(&menuSystem);
        
        notif->active = true;
        MenuScroll_Stop();
        
        OledUtils_ExecuteNotificationHook(notif->queue[notif->head].onShow);

        menuSystem.renderFn = OledUtils_RenderNotification_Wrapper;
        menuSystem.userEventManagerFn = ClickCancelar_UserEventManager;
        menuSystem.allowPeriodicRefresh = false;

        ScreenCode_t nextNotificationScreenCode = SCREEN_CODE_NONE;
        if (simpleId != (uint8_t)OLED_SIMPLE_NOTIFICATION_NONE) {
            const OledSimpleNotificationDef *def = OledUtils_GetSimpleNotificationDef(simpleId);
            if (def != NULL && def->screenCode != SCREEN_CODE_NONE) {
                nextNotificationScreenCode = def->screenCode;
            }
        } else {
            nextNotificationScreenCode = OledUtils_NotificationScreenCodeForRenderer(renderFn);
        }

        if (nextNotificationScreenCode != SCREEN_CODE_NONE) {
            MenuSys_SetCurrentScreenCode(&menuSystem,
                                         nextNotificationScreenCode,
                                         SCREEN_REPORT_SOURCE_NOTIFICATION);
        }
    } else {
        if (notif->count == 1) {
            notif->queue[notif->head].needsFullRender = true;
        }
    }

    menuSystem.renderFlag = true;
}

static uint8_t OledUtils_ReplayDeferredNotification(OledNotificationState *notif)
{
    if (notif == NULL || notif->active || !notif->suspended.valid ||
        OledCanvas_IsActiveScreen() || oledNotificationsBlocked) {
        return 0u;
    }

    uint8_t replayed = notif->suspended.data.simpleId;
    OledUtils_ShowNotificationTicks10msInternal(notif->suspended.data.renderFn,
                                                notif->suspended.data.simpleId,
                                                notif->suspended.data.clientNumber,
                                                notif->suspended.data.timeoutTicks,
                                                notif->suspended.data.onShow,
                                                notif->suspended.data.onHide);
    OledUtils_ClearDeferredNotification(notif);

    return replayed;
}

void OledUtils_ShowNotificationMsEx(RenderFunction renderFn,
                                    uint16_t timeout_ms,
                                    OledNotificationHook onShow,
                                    OledNotificationHook onHide)
{
    uint16_t ticks = (uint16_t)((timeout_ms + 9U) / 10U);
    OledUtils_ShowNotificationTicks10msInternal(renderFn, 0u, 0u, ticks, onShow, onHide);
}

void OledUtils_ShowNotificationMs(RenderFunction renderFn, uint16_t timeout_ms)
{
    OledUtils_ShowNotificationMsEx(renderFn, timeout_ms, NULL, NULL);
}

bool OledUtils_IsNotificationShowing(RenderFunction renderFn)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active || notif->count == 0) return false;
    for (uint8_t i = 0; i < notif->count; i++) {
        uint8_t idx = (notif->head + i) % OLED_MAX_NOTIFICATIONS;
        if (notif->queue[idx].renderFn == renderFn) return true;
    }
    return false;
}

void OledUtils_ShowSimpleNotificationMsEx(OledSimpleNotificationId notificationId,
                                          uint16_t timeout_ms,
                                          OledNotificationHook onShow,
                                          OledNotificationHook onHide)
{
    OledUtils_ShowNumberedSimpleNotificationMsEx(notificationId, 0u, timeout_ms, onShow, onHide);
}

void OledUtils_ShowNumberedSimpleNotificationMsEx(OledSimpleNotificationId notificationId,
                                                   uint8_t clientNumber,
                                                   uint16_t timeout_ms,
                                                   OledNotificationHook onShow,
                                                   OledNotificationHook onHide)
{
    uint16_t ticks;

    if (OledUtils_GetSimpleNotificationDef((uint8_t)notificationId) == NULL) {
        return;
    }

    ticks = (uint16_t)((timeout_ms + 9U) / 10U);
    OledUtils_ShowNotificationTicks10msInternal(OledUtils_RenderSimpleNotification,
                                                (uint8_t)notificationId,
                                                clientNumber,
                                                ticks,
                                                onShow,
                                                onHide);
}

void OledUtils_ShowSimpleNotificationMs(OledSimpleNotificationId notificationId, uint16_t timeout_ms)
{
    OledUtils_ShowSimpleNotificationMsEx(notificationId,
                                         timeout_ms,
                                         NULL,
                                         NULL);
}

uint8_t OledUtils_GetNotificationClientNumber(void)
{
    const OledNotificationState *notif = &oledHandle.notification;
    return notif->active && notif->count ? notif->queue[notif->head].clientNumber : 0u;
}

bool OledUtils_IsSimpleNotificationShowing(OledSimpleNotificationId notificationId)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active || notif->count == 0) return false;
    for (uint8_t i = 0; i < notif->count; i++) {
        uint8_t idx = (notif->head + i) % OLED_MAX_NOTIFICATIONS;
        if (notif->queue[idx].simpleId == (uint8_t)notificationId) return true;
    }
    return false;
}



void OledUtils_NextNotification(void)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active || notif->count == 0) return;

    OledUtils_ExecuteNotificationHook(notif->queue[notif->head].onHide);

    notif->count--;
    notif->head = (notif->head + 1) % OLED_MAX_NOTIFICATIONS;

    if (notif->count == 0) {
        OledUtils_NotificationRestore();
    } else {
        notif->queue[notif->head].needsFullRender = true;
        OledUtils_ExecuteNotificationHook(notif->queue[notif->head].onShow);

        uint8_t simpleId = notif->queue[notif->head].simpleId;
        ScreenCode_t nextNotificationScreenCode = SCREEN_CODE_NONE;
        if (simpleId != (uint8_t)OLED_SIMPLE_NOTIFICATION_NONE) {
            const OledSimpleNotificationDef *def = OledUtils_GetSimpleNotificationDef(simpleId);
            if (def != NULL && def->screenCode != SCREEN_CODE_NONE) {
                nextNotificationScreenCode = def->screenCode;
            }
        }

        if (nextNotificationScreenCode != SCREEN_CODE_NONE) {
            OledUtils_SetScreenCode(nextNotificationScreenCode, SCREEN_REPORT_SOURCE_NOTIFICATION);
        } else {
            OledUtils_SetScreenCode(SCREEN_CODE_NONE, SCREEN_REPORT_SOURCE_NOTIFICATION);
        }
        menuSystem.renderFlag = true;
    }
}

void OledUtils_DismissNotification(void)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active) {
        OledUtils_ClearDeferredNotification(notif);
        return;
    }

    OledUtils_NextNotification();
}

void OledUtils_RemoveNotification(RenderFunction renderFn)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active || notif->count == 0) return;

    if (notif->queue[notif->head].renderFn == renderFn) {
        OledUtils_NextNotification();
        return;
    }

    for (uint8_t i = 1; i < notif->count; i++) {
        uint8_t idx = (notif->head + i) % OLED_MAX_NOTIFICATIONS;
        if (notif->queue[idx].renderFn == renderFn) {
            // Remove by shifting
            for (uint8_t j = i; j < notif->count - 1; j++) {
                uint8_t curr = (notif->head + j) % OLED_MAX_NOTIFICATIONS;
                uint8_t next = (notif->head + j + 1) % OLED_MAX_NOTIFICATIONS;
                notif->queue[curr] = notif->queue[next];
            }
            notif->count--;
            notif->tail = (notif->tail - 1 + OLED_MAX_NOTIFICATIONS) % OLED_MAX_NOTIFICATIONS;
            menuSystem.renderFlag = true;
            return;
        }
    }
}

void OledUtils_RemoveSimpleNotification(OledSimpleNotificationId notificationId)
{
    OledNotificationState *notif = &oledHandle.notification;
    if (!notif->active || notif->count == 0) return;

    if (notif->queue[notif->head].simpleId == (uint8_t)notificationId) {
        OledUtils_NextNotification();
        return;
    }

    for (uint8_t i = 1; i < notif->count; i++) {
        uint8_t idx = (notif->head + i) % OLED_MAX_NOTIFICATIONS;
        if (notif->queue[idx].simpleId == (uint8_t)notificationId) {
            // Remove by shifting
            for (uint8_t j = i; j < notif->count - 1; j++) {
                uint8_t curr = (notif->head + j) % OLED_MAX_NOTIFICATIONS;
                uint8_t next = (notif->head + j + 1) % OLED_MAX_NOTIFICATIONS;
                notif->queue[curr] = notif->queue[next];
            }
            notif->count--;
            notif->tail = (notif->tail - 1 + OLED_MAX_NOTIFICATIONS) % OLED_MAX_NOTIFICATIONS;
            menuSystem.renderFlag = true;
            return;
        }
    }
}


uint8_t OledUtils_GetNotificationCount(void)
{
    return oledHandle.notification.active ? oledHandle.notification.count : 0;
}

void OledUtils_SetNotificationsBlocked(bool blocked)
{
    oledNotificationsBlocked = blocked;
}

void OledUtils_NotificationTick10ms(void)
{
    OledNotificationState *notif = &oledHandle.notification;

    if (!notif->active) {
        (void)OledUtils_ReplayDeferredNotification(notif);
        return;
    }

    if (notif->count == 0) {
        OledUtils_NotificationRestore();
        return;
    }

    OledNotificationData *data = &notif->queue[notif->head];

    if (data->timeoutTicks > 0) {
        data->timeoutTicks--;
    }

    /* Force redraw so the progress bar updates */
    menuSystem.renderFlag = true;

    if (data->timeoutTicks == 0) {
        OledUtils_NextNotification();
    }
}

void OledUtils_MotorTest_Complete(void) {
    // 1) Determinar motor seleccionado desde nibble alto
    uint8_t sel = NIBBLEH_GET_STATE(motorTask.motorData.flags);
    const char *motorStr;
    switch (sel) {
        case MOTORLEFT_SELECTED:  motorStr = "Motor Izquierdo"; break;
        case MOTORRIGHT_SELECTED: motorStr = "Motor Derecho";   break;
        case MOTORNONE_SELECTED:  motorStr = "Ambos Motores";   break;
        default:                  motorStr = "Motor ?";         break;
    }

    // 2) Obtener velocidad y dirección según selección
    float speed = 0.0f;
    Motor_Direction_t dir = MOTOR_DIR_FORWARD;
    if (sel == MOTORLEFT_SELECTED) {
        speed = motorTask.motorData.motorLeft.speedPercent;
        dir   = motorTask.motorData.motorLeft.motorDirection;
    }
    else if (sel == MOTORRIGHT_SELECTED) {
        speed = motorTask.motorData.motorRight.speedPercent;
        dir   = motorTask.motorData.motorRight.motorDirection;
    }
    else { // Ambos
        float sL = motorTask.motorData.motorLeft.speedPercent;
        float sR = motorTask.motorData.motorRight.speedPercent;
        speed = (sL + sR) * 0.5f;
        dir   = (motorTask.motorData.motorLeft.motorDirection == motorTask.motorData.motorRight.motorDirection)
                ? motorTask.motorData.motorLeft.motorDirection
                : motorTask.motorData.motorLeft.motorDirection;
    }

    const char *dirStr = (dir == MOTOR_DIR_FORWARD) ? "Adelante" : "Atras";
    char speedStr[8];
    Oled_SetFont(&Font_7x10);

    // Línea 1: nombre del motor
    ssd1306_SetCursor(3, 11 - Oled_FontHeight());
    Oled_DrawStr(motorStr);

    // Línea 2: dirección
    ssd1306_SetCursor(41, 29 - Oled_FontHeight());
    Oled_DrawStr(dirStr);

    // Línea 3: marco de barra de velocidad
    Oled_DrawFrame(5, 34, 116, 11);

    // Línea 4: relleno de barra o estado PARADO
    if (IS_FLAG_SET(motorTask.motorData.flags, ENABLE_MOVEMENT)) {
        uint8_t speedPct = Motor_SpeedPercentToDisplay(speed);
        uint8_t barWidth = (uint16_t)speedPct * 116u / 100u;
        Oled_DrawBox(5, 34, barWidth, 11);

        Oled_FormatPercent3(speedStr, sizeof(speedStr), speedPct);
    } else {
        Oled_CopyText(speedStr, sizeof(speedStr), "PARADO");
    }

    // Línea 5: porcentaje o texto
    ssd1306_SetCursor(50, 57 - Oled_FontHeight());
    Oled_DrawStr(speedStr);
}

void OledUtils_MotorTest_Changes(void) {
    // 1) Determinar motor y texto
    uint8_t sel = NIBBLEH_GET_STATE(motorTask.motorData.flags);
    const char *motorStr = (sel == MOTORLEFT_SELECTED)  ? "Probando Motor 1" :
                           (sel == MOTORRIGHT_SELECTED) ? "Probando Motor 2" :
                                                          "Probando Ambos";
    // 2) Dirección
    Motor_Direction_t dir = (sel == MOTORRIGHT_SELECTED)
                            ? motorTask.motorData.motorRight.motorDirection
                            : motorTask.motorData.motorLeft.motorDirection;
    const char *dirStr = (dir == MOTOR_DIR_FORWARD) ? "Adelante" : "Atras";

    char speedStr[8];
    Oled_SetFont(&Font_7x10);

    // Línea 1: motor
    Oled_ClearBox(0, 11 - Oled_FontHeight(), 128, Oled_FontHeight());
    ssd1306_SetCursor(3,  11 - Oled_FontHeight());
    Oled_DrawStr(motorStr);

    // Línea 2: dirección
    Oled_ClearBox(0, 29 - Oled_FontHeight(), 128, Oled_FontHeight());
    ssd1306_SetCursor(41, 29 - Oled_FontHeight());
    Oled_DrawStr(dirStr);

    // Línea 3: barra
    Oled_ClearBox(5,  34, 116, 11);
    Oled_DrawFrame(5, 34, 116, 11);

    // Línea 4: relleno o PARADO
    if (IS_FLAG_SET(motorTask.motorData.flags, ENABLE_MOVEMENT)) {
        float speed = 0.0f;
        if (sel == MOTORRIGHT_SELECTED) {
            speed = motorTask.motorData.motorRight.speedPercent;
        } else if (sel == MOTORLEFT_SELECTED) {
            speed = motorTask.motorData.motorLeft.speedPercent;
        } else {
            speed = (motorTask.motorData.motorLeft.speedPercent +
                     motorTask.motorData.motorRight.speedPercent) * 0.5f;
        }
        uint8_t pct = Motor_SpeedPercentToDisplay(speed);
        uint8_t barW = (uint16_t)pct * 116u / 100u;
        Oled_DrawBox(5, 34, barW, 11);

        Oled_FormatPercent3(speedStr, sizeof(speedStr), pct);
    } else {
        Oled_CopyText(speedStr, sizeof(speedStr), "PARADO");
    }

    // Línea 5: porcentaje o estado
    Oled_ClearBox(0, 57 - Oled_FontHeight(), 128, Oled_FontHeight());
    ssd1306_SetCursor(50, 57 - Oled_FontHeight());
    Oled_DrawStr(speedStr);
}

static void Oled_ClearBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/* ── Menu text scroll state ────────────────────────────────────────────── */




/* Geometry captured at draw time so the tick can repaint without a full render */





   /* to skip identical frames */

static void Oled_DrawStrScrolled(const char *text, uint8_t min_x, uint8_t max_x, uint8_t y, int16_t offset_x)
{
    uint16_t len = strlen(text);
    const FontDef *font = oled_font;
    uint8_t compact_rows = (font->FontWidth <= 8u) ? 1u : 0u;

    for (uint16_t c = 0; c < len; c++) {
        char ch = text[c];
        uint8_t glyph = ((uint8_t)ch < 32u || (uint8_t)ch > 126u) ? (uint8_t)('?' - 32) : (uint8_t)((uint8_t)ch - 32u);

        int16_t char_x = (int16_t)min_x - offset_x + (int16_t)(c * font->FontWidth);

        if (char_x + (int16_t)font->FontWidth <= (int16_t)min_x) continue;
        if (char_x >= (int16_t)max_x) break;

        for (uint32_t i = 0; i < font->FontHeight; i++) {
            uint32_t row_index = ((uint32_t)glyph * font->FontHeight) + i;
            uint32_t b;
            if (compact_rows) {
                b = ((uint32_t)((const uint8_t *)font->data)[row_index]) << 8;
            } else {
                b = ((const uint16_t *)font->data)[row_index];
            }

            for (uint32_t j = 0; j < font->FontWidth; j++) {
                int16_t px = char_x + (int16_t)j;
                if (px >= (int16_t)min_x && px < (int16_t)max_x) {
                    if ((b << j) & 0x8000u) {
                        ssd1306_DrawPixel((uint8_t)px, y + (uint8_t)i);
                    }
                }
            }
        }
    }
}

/**
 * @brief  Standalone scroll tick – called from OLED_MainTask every ~40 ms.
 *         Patches ONLY the text rectangle in the framebuffer and pushes
 *         just the affected SSD1306 pages over I2C.
 */
void MenuScroll_Stop(void)
{
    menu_scroll_active = false;
    menu_scroll_current_item = NULL;
    scroll_text_ptr = NULL;
    scroll_last_offset = -1;
}

void MenuScroll_Tick(void)
{
    if (!menu_scroll_active || !scroll_text_ptr || scroll_text_len == 0) {
        return;
    }
    
    if (oledHandle.notification.active) {
        return;
    }

    uint32_t elapsed = Screens_NowMs() - menu_scroll_start_tick;
    if (elapsed <= 800) {
        return;  /* still in the dwell period */
    }

    uint32_t scroll_time = elapsed - 800;
    uint16_t extra_pixels = (scroll_text_len - scroll_max_chars) * Font_7x10.FontWidth + 10;
    int16_t offset_x = (int16_t)((scroll_time / 40) % (extra_pixels + 30));
    if (offset_x > (int16_t)extra_pixels) {
        offset_x = (int16_t)extra_pixels;
    }

    /* Skip if offset hasn't changed (avoids redundant I2C traffic) */
    if (offset_x == scroll_last_offset) {
        return;
    }
    scroll_last_offset = offset_x;

    if (!ssd1306_UpdateScreenCompleted()) {
        return;  /* previous DMA still in flight */
    }

    /* 1) Clear only the text box in the framebuffer */
    uint8_t box_w = scroll_max_chars * Font_7x10.FontWidth;
    Oled_ClearBox(scroll_text_x, scroll_text_y, box_w, Font_7x10.FontHeight);

    /* 2) Draw scrolled text into the framebuffer */
    Oled_SetFont(&Font_7x10);
    ssd1306_SetColor(White);
    Oled_DrawStrScrolled(scroll_text_ptr, scroll_text_x,
                         scroll_text_x + box_w, scroll_text_y, offset_x);

    /* 3) Push only the affected pages (8-pixel bands) */
    uint8_t page_start = scroll_text_y / 8;
    uint8_t page_end   = (scroll_text_y + Font_7x10.FontHeight - 1) / 8;
    for (uint8_t p = page_start; p <= page_end; p++) {
        ssd1306_RequestPageUpdate(p, I2C_REQ_PRIO_REGULAR, (p < page_end) ? 1 : 0);
    }
}

void OledUtils_Clear(void) {
    (void)0; // BREAKPOINT: limpieza del buffer OLED
    /* Kill any active text scroll on screen transition */
    MenuScroll_Stop();
    ssd1306_Clear();
}

/**
 * @brief  Dibuja un ítem de menú (texto + icono + cursor) directamente desde el MenuItem.
 */
void OledUtils_DrawItem(const MenuItem *item, uint8_t y, bool selected)
{
    const uint8_t *icon = NULL;
    const char *name = "";
    uint8_t textX = 6u;
    uint8_t maxChars = 16u;
    Oled_DrawFrame(0, y - 3, 128, 20);

    if (item) {
        icon = item->dynamicIconFn ? item->dynamicIconFn() : item->icon;
        name = item->dynamicNameFn ? item->dynamicNameFn() : item->name;
        if (!name) {
            name = "";
        }
    }

    if (icon) {
        uint8_t iconW, iconH;
        uint8_t iconY = (uint8_t)(y - 1u);
        if      (icon == Icon_Wifi_bits)       { iconW = 19; iconH = 16; }
        else if (icon == Icon_Volver_bits)     { iconW = 16; iconH = 16; }
        else if (icon == Icon_Sensors_bits)    { iconW = 14; iconH = 16; }
        else if (icon == Icon_Puzzle_bits)     { iconW = 16; iconH = 16; }
        else if (icon == Icon_Config_bits)     { iconW = 16; iconH = 16; }
        else if (icon == Icon_Encoder_bits)    { iconW = 13; iconH = 13; }
        else if (icon == Icon_Smartphone_bits) { iconW = 9;  iconH = 16; }
        else if (icon == Icon_Balance_bits)    { iconW = 16; iconH = 10; iconY = (uint8_t)(y + 3u); }
        else {
            iconW = 16; iconH = 16;
        }
        Oled_DrawXBM(6, iconY, iconW, iconH, icon);
        textX = 6 + iconW + 5u;
        maxChars = (118 - textX) / 7u;
    } else {
        textX = 6u;
        maxChars = (118 - textX) / 7u;
    }

    /* ── Track which item is selected for scroll ── */
    if (selected) {
        if (menu_scroll_current_item != item) {
            menu_scroll_current_item = item;
            menu_scroll_start_tick = Screens_NowMs();
            scroll_last_offset = -1;
        }
    } else {
        if (menu_scroll_current_item == item) {
            menu_scroll_current_item = NULL;
            menu_scroll_active = false;
            scroll_text_ptr = NULL;
        }
    }

    Oled_SetFont(&Font_7x10);
    uint16_t text_len = strlen(name);

    if (selected && text_len > maxChars) {
        /* Arm the scroll – store geometry for MenuScroll_Tick */
        menu_scroll_active = true;
        scroll_text_x    = textX;
        scroll_text_y    = y + 3;
        scroll_max_chars = maxChars;
        scroll_text_ptr  = name;
        scroll_text_len  = text_len;

        /* For the initial draw (offset 0) just truncate normally */
        ssd1306_SetCursor(textX, y + 3);
        Oled_DrawStrMax(name, maxChars);
    } else {
        if (selected) {
            menu_scroll_active = false;
            scroll_text_ptr = NULL;
        }
        ssd1306_SetCursor(textX, y+3);
        Oled_DrawStrMax(name, maxChars);
    }
    /* ── Cursor / Favorito ── */
    const uint8_t CUR_FAV_W = 8, CUR_FAV_H = 12;
    const uint8_t CUR_W = 7, CUR_H = 16;
    const uint8_t CUR_X = 119, CUR_Y = y;
    bool isFav = !MenuSys_IsFavoritesMenu(menuSystem.currentMenu)
              && MenuSys_IsFavorite(item);

    if (selected) {
        if (isFav) {
            Oled_ClearBox(CUR_X - 4, CUR_Y, CUR_FAV_W + 4, CUR_H);
            Oled_DrawXBM(CUR_X - 4, y + 2, CUR_FAV_W, CUR_FAV_H, Icon_CursorFav_bits);
        } else {
            Oled_DrawXBM(CUR_X, CUR_Y, CUR_W, CUR_H, Icon_Cursor_bits);
        }
    } else {
        Oled_ClearBox(CUR_X - 4, CUR_Y, CUR_FAV_W + 4, CUR_H);
    }
}
/* Reserve the whole right status area even when its indicators are hidden.
 * Both text lanes are clipped to pixels 0..63; icons own pixels 64..127. */
#define OLED_DASHBOARD_TEXT_RIGHT 64u

/* Independent lanes retain their phase across dashboard redraws. */





static int16_t DashboardScroll_Offset(const DashboardScrollLane *lane, uint32_t now)
{
    const uint16_t textWidth = (uint16_t)(strlen(lane->text) * Font_7x10.FontWidth);
    const uint16_t width = lane->right - lane->x;
    if (textWidth <= width) return 0;
    const uint16_t overflow = textWidth - width;
    const uint32_t travelMs = (uint32_t)overflow * 40u;
    const uint32_t phase = (now - lane->startedMs) % (1600u + travelMs);
    if (phase < 800u) return 0;
    if (phase >= 800u + travelMs) return (int16_t)overflow;
    return (int16_t)((phase - 800u) / 40u);
}

static void DashboardScroll_DrawLane(uint8_t index, const char *text,
                                     uint8_t x, uint8_t right)
{
    DashboardScrollLane *lane = &dashboardScrollLanes[index];
    if (right > OLED_DASHBOARD_TEXT_RIGHT) right = OLED_DASHBOARD_TEXT_RIGHT;
    if (x >= right) return;
    const uint32_t now = Screens_NowMs();
    if (!dashboardScrollActive || strcmp(lane->text, text) != 0 ||
        lane->x != x || lane->right != right) {
        Oled_CopyText(lane->text, sizeof(lane->text), text);
        lane->x = x;
        lane->right = right;
        lane->startedMs = now;
    }
    lane->lastOffset = DashboardScroll_Offset(lane, now);
    Oled_DrawStrScrolled(lane->text, x, right, index * 10u, lane->lastOffset);
}

void OledUtils_DashboardScrollTick(void)
{
    if (menuSystem.renderFn != OledUtils_RenderDashboard_Wrapper ||
        oledHandle.notification.active) {
        dashboardScrollActive = false;
        return;
    }
    if (!dashboardScrollActive || menuSystem.renderFlag ||
        !ssd1306_UpdateScreenCompleted()) return;

    bool changed = false;
    Oled_SetFont(&Font_7x10);
    ssd1306_SetColor(White);
    for (uint8_t i = 0u; i < 2u; ++i) {
        DashboardScrollLane *lane = &dashboardScrollLanes[i];
        const int16_t offset = DashboardScroll_Offset(lane, Screens_NowMs());
        if (offset == lane->lastOffset) continue;
        const uint8_t y = i * 10u;
        Oled_ClearBox(lane->x, y, lane->right - lane->x, 10u);
        Oled_DrawStrScrolled(lane->text, lane->x, lane->right, y, offset);
        lane->lastOffset = offset;
        changed = true;
    }
    if (changed) {
        /* Both lanes occupy pages 0..2; other pixels remain intact. */
        for (uint8_t page = 0u; page < 3u; ++page) {
            ssd1306_RequestPageUpdate(page, I2C_REQ_PRIO_REGULAR, page < 2u);
        }
    }
}

void OledUtils_RenderDashboard(void)
{
    MenuScroll_Stop();
    OledUtils_SetScreenCode(SCREEN_CODE_CORE_DASHBOARD, SCREEN_REPORT_SOURCE_RENDER);
    Oled_SetFont(&Font_7x10);
    const uint8_t fh = Oled_FontHeight();
    const char *backendLabel = WifiBridge_GetBackendLabel(&wifiBridge);
    char ipStr[20];

    const uint8_t textRight = OLED_DASHBOARD_TEXT_RIGHT;
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT) &&
        (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE) || IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE))) {
        const bool showSta = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
        const uint8_t textX = showSta ? 1u : 21u;
        Oled_FormatIp(showSta ? &autoMicroSystem.connectivity.espWifiConnection.staIp : &autoMicroSystem.connectivity.espWifiConnection.apIp,
                      ipStr, sizeof(ipStr));
        if (!showSta) Oled_DrawXBM(1, 2, 15, 16, Icon_APWifi_bits);
        DashboardScroll_DrawLane(0u, showSta ? Oled_GetDashboardStaSsid()
            : (autoMicroSystem.connectivity.espWifiConnection.apSsid[0] ? autoMicroSystem.connectivity.espWifiConnection.apSsid : "AP"),
            textX, textRight);
        DashboardScroll_DrawLane(1u, ipStr, textX, textRight);
        dashboardScrollActive = true;
    } else {
        dashboardScrollActive = false;
        if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT)) {
            Oled_DrawXBM(1, 2, 16, 16, Icon_Wifi_NotConnected_bits);
            ssd1306_SetCursor(21, 14 - fh);
            Oled_DrawStr("Sin WiFi");
        } else {
            ssd1306_SetCursor(2, 10 - fh);
            Oled_DrawStr("No ESP");
            ssd1306_SetCursor(2, 20 - fh);
            Oled_DrawStr("Sin WiFi");
        }
    }

    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE)) {
        Oled_DrawXBM(91, 2, 16, 16, Icon_USB_bits);
    }
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, RF_ACTIVE)) {
        Oled_DrawXBM(110, 2, 17, 16, Icon_RF_bits);
    }
    ssd1306_DrawHorizontalLine(1, 20, 125);

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(2, 42 - Oled_FontHeight());
    Oled_DrawStr(backendLabel);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(80, 38 - Oled_FontHeight());
    switch (GET_CAR_MODE()) {
		case IDLE_MODE:
		    Oled_DrawStr("IDLE");
			break;
		case FOLLOW_MODE:
		    Oled_DrawStr("FOLLOW");
			break;
		case TEST_MODE:
		    Oled_DrawStr("TEST");
			break;
		case GOTO_MODE:
		    Oled_DrawStr("GOTO");
			break;
		default:
			Oled_DrawStr("DEF");
			break;
	}

    Oled_DrawXBM(4, 47, 15, 16, Icon_UserBtn_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(23, 61 - Oled_FontHeight());
    Oled_DrawStr("Menu");

    ssd1306_SetCursor(80, 61 - Oled_FontHeight());
    Oled_DrawStr("Modo");

    Oled_DrawXBM(111, 48, 13, 13, Icon_Encoder_bits);
}

/**
 * @brief Pantalla principal de información del proyecto
 */
void OledUtils_RenderStabilizationPrompt(void)
{
    const uint8_t armed = MotionControl_IsArmed(&motionControl);
    const uint8_t *actionIcon = armed ? Icon_Stop_bits : Icon_Play_bits;

    OledUtils_SetScreenCode(SCREEN_CODE_CORE_STABILIZE_PROMPT, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    Oled_SetFont(&Font_11x18);
    Oled_DrawCenteredLine("Estabilizar", 0u, SSD1306_WIDTH, 20u);

    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(armed ? "Balance activo" : "Balance apagado",
                          0u,
                          SSD1306_WIDTH,
                          35u);

    Oled_DrawXBM(14u, 48u, 4u, 7u, Arrow_Left_bits);
    ssd1306_SetCursor(24u, 58u - Oled_FontHeight());
    Oled_DrawStr("Modo");

    Oled_DrawXBM(82u, 48u, 13u, 13u, actionIcon);
    Oled_DrawXBM(100u, 48u, 13u, 13u, Icon_Encoder_bits);
    OledUtils_DrawDashboardActionTimeoutBar();
}

bool OledUtils_GetNotificationProgress(uint16_t *remaining_ms, uint16_t *total_ms)
{
    OledNotificationState *notif = &oledHandle.notification;

    if (!notif->active || notif->count == 0) {
        return false;
    }

    if (remaining_ms) {
        *remaining_ms = (uint16_t)(notif->queue[notif->head].timeoutTicks * 10u);
    }
    if (total_ms) {
        *total_ms = (uint16_t)(notif->queue[notif->head].totalTicks * 10u);
    }
    return true;
}

void OledUtils_ShowRemoteModeChangedNotification(uint8_t mode, uint16_t timeout_ms)
{
    const char *label = "desconocido";

    switch ((CarMode_t)mode) {
        case IDLE_MODE: label = "a IDLE"; break;
        case FOLLOW_MODE: label = "a FOLLOW"; break;
        case TEST_MODE: label = "a TEST"; break;
        case GOTO_MODE: label = "a GOTO"; break;
        default: break;
    }

    Oled_CopyText(oled_remote_mode_notification_line,
                  sizeof(oled_remote_mode_notification_line),
                  label);
    OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_REMOTE_MODE_CHANGED,
                                       timeout_ms);
}

void OledUtils_RenderModePrompt(void)
{
    const char *modeText = "DEF";

    OledUtils_SetScreenCode(SCREEN_CODE_CORE_MODE_PROMPT, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    switch (GET_CAR_MODE()) {
        case IDLE_MODE:
            modeText = "IDLE";
            break;
        case FOLLOW_MODE:
            modeText = "FOLLOW";
            break;
        case TEST_MODE:
            modeText = "TEST";
            break;
        case GOTO_MODE:
            modeText = "GOTO";
            break;
        default:
            break;
    }

    Oled_SetFont(&Font_11x18);
    Oled_DrawCenteredLine("Modo", 0u, SSD1306_WIDTH, 20u);

    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(modeText, 0u, SSD1306_WIDTH, 37u);

    Oled_DrawXBM(110u, 48u, 4u, 7u, Arrow_Right_bits);

    Oled_DrawXBM(23u, 48u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(40u, 58u - Oled_FontHeight());
    Oled_DrawStr("Seleccion");
    OledUtils_DrawDashboardActionTimeoutBar();
}

void OledUtils_RenderProyectScreen(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SETTINGS_ABOUT_PROJECT, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    // Texto principal "Proyecto Auto" con fuente grande
    Oled_SetFont(&Font_11x18);
    const uint8_t fh_grande = Oled_FontHeight();
    ssd1306_SetCursor(40, 19 - fh_grande);
    Oled_DrawStr("Auto");

    // Texto secundario "Microcontroladores" con fuente pequeña
    Oled_SetFont(&Font_7x10);
    const uint8_t fh_pequena = Oled_FontHeight();
    ssd1306_SetCursor(0, 30 - fh_pequena);
    Oled_DrawStr("Microcontroladores");

    // Icono de información (15x16 píxeles)
    Oled_DrawXBM(20, 2, 15, 16, Icon_Info_bits);

    // Flecha derecha (4x7 píxeles)

    // Texto "Github"
    // Nombre "Koba"
    ssd1306_SetCursor(20, 42 - fh_pequena);
    Oled_DrawStr("Koba Chajchir");

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(4, 62 - fh_grande);
    Oled_DrawStr("2026");

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(76, 57 - fh_pequena);
    Oled_DrawStr("v: 2.1");
}

/**
 * @brief Pantalla secundaria con QR del repositorio
 */
void OledUtils_RenderProyectInfoScreen(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SETTINGS_ABOUT_REPO, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    // Flecha izquierda (4x7 píxeles)
    Oled_DrawXBM(12, 50, 4, 7, Arrow_Left_bits);

    // Texto "Repo" con fuente pequeña
    Oled_SetFont(&Font_7x10);
    const uint8_t fh = Oled_FontHeight();
    ssd1306_SetCursor(11, 13 - fh);
    Oled_DrawStr("Repo");
    ssd1306_SetCursor(2, 25 - fh);
    Oled_DrawStr("Firmware");
    ssd1306_SetCursor(2, 37 - fh);
    Oled_DrawStr("STM32");

    /*
     * El QR ya no vive en flash STM. Al renderizar esta pantalla se pide al
     * backend activo y se dibuja cuando llega completo.
     */
    // QR Code del repositorio (64x64 píxeles)
    Oled_DrawXBM_MSB(64u, 0u, 64u, 64u, QRCode_Github_bits);
}

void OledUtils_RenderBuildInfoScreen(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SETTINGS_ABOUT_BUILD, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    Oled_DrawXBM(4, 18, 16, 16, Icon_Info_bits);

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(30, 18 - Oled_FontHeight());
    Oled_DrawStr("Build");

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(30, 34 - Oled_FontHeight());
    Oled_DrawStrMax(buildDate, 13u);

    ssd1306_SetCursor(30, 47 - Oled_FontHeight());
    Oled_DrawStrMax(buildTime, 13u);

    Oled_DrawXBM(28u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(47u, 62u - Oled_FontHeight());
    Oled_DrawStr("Volver");
}

void OledUtils_RenderAboutBootloaderScreen(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SETTINGS_ABOUT_BOOTLOADER, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    Oled_DrawXBM(4, 18, 16, 16, Icon_Info_bits);

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(30, 18 - Oled_FontHeight());
    Oled_DrawStr("Bootloader");

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(30, 34 - Oled_FontHeight());
    
    uint32_t bl_version = 0;
    if (UNER_App_GetBootloaderVersion(&bl_version)) {
        char buf[16];
        (void)snprintf(buf, sizeof(buf), "v%lu.%lu.%lu",
                 AUTOMICRO_VERSION_MAJOR(bl_version),
                 AUTOMICRO_VERSION_MINOR(bl_version),
                 AUTOMICRO_VERSION_PATCH(bl_version));
        Oled_DrawStrMax(buf, 13u);
    } else {
        Oled_DrawStrMax("Desconocido", 13u);
    }

    Oled_DrawXBM(28u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(47u, 62u - Oled_FontHeight());
    Oled_DrawStr("Volver");
}

void OledUtils_RenderGenericConfirm(void)
{
    // The screen code should ideally be passed in, but we might just keep the current one from context
    // Actually we don't have a getter for screenCode, but MenuSys_SetCurrentScreenCode is already called by the wrapper.
    // So we don't need to call OledUtils_SetScreenCode here if it's in the wrapper.
    // Wait, the wrapper does call MenuSys_SetCurrentScreenCode.
    // Let's just do ssd1306_SetColor(White)
    ssd1306_SetColor(White);

    Oled_SetFont(&Font_11x18);
    
    const char* l1 = GenericConfirm_GetLine1();
    const char* l2 = GenericConfirm_GetLine2();

    if (l1 && l2) {
        ssd1306_SetCursor(20, 20 - Oled_FontHeight());
        Oled_DrawStrMax(l1, 10u);
        
        ssd1306_SetCursor(30, 40 - Oled_FontHeight());
        Oled_DrawStrMax(l2, 10u);
    } else if (l1) {
        ssd1306_SetCursor(20, 30 - Oled_FontHeight());
        Oled_DrawStrMax(l1, 10u);
    }

    Oled_DrawXBM(48u, 49u, 13u, 13u, Icon_Encoder_bits);
    
    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(65u, 62u - Oled_FontHeight());
    Oled_DrawStr("OK");
}

void OledUtils_RenderModeChange_Full(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CORE_MODE_CHANGE, SCREEN_REPORT_SOURCE_RENDER);
    // Título "Modo" con fuente grande
    Oled_SetFont(&Font_11x18);
    const uint8_t fh_grande = Oled_FontHeight();
    const uint8_t fw_grande = Oled_FontWidth();

    // Centrar título "Modo" (4 caracteres)
    uint8_t titulo_x = (SSD1306_WIDTH - (4 * fw_grande)) / 2;
    ssd1306_SetCursor(titulo_x, 20 - fh_grande);
    Oled_DrawStr("Modo");

    // Dibujar modo actual
    OledUtils_RenderModeChange_ModeOnly();

    // Flechas centradas verticalmente con el texto
    uint8_t modo_y = 42 - fh_grande;
    uint8_t arrow_y = modo_y + (fh_grande / 2) - 3;
    Oled_DrawXBM(14, arrow_y, 4, 7, Arrow_Left_bits);
    Oled_DrawXBM(110, arrow_y, 4, 7, Arrow_Right_bits);

    // Texto "Confirmar" e icono
    Oled_SetFont(&Font_7x10);
    const uint8_t fh_pequena = Oled_FontHeight();
    const uint8_t fw_pequena = Oled_FontWidth();

    const uint8_t icon_width = 13;
    const uint8_t icon_height = 13;
    const uint8_t separacion = 3;

    // "Confirmar" = 9 caracteres
    uint8_t texto_width = 9 * fw_pequena;
    uint8_t total_width = icon_width + separacion + texto_width;

    // Centrar el conjunto
    uint8_t start_x = (SSD1306_WIDTH - total_width) / 2;
    uint8_t icon_x = start_x;
    uint8_t icon_y = 50;
    uint8_t texto_x = icon_x + icon_width + separacion;
    uint8_t texto_y = icon_y + (icon_height / 2) - (fh_pequena / 2);

    // Dibujar
    Oled_DrawXBM(icon_x, icon_y, icon_width, icon_height, Icon_Encoder_bits);
    ssd1306_SetCursor(texto_x, texto_y);
    Oled_DrawStr("Confirmar");
    OledUtils_DrawDashboardActionTimeoutBar();
}

void OledUtils_RenderModeChange_ModeOnly(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CORE_MODE_CHANGE, SCREEN_REPORT_SOURCE_RENDER);
    // Fuente grande para el modo
    Oled_SetFont(&Font_11x18);
    const uint8_t fh_grande = Oled_FontHeight();
    const uint8_t fw_grande = Oled_FontWidth();

    // Limpiar zona del modo (usar "FOLLOW" = 6 caracteres, la más larga)
    const uint8_t max_modo_len = 6;  // "FOLLOW" es la más larga
    uint8_t max_modo_width = max_modo_len * fw_grande;
    uint8_t modo_clear_x = (SSD1306_WIDTH - max_modo_width) / 2;
    uint8_t modo_y = 42 - fh_grande;

    // Limpiar zona del texto del modo
    Oled_ClearBox(modo_clear_x, modo_y, max_modo_width, fh_grande);

    // Obtener texto del modo actual
    const char *modo_texto;
    uint8_t modo_len;
    switch(autoMicroSystem.runtime.auxCarMode)
    {
        case IDLE_MODE:
            modo_texto = "IDLE";
            modo_len = 4;
            break;

        case FOLLOW_MODE:
            modo_texto = "FOLLOW";
            modo_len = 6;
            break;

        case TEST_MODE:
            modo_texto = "TEST";
            modo_len = 4;
            break;

        case GOTO_MODE:
            modo_texto = "GOTO";
            modo_len = 4;
            break;

        default:
            modo_texto = "ERROR";
            modo_len = 5;
            break;
    }

    // Dibujar texto del modo centrado
    uint8_t modo_x = (SSD1306_WIDTH - (modo_len * fw_grande)) / 2;
    ssd1306_SetCursor(modo_x, modo_y);
    Oled_DrawStr(modo_texto);
    OledUtils_DrawDashboardActionTimeoutBar();
}



void Preferences_BeginEspNotifWatchdogLimitEdit(void *param)
{
    (void)param;
    appEspNotifWatchdogLimitDraft = AppPreferences_GetEspNotifWatchdogLimit();
    if (menuSystem.clearScreen != NULL) {
        menuSystem.clearScreen();
    }
    OledUtils_RenderEspNotifWatchdogLimit_Full();
}

uint8_t Preferences_GetEspNotifWatchdogLimitDraft(void)
{
    return appEspNotifWatchdogLimitDraft;
}

void Preferences_SetEspNotifWatchdogLimitDraft(uint8_t limit)
{
    appEspNotifWatchdogLimitDraft = limit;
}

/* Shared drawing fragments keep each public renderer's call order and layout. */
static void Oled_DrawSettingHeader(ScreenCode_t screenCode, const char *title)
{
    OledUtils_SetScreenCode(screenCode, SCREEN_REPORT_SOURCE_RENDER);
    Oled_DrawTitleLine(title, 0u, SSD1306_WIDTH, 18u);
    Oled_DrawXBM(5u, 27u, 4u, 7u, Arrow_Left_bits);
    Oled_DrawXBM(119u, 27u, 4u, 7u, Arrow_Right_bits);
}

static void Oled_DrawSaveFooter(void)
{
    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(31u, 62u - Oled_FontHeight());
    Oled_DrawStr("Guardar");
    Oled_DrawXBM(12u, 49u, 13u, 13u, Icon_Encoder_bits);
}

static void Oled_ClearSettingValue(const FontDef *font, uint8_t baselineY)
{
    Oled_SetFont(font);
    Oled_ClearBox(0u, (uint8_t)(baselineY - Oled_FontHeight()), SSD1306_WIDTH, Oled_FontHeight());
}

/* The caller retains its buffer capacity and samples its value at the same point. */
static void Oled_DrawUnsignedSettingValue(char *value, size_t capacity,
                                         uint32_t number, const char *unit,
                                         uint8_t baselineY)
{
    size_t pos;

    Oled_FormatUnsigned(value, capacity, number);
    pos = Oled_AppendText(value, capacity, strlen(value), unit);
    Oled_Terminate(value, capacity, pos);
    Oled_DrawCenteredLine(value, 0u, SSD1306_WIDTH, baselineY);
}

void OledUtils_RenderEspNotifWatchdogLimit_Full(void)
{
    Oled_DrawSettingHeader(SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU, "Mutear ESP");
    OledUtils_RenderEspNotifWatchdogLimit_ValueOnly();
    Oled_DrawSaveFooter();
}

void OledUtils_RenderEspNotifWatchdogLimit_ValueOnly(void)
{
    char value[16];
    uint8_t limit = appEspNotifWatchdogLimitDraft;

    Oled_ClearSettingValue(&Font_11x18, 38u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), limit, " vec.", 38u);
}

void OledUtils_RenderScreenLockTimeout_Full(void)
{
    Oled_DrawSettingHeader(SCREEN_CODE_SETTINGS_PREFS_DISPLAY_TIMEOUT, "T. bloqueo");
    OledUtils_RenderScreenLockTimeout_ValueOnly();
    Oled_DrawSaveFooter();
}

void OledUtils_RenderScreenLockTimeout_ValueOnly(void)
{
    char value[8];
    uint16_t timeoutSec = ScreenLock_GetTimeoutDraftSec();

    Oled_ClearSettingValue(&Font_11x18, 38u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), timeoutSec, "s", 38u);
}

void OledUtils_RenderSystemActionTimeout_Full(void)
{
    Oled_DrawSettingHeader(SCREEN_CODE_SETTINGS_PREFS_SYSTEM_ACTION_TIMEOUT, "T. accion");
    OledUtils_RenderSystemActionTimeout_ValueOnly();
    Oled_DrawSaveFooter();
}

void OledUtils_RenderSystemActionTimeout_ValueOnly(void)
{
    char value[8];
    uint16_t timeoutSec = Preferences_GetSystemActionTimeoutDraft();

    Oled_ClearSettingValue(&Font_11x18, 38u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), timeoutSec, "s", 38u);
}

void OledUtils_RenderUsbAlivePeriod_Full(void)
{
    Oled_DrawSettingHeader(SCREEN_CODE_SETTINGS_PREFS_SYSTEM_USB_ALIVE, "Alive USB");
    OledUtils_RenderUsbAlivePeriod_ValueOnly();
    Oled_DrawSaveFooter();
}

void OledUtils_RenderUsbAlivePeriod_ValueOnly(void)
{
    char value[12];

    Oled_ClearSettingValue(&Font_11x18, 38u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), Preferences_GetUsbAlivePeriodDraftMs(), " ms", 38u);
}

void OledUtils_RenderLineBlackWidth_Full(void)
{
    Oled_DrawSettingHeader(SCREEN_CODE_SETTINGS_PREFS_LINE_WIDTH, "Ancho cinta");
    OledUtils_RenderLineBlackWidth_ValueOnly();
    Oled_DrawSaveFooter();
}

void OledUtils_RenderLineBlackWidth_ValueOnly(void)
{
    char value[12];

    Oled_ClearSettingValue(&Font_11x18, 38u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), Preferences_GetLineBlackWidthDraftMm(), " mm", 38u);
}

AM_Result OledUtils_SetRotationAndRefresh(OledRotation_t rotation)
{
    AM_Result status = ssd1306_SetRotation(rotation);

    if (status == AM_OK) {
        OledUtils_Clear();
        autoMicroSystem.runtime.oled_first_draw = false;
        menuSystem.renderFlag = true;
    }

    return status;
}

void OledUtils_RenderOledRotationTest_Full(void)
{
    Oled_DrawSettingHeader(SCREEN_CODE_SENSORS_DISPLAY_ROTATION_TEST, "Orientacion");
    OledUtils_RenderOledRotationTest_ValueOnly();
    Oled_DrawSaveFooter();
}

void OledUtils_RenderOledRotationTest_ValueOnly(void)
{
    char value[12];

    Oled_ClearSettingValue(&Font_11x18, 38u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), Preferences_GetOledRotationDraftDeg(), " deg", 38u);
}

void OledUtils_RenderOledRotationMpuTest_Full(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SENSORS_DISPLAY_ROTATION_MPU_TEST,
                            SCREEN_REPORT_SOURCE_RENDER);
    Oled_DrawTitleLine("Auto MPU", 0u, SSD1306_WIDTH, 18u);
    OledUtils_RenderOledRotationMpuTest_ValueOnly();

    Oled_SetFont(&Font_7x10);
    Oled_DrawXBM(8u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(27u, 62u - Oled_FontHeight());
    Oled_DrawStr("Volver");
}

void OledUtils_RenderOledRotationMpuTest_ValueOnly(void)
{
    char value[12];
    char yawLine[12];
    size_t pos;

    Oled_ClearBox(0u, 18u, SSD1306_WIDTH, 31u);

    Oled_SetFont(&Font_11x18);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), Preferences_GetOledRotationMpuTestDeg(), " deg", 34u);

    Oled_SetFont(&Font_7x10);
    if (Preferences_IsOledRotationMpuTestReady()) {
        pos = Oled_AppendText(yawLine, sizeof(yawLine), 0u, "Z:");
        Oled_FormatSigned(value, sizeof(value), Preferences_GetOledRotationMpuTestYawDeg());
        pos = Oled_AppendText(yawLine, sizeof(yawLine), pos, value);
        Oled_Terminate(yawLine, sizeof(yawLine), pos);
        Oled_DrawCenteredLine(yawLine, 0u, SSD1306_WIDTH, 47u);
    } else {
        Oled_DrawCenteredLine("Sin MPU", 0u, SSD1306_WIDTH, 47u);
    }
}

void OledUtils_RenderMotorMinPwm_Full(void)
{
    OledUtils_SetScreenCode(Preferences_GetMotorMinPwmEditScreenCode(),
                            SCREEN_REPORT_SOURCE_RENDER);
    Oled_DrawTitleLine("PWM minimo", 0u, SSD1306_WIDTH, 18u);
    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(Preferences_GetMotorMinPwmEditLabel(), 0u, SSD1306_WIDTH, 31u);

    Oled_DrawXBM(5u, 34u, 4u, 7u, Arrow_Left_bits);
    Oled_DrawXBM(119u, 34u, 4u, 7u, Arrow_Right_bits);
    OledUtils_RenderMotorMinPwm_ValueOnly();

    Oled_DrawSaveFooter();
}

void OledUtils_RenderMotorMinPwm_ValueOnly(void)
{
    char value[12];

    Oled_ClearSettingValue(&Font_7x10, 45u);
    Oled_DrawUnsignedSettingValue(value, sizeof(value), Preferences_GetMotorMinPwmDraft(), " PWM", 45u);
}

void OledUtils_RenderSingleMotorTest_Full(void)
{
    OledUtils_SetScreenCode(MotorSingleTest_GetScreenCode(), SCREEN_REPORT_SOURCE_RENDER);
    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(MotorSingleTest_GetLabel(), 0u, SSD1306_WIDTH, 10u);

    // Encoder -> Dir (Left)
    Oled_DrawXBM(8u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(25u, 62u - Oled_FontHeight());
    Oled_DrawStr("Dir");
    
    Oled_DrawXBM(88u, 49u, 13u, 13u, Icon_UserBtn_bits);
    Oled_DrawXBM(105u, 50u, 13u, 13u, Icon_Volver_bits);

    OledUtils_RenderSingleMotorTest_ValueOnly();
}

void OledUtils_RenderSingleMotorTest_ValueOnly(void)
{
    char pct[8];
    char pwm[8];
    char line[64];
    size_t pos = 0u;
    const uint16_t pwmValue = MotorSingleTest_GetPwm();
    const uint16_t minPwm = MotorSingleTest_GetMinPwm();
    const uint16_t maxPwm = MotorSingleTest_GetMaxPwm();
    uint8_t fill = 0u;
    uint8_t minMarker = 6u;

    Oled_SetFont(&Font_7x10);
    Oled_ClearBox(0u, 12u, SSD1306_WIDTH, 35u);

    Oled_FormatPercent3(pct, sizeof(pct), MotorSingleTest_GetSpeedPercentDisplay());
    Oled_FormatUnsigned(pwm, sizeof(pwm), pwmValue);
    
    pos = Oled_AppendText(line, sizeof(line), pos, MotorSingleTest_GetDirectionLabel());
    pos = Oled_AppendText(line, sizeof(line), pos, " ");
    pos = Oled_AppendText(line, sizeof(line), pos, pct);
    pos = Oled_AppendText(line, sizeof(line), pos, " ");
    pos = Oled_AppendText(line, sizeof(line), pos, pwm);
    pos = Oled_AppendText(line, sizeof(line), pos, "PWM");
    Oled_Terminate(line, sizeof(line), pos);

    Oled_DrawCenteredLine(line, 0u, SSD1306_WIDTH, 24u);

    Oled_DrawFrame(5u, 31u, 118u, 12u);
    if (maxPwm > minPwm) {
        const uint16_t usableRange = (uint16_t)(maxPwm - minPwm);
        uint16_t pwmAboveMin = 0u;

        if (pwmValue > minPwm) {
            pwmAboveMin = (uint16_t)(pwmValue - minPwm);
        }
        if (pwmAboveMin > usableRange) {
            pwmAboveMin = usableRange;
        }

        fill = (uint8_t)(((uint32_t)pwmAboveMin * 116u) / usableRange);
    }
    if (fill > 0u) {
        Oled_DrawBox(6u, 32u, fill, 10u);
    }
    ssd1306_DrawLine(minMarker, 29, minMarker, 44);
}

static const char *OledUtils_BalanceRejectText(MotionControl_RejectReason_t reason)
{
    switch (reason) {
        case MOTION_REJECT_NONE:
            return "OK";
        case MOTION_REJECT_NOT_INITIALIZED:
            return "Sin init";
        case MOTION_REJECT_MPU_NOT_READY:
            return "MPU no listo";
        case MOTION_REJECT_MPU_CALIBRATING:
            return "Calibrando";
        case MOTION_REJECT_TILT_LIMIT:
            return "Angulo limite";
        case MOTION_REJECT_DISABLED:
            return "Deshabilitado";
        case MOTION_REJECT_DISARMED:
            return "Apagado";
        default:
            return "Estado ?";
    }
}

void OledUtils_RenderBalancePidStatus(void)
{
    const MotionControl_Status_t *motion = MotionControl_GetStatus(&motionControl);
    const BalanceControl_Status_t *pid = BalanceControl_GetStatus(&balanceControl);
    char a[12];
    char b[12];
    char line[24];
    size_t pos;
    bool isArmed = (motion != NULL && motion->armed);

    OledUtils_SetScreenCode(SCREEN_CODE_BALANCE_PID_STATUS,
                            SCREEN_REPORT_SOURCE_RENDER);

    ssd1306_Clear();
    ssd1306_SetColor(White);

    // Toggle row at the top (0 to 15)
    Oled_DrawXBM(0, 0, 16, 16, isArmed ? Icon_ChoiceBulletOn_bits : Icon_ChoiceBulletOff_bits);
    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(18, 12 - Oled_FontHeight());
    Oled_DrawStr(isArmed ? "Activado" : "Desactivado");

    // Title line
    Oled_DrawTitleLine("Estado PID", 0u, SSD1306_WIDTH, 27u);

    Oled_SetFont(&Font_7x10);
    Oled_ClearBox(0u, 29u, SSD1306_WIDTH, 35u);

    // Line 1: P / SP
    Oled_FormatFixed(a, sizeof(a), (motion != NULL) ? motion->pitch_measured_deg : 0.0f, 1u);
    Oled_FormatFixed(b, sizeof(b), (motion != NULL) ? motion->pitch_setpoint_deg : 0.0f, 1u);
    pos = 0u;
    pos = Oled_AppendText(line, sizeof(line), pos, "P:");
    pos = Oled_AppendText(line, sizeof(line), pos, a);
    pos = Oled_AppendText(line, sizeof(line), pos, " SP:");
    pos = Oled_AppendText(line, sizeof(line), pos, b);
    Oled_Terminate(line, sizeof(line), pos);
    ssd1306_SetCursor(0u, 39u);
    Oled_DrawStr(line);

    // Line 2: Out / E
    Oled_FormatFixed(a, sizeof(a), (motion != NULL) ? motion->balance_output_percent : 0.0f, 1u);
    Oled_FormatFixed(b, sizeof(b), (pid != NULL) ? pid->error_deg : 0.0f, 1u);
    pos = 0u;
    pos = Oled_AppendText(line, sizeof(line), pos, "Out:");
    pos = Oled_AppendText(line, sizeof(line), pos, a);
    pos = Oled_AppendText(line, sizeof(line), pos, " E:");
    pos = Oled_AppendText(line, sizeof(line), pos, b);
    Oled_Terminate(line, sizeof(line), pos);
    ssd1306_SetCursor(0u, 50u);
    Oled_DrawStr(line);

    // Line 3: Mot state
    pos = 0u;
    pos = Oled_AppendText(line, sizeof(line), pos, "Mot:");
    pos = Oled_AppendText(line, sizeof(line), pos,
                          (motion != NULL) ? OledUtils_BalanceRejectText(motion->reject_reason) : "Sin estado");
    Oled_Terminate(line, sizeof(line), pos);
    ssd1306_SetCursor(0u, 61u);
    Oled_DrawStr(line);
}

void OledUtils_RenderBalancePidLog_Full(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_BALANCE_PID_LOG,
                            SCREEN_REPORT_SOURCE_RENDER);
    Oled_DrawTitleLine("Log PID", 0u, SSD1306_WIDTH, 18u);

    Oled_DrawXBM(5u, 28u, 4u, 7u, Arrow_Left_bits);
    Oled_DrawXBM(119u, 28u, 4u, 7u, Arrow_Right_bits);
    OledUtils_RenderBalancePidLog_ValueOnly();

    Oled_SetFont(&Font_7x10);
    Oled_DrawXBM(87u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(104u, 62u - Oled_FontHeight());
    Oled_DrawStr("OK");
}

void OledUtils_RenderBalancePidLog_ValueOnly(void)
{
    char value[10];
    char memEst[10];
    char memAvail[10];
    char line[22];
    size_t pos;

    Oled_SetFont(&Font_11x18);
    Oled_ClearBox(10u, (uint8_t)(38u - Oled_FontHeight()), 108u, Oled_FontHeight());
    Oled_DrawUnsignedSettingValue(value, sizeof(value), BalancePid_GetLogDurationSec(), "s", 38u);

    Oled_SetFont(&Font_7x10);
    Oled_ClearBox(0u, 45u, 86u, 19u);
    Oled_FormatBytesCompact(memEst, sizeof(memEst), BalancePid_GetLogEstimatedBytes());
    Oled_FormatBytesCompact(memAvail, sizeof(memAvail), BalancePid_GetLogAvailableBytes());
    pos = 0u;
    pos = Oled_AppendText(line, sizeof(line), pos, memEst);
    pos = Oled_AppendText(line, sizeof(line), pos, "/");
    pos = Oled_AppendText(line, sizeof(line), pos, memAvail);
    Oled_Terminate(line, sizeof(line), pos);
    Oled_DrawCenteredLine(line, 0u, 86u, 56u);
}

void OledUtils_RenderBalancePidGain_Full(void)
{
    OledUtils_SetScreenCode(BalancePid_GetCurrentGainScreenCode(),
                            SCREEN_REPORT_SOURCE_RENDER);
    Oled_DrawTitleLine(BalancePid_GetCurrentGainLabel(), 0u, SSD1306_WIDTH, 18u);

    Oled_SetFont(&Font_7x10);
    Oled_DrawXBM(5u, 34u, 4u, 7u, Arrow_Left_bits);
    Oled_DrawXBM(119u, 34u, 4u, 7u, Arrow_Right_bits);
    OledUtils_RenderBalancePidGain_ValueOnly();

    Oled_SetFont(&Font_7x10);
    Oled_DrawXBM(12u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(31u, 62u - Oled_FontHeight());
    Oled_DrawStr("Aplicar");
}

void OledUtils_RenderBalancePidGain_ValueOnly(void)
{
    char value[12];

    Oled_SetFont(&Font_11x18);
    Oled_ClearBox(0u, (uint8_t)(45u - Oled_FontHeight()), SSD1306_WIDTH, Oled_FontHeight());
    Oled_FormatFixed(value, sizeof(value), BalancePid_GetCurrentGainDraft(), 2u);
    Oled_DrawCenteredLine(value, 0u, SSD1306_WIDTH, 45u);
}

static const char *Oled_UnerNodeLabel(uint8_t node)
{
    switch (node) {
        case UNER_NODE_MCU:
            return "MCU";
        case UNER_NODE_PC_QT:
            return "QT";
        case UNER_NODE_WEB_APP:
            return "WEB";
        case UNER_NODE_REMOTE_NRF:
            return "NRF";
        case UNER_NODE_BROADCAST:
            return "ALL";
        default:
            return "?";
    }
}

static const char *Oled_UnerTransportLabel(uint8_t transport)
{
    switch (transport) {
        case UNER_TR_UART1_ESP:
            return "UART";
        case UNER_TR_NRF24_SPI2:
            return "NRF";
        case UNER_TR_USB_CDC:
            return "USB";
        default:
            return "?";
    }
}

static const char *Oled_UnerTransportTitleLabel(uint8_t transport)
{
    switch (transport) {
        case UNER_TR_UART1_ESP:
            return (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_AT)
                ? "UART AT"
                : "UART ESP";
        case UNER_TR_NRF24_SPI2:
            return "NRF24";
        case UNER_TR_USB_CDC:
            return "USB CDC";
        default:
            return "?";
    }
}

static const char *Oled_UnerRouterStatusLabel(uint8_t status)
{
    switch (status) {
        case UNER_ROUTER_STATUS_RECEIVED:
            return "RX";
        case UNER_ROUTER_STATUS_FORWARDED:
            return "FWD";
        case UNER_ROUTER_STATUS_SENT:
            return "SENT";
        case UNER_ROUTER_STATUS_DISABLED:
            return "OFF";
        case UNER_ROUTER_STATUS_ROUTE_MISS:
            return "MISS";
        case UNER_ROUTER_STATUS_TX_BUSY:
            return "BUSY";
        case UNER_ROUTER_STATUS_SAME_TRANSPORT:
            return "SAME";
        case UNER_ROUTER_STATUS_INVALID:
            return "INV";
        default:
            return "NONE";
    }
}

static void Oled_UnerDrawLine(uint8_t y, const char *text)
{
    ssd1306_SetCursor(0u, y);
    Oled_DrawStrMax(text, 18u);
}

static void Oled_UnerFormatPayloadPreview(char *line, size_t len, const UNER_RouterSnapshot *snapshot)
{
    size_t pos = 0u;

    pos = Oled_AppendText(line, len, pos, "D:");
    if (snapshot == NULL || snapshot->len == 0u || snapshot->payload_preview_len == 0u) {
        pos = Oled_AppendChar(line, len, pos, '-');
    } else {
        for (uint8_t i = 0u; i < snapshot->payload_preview_len; ++i) {
            if (i > 0u) {
                pos = Oled_AppendChar(line, len, pos, ' ');
            }
            pos = Oled_AppendHexByteCompact(line, len, pos, snapshot->payload_preview[i]);
        }
        if (snapshot->payload_preview_len < snapshot->len) {
            pos = Oled_AppendText(line, len, pos, " +");
        }
    }
    Oled_Terminate(line, len, pos);
}

static ScreenCode_t Oled_UnerRouterScreenCode(uint8_t transport_id)
{
    switch (transport_id) {
        case UNER_TR_UART1_ESP:
            return SCREEN_CODE_SERVICE_UNER_ROUTER_UART;
        case UNER_TR_NRF24_SPI2:
            return SCREEN_CODE_SERVICE_UNER_ROUTER_NRF;
        case UNER_TR_USB_CDC:
        default:
            return SCREEN_CODE_SERVICE_UNER_ROUTER_USB;
    }
}

static void Oled_UnerDrawRouterTitle(uint8_t transport_id, bool tx_view)
{
    char title[18];
    size_t pos = 0u;

    pos = Oled_AppendText(title, sizeof(title), pos, Oled_UnerTransportTitleLabel(transport_id));
    pos = Oled_AppendChar(title, sizeof(title), pos, ' ');
    pos = Oled_AppendText(title, sizeof(title), pos, tx_view ? "TX" : "RX");
    Oled_Terminate(title, sizeof(title), pos);
    
    Oled_ClearBox(0u, 0u, SSD1306_WIDTH, 20u);
    
    if (transport_id == UNER_TR_NRF24_SPI2) {
        Oled_DrawTitleLine(title, 0u, 110u, 18u);
        Oled_DrawXBM(110u, 2u, 17u, 16u, Icon_RF_bits);
        ssd1306_DrawHorizontalLine(110u, 20u, SSD1306_WIDTH - 110u);
    } else {
        Oled_DrawTitleLine(title, 0u, SSD1306_WIDTH, 18u);
    }
}

void OledUtils_RenderUnerRouterTest_Full(void)
{
    const uint8_t transport_id = OledUtils_GetUnerRouterTransportId();

    OledUtils_SetScreenCode(Oled_UnerRouterScreenCode(transport_id),
                            SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_RenderUnerRouterTest_ValueOnly();
}

void OledUtils_RenderUnerRouterTest_ValueOnly(void)
{
    UNER_RouterSnapshot snapshot;
    char line[24];
    size_t pos;
    const bool tx_view = OledUtils_IsUnerRouterTxView();
    const uint8_t transport_id = OledUtils_GetUnerRouterTransportId();

    UNER_App_GetRouterPacketSnapshot(tx_view ? 1u : 0u, transport_id, &snapshot);

    Oled_UnerDrawRouterTitle(transport_id, tx_view);

    Oled_SetFont(&Font_7x10);
    Oled_ClearBox(0u, 20u, SSD1306_WIDTH, 44u);

    pos = 0u;
    pos = Oled_AppendText(line, sizeof(line), pos, tx_view ? "TX " : "RX ");
    pos = Oled_AppendText(line, sizeof(line), pos, Oled_UnerTransportLabel(transport_id));
    pos = Oled_AppendChar(line, sizeof(line), pos, ' ');
    pos = Oled_AppendText(line, sizeof(line), pos, "ST:");
    pos = Oled_AppendText(line, sizeof(line), pos,
                          Oled_UnerRouterStatusLabel(snapshot.status));
    Oled_Terminate(line, sizeof(line), pos);
    Oled_UnerDrawLine(21u, line);

    if (!snapshot.valid) {
        Oled_UnerDrawLine(31u, tx_view ? "Sin paquetes TX" : "Sin paquetes RX");
        Oled_UnerDrawLine(41u, "CMD:- LEN:0");
        Oled_UnerDrawLine(52u, "Esperando frame");
        return;
    } else {
        pos = 0u;
        pos = Oled_AppendText(line, sizeof(line), pos, Oled_UnerNodeLabel(snapshot.src));
        pos = Oled_AppendChar(line, sizeof(line), pos, '>');
        pos = Oled_AppendText(line, sizeof(line), pos, Oled_UnerNodeLabel(snapshot.dst));
        Oled_Terminate(line, sizeof(line), pos);
        Oled_UnerDrawLine(31u, line);

        pos = 0u;
        pos = Oled_AppendText(line, sizeof(line), pos, "CMD:");
        pos = Oled_AppendHex2(line, sizeof(line), pos, snapshot.cmd);
        pos = Oled_AppendText(line, sizeof(line), pos, " LEN:");
        pos = Oled_AppendUnsigned(line, sizeof(line), pos, snapshot.len);
        Oled_Terminate(line, sizeof(line), pos);
        Oled_UnerDrawLine(41u, line);
    }

    Oled_UnerFormatPayloadPreview(line, sizeof(line), &snapshot);
    Oled_UnerDrawLine(52u, line);
}

void OledUtils_RenderMPUCalibration_Full(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SENSORS_MPU_CALIBRATION,
                            SCREEN_REPORT_SOURCE_RENDER);
    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine("Calibrar MPU", 0u, SSD1306_WIDTH, 10u);
    ssd1306_DrawLine(0, 13, 127, 13);
    OledUtils_RenderMPUCalibration_ValueOnly();
}

void OledUtils_RenderMPUCalibration_ValueOnly(void)
{
    char pctText[8];
    size_t pos;
    const uint8_t pct = APP_MPU_GetCalibrationProgressPercent();
    uint8_t fill = (uint8_t)(((uint16_t)pct * 108u) / 100u);

    Oled_SetFont(&Font_7x10);
    Oled_ClearBox(0u, 15u, SSD1306_WIDTH, 49u);

    if (APP_MPU_IsCalibrationPromptActive()) {
        Oled_DrawCenteredLine("Deje quieto", 0u, SSD1306_WIDTH, 31u);
        Oled_DrawCenteredLine("horizontal", 0u, SSD1306_WIDTH, 45u);
        return;
    }

    if (APP_MPU_IsCalibrationRunning()) {
        Oled_DrawCenteredLine("Tomando muestras", 0u, SSD1306_WIDTH, 27u);
        Oled_DrawFrame(9u, 33u, 110u, 10u);
        if (fill > 108u) {
            fill = 108u;
        }
        if (fill > 0u) {
            Oled_DrawBox(10u, 34u, fill, 8u);
        }

        Oled_FormatUnsigned(pctText, sizeof(pctText), pct);
        pos = Oled_AppendText(pctText, sizeof(pctText), strlen(pctText), "%");
        Oled_Terminate(pctText, sizeof(pctText), pos);
        Oled_DrawCenteredLine(pctText, 0u, SSD1306_WIDTH, 57u);
        return;
    }

    if (APP_MPU_IsCalibrationDoneNoticeActive()) {
        Oled_DrawCenteredLine("Calibracion", 0u, SSD1306_WIDTH, 28u);
        Oled_DrawCenteredLine("terminada", 0u, SSD1306_WIDTH, 41u);
        return;
    }

    if (APP_MPU_GetCalibrationProgressPercent() >= 100u) {
        Oled_DrawCenteredLine("Calibracion OK", 0u, SSD1306_WIDTH, 28u);
    } else {
        Oled_DrawCenteredLine("MPU no listo", 0u, SSD1306_WIDTH, 28u);
    }

    Oled_DrawXBM(2u, 46u, 15u, 16u, Icon_UserBtn_bits);
    ssd1306_SetCursor(20u, 62u - Oled_FontHeight());
    Oled_DrawStr("Recalib");
    Oled_DrawXBM(78u, 48u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(94u, 62u - Oled_FontHeight());
    Oled_DrawStr("Volver");
}

void OledUtils_RenderScreenLocked(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_WARNING_LOCKED,
                            SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    Oled_DrawXBM(LOCK_ICON_X, LOCK_ICON_Y, LOCK_ICON_W, LOCK_ICON_H, Icon_Lock_bits);

    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine("Pantalla", 0u, SSD1306_WIDTH, 34u);
    Oled_DrawCenteredLine("bloqueada", 0u, SSD1306_WIDTH, 46u);
    Oled_DrawXBM(18u, 46u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(35u, 62u - Oled_FontHeight());
    Oled_DrawStr("Desbloquear");
}

/**
 * @brief Dibuja la escena completa del gráfico IR (leyenda + separador + barras vacías)
 *        Se llama solo una vez al entrar a la pantalla
 */
enum {
    OLED_IR_LINE_FRAME_X = 0u,
    OLED_IR_LINE_FRAME_Y = 12u,
    OLED_IR_LINE_FRAME_W = 49u,
    OLED_IR_OBJECT_FRAME_X = 51u,
    OLED_IR_OBJECT_FRAME_Y = 12u,
    OLED_IR_OBJECT_FRAME_W = 77u,
    OLED_IR_FRAME_H = 52u,
    OLED_IR_BAR_TOP_Y = 25u,
    OLED_IR_BAR_H = 36u,
    OLED_IR_LINE_BAR_W = 8u,
    OLED_IR_LINE_BAR_GAP = 4u,
    OLED_IR_OBJECT_BAR_W = 7u,
    OLED_IR_OBJECT_BAR_GAP = 5u,
};

static void OledUtils_DrawIRGroupFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *title)
{
    Oled_DrawFrame(x, y, w, h);
    Oled_DrawFrame((uint8_t)(x + 1u), (uint8_t)(y + 1u), (uint8_t)(w - 2u), (uint8_t)(h - 2u));

    ssd1306_SetColor(White);
    OLED_DrawMiniTextCentered(title, (uint8_t)(x + 2u), (uint8_t)(w - 4u), (uint8_t)(y + 3u));
    ssd1306_DrawLine((int16_t)(x + 2u), (int16_t)(y + 10u), (int16_t)(x + w - 3u), (int16_t)(y + 10u));
}

static void OledUtils_DrawIRBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t value)
{
    uint8_t barHeight;
    uint8_t barY;

    Oled_ClearBox(x, y, w, h);

    value = (value > TCRT5000_ADC_MAX_VALUE) ? TCRT5000_ADC_MAX_VALUE : value;
    barHeight = (uint8_t)(((uint32_t)value * h) / TCRT5000_ADC_MAX_VALUE);
    if (barHeight == 0u) {
        return;
    }

    barY = (uint8_t)(y + h - barHeight);
    Oled_DrawBox(x, barY, w, barHeight);
}

void OledUtils_RenderIRGraphScene(bool emittersOn)
{
    OledUtils_SetScreenCode(emittersOn ? SCREEN_CODE_SENSORS_IR_VALUES
                                       : SCREEN_CODE_SENSORS_IR_AMBIENT,
                            SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);
    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(emittersOn ? "IR encendidos" : "IR apagados",
                          0u, SSD1306_WIDTH, 10u);

    OledUtils_DrawIRGroupFrame(OLED_IR_LINE_FRAME_X,
                               OLED_IR_LINE_FRAME_Y,
                               OLED_IR_LINE_FRAME_W,
                               OLED_IR_FRAME_H,
                               "LINEAS");
    OledUtils_DrawIRGroupFrame(OLED_IR_OBJECT_FRAME_X,
                               OLED_IR_OBJECT_FRAME_Y,
                               OLED_IR_OBJECT_FRAME_W,
                               OLED_IR_FRAME_H,
                               "OBJETOS");
}

/**
 * @brief Actualiza solo las barras del gráfico IR según los valores del sensor
 * @param sensorData Puntero al array de 8 valores de sensores IR (0-4095)
 */
void OledUtils_UpdateIRBars(volatile uint16_t *sensorData)
{
    if (sensorData == NULL) {
        return;
    }

    for (uint8_t i = 0u; i < TCRT5000_LINE_COUNT; i++) {
        const TCRT5000_SensorDef_t *sensor = TCRT5000_GetLineSensorDef((TCRT5000_LineSensor_t)i);
        uint8_t x = (uint8_t)(OLED_IR_LINE_FRAME_X + 2u + OLED_IR_LINE_BAR_GAP +
                              (i * (OLED_IR_LINE_BAR_W + OLED_IR_LINE_BAR_GAP)));
        uint16_t value = TCRT5000_NormalizeForDisplay(TCRT5000_GetRaw(sensorData, sensor),
                                                      sensor ? sensor->polarity : TCRT5000_POLARITY_PULL_DOWN);
        OledUtils_DrawIRBar(x, OLED_IR_BAR_TOP_Y, OLED_IR_LINE_BAR_W, OLED_IR_BAR_H, value);
    }

    for (uint8_t i = 0u; i < TCRT5000_OBJECT_COUNT; i++) {
        const TCRT5000_SensorDef_t *sensor = TCRT5000_GetObjectSensorDef((TCRT5000_ObjectSensor_t)i);
        uint8_t x = (uint8_t)(OLED_IR_OBJECT_FRAME_X + 2u + OLED_IR_OBJECT_BAR_GAP +
                              (i * (OLED_IR_OBJECT_BAR_W + OLED_IR_OBJECT_BAR_GAP)));
        uint16_t value = TCRT5000_NormalizeForDisplay(TCRT5000_GetRaw(sensorData, sensor),
                                                      sensor ? sensor->polarity : TCRT5000_POLARITY_PULL_DOWN);
        OledUtils_DrawIRBar(x, OLED_IR_BAR_TOP_Y, OLED_IR_OBJECT_BAR_W, OLED_IR_BAR_H, value);
    }
}

static void OledUtils_FormatIRRawPair(char *buf,
                                      size_t len,
                                      const char *labelA,
                                      uint16_t valueA,
                                      const char *labelB,
                                      uint16_t valueB)
{
    size_t pos = 0u;

    if (!buf || len == 0u) {
        return;
    }

    pos = Oled_AppendText(buf, len, pos, labelA);
    pos = Oled_AppendUnsigned(buf, len, pos, valueA);
    pos = Oled_AppendChar(buf, len, pos, ' ');
    pos = Oled_AppendText(buf, len, pos, labelB);
    pos = Oled_AppendUnsigned(buf, len, pos, valueB);
    Oled_Terminate(buf, len, pos);
}

static bool Oled_SsidIsUseful(const char *ssid)
{
    bool hasVisibleCharacter = false;

    if (ssid == NULL || ssid[0] == '\0') {
        return false;
    }

    for (uint8_t i = 0u; i < WIFI_SSID_MAX_LEN && ssid[i] != '\0'; ++i) {
        const uint8_t ch = (uint8_t)ssid[i];

        if (ch < 0x20u || ch > 0x7Eu) {
            return false;
        }
        if (ch != (uint8_t)'?' && ch != (uint8_t)' ') {
            hasVisibleCharacter = true;
        }
    }

    return hasVisibleCharacter;
}

static const char *Oled_GetDashboardStaSsid(void)
{
    const WifiBridge_Credentials *credentials;

    if (WifiBridge_GetActiveBackend(&wifiBridge) != WIFI_BRIDGE_BACKEND_WEB) {
        return Oled_SsidIsUseful(autoMicroSystem.connectivity.espWifiConnection.staSsid)
            ? autoMicroSystem.connectivity.espWifiConnection.staSsid
            : "WiFi";
    }

    if (Oled_SsidIsUseful(wifiBridge.ssid)) {
        return wifiBridge.ssid;
    }

    credentials = WifiBridge_GetRuntimeCredentials();
    if (credentials != NULL &&
        credentials->flags.bits.ssid_set &&
        Oled_SsidIsUseful(credentials->ssid)) {
        return credentials->ssid;
    }

    return Oled_SsidIsUseful(autoMicroSystem.connectivity.espWifiConnection.staSsid)
        ? autoMicroSystem.connectivity.espWifiConnection.staSsid
        : "WiFi";
}

bool OledUtils_GetDashboardState(OledDashboardState *state)
{
    const volatile IPStruct_t *ip = NULL;
    const char *ssid = "";

    if (state == NULL) {
        return false;
    }

    memset(state, 0, sizeof(*state));
    state->espPresent = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
    state->staActive = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    state->apActive = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    state->usbActive = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE);
    state->rfActive = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, RF_ACTIVE);
    state->backend = (uint8_t)WifiBridge_GetActiveBackend(&wifiBridge);
    state->carMode = (uint8_t)GET_CAR_MODE();

    if (state->staActive) {
        ip = &autoMicroSystem.connectivity.espWifiConnection.staIp;
        ssid = Oled_GetDashboardStaSsid();
    } else if (state->apActive) {
        ip = &autoMicroSystem.connectivity.espWifiConnection.apIp;
        ssid = autoMicroSystem.connectivity.espWifiConnection.apSsid;
    }

    if (ip != NULL) {
        state->ip[0] = ip->block1;
        state->ip[1] = ip->block2;
        state->ip[2] = ip->block3;
        state->ip[3] = ip->block4;
    }
    Oled_CopyText(state->ssid, sizeof(state->ssid), ssid);
    return true;
}

void OledUtils_RenderIRRawScene(bool emittersOn)
{
    OledUtils_SetScreenCode(emittersOn ? SCREEN_CODE_SENSORS_IR_VALUES
                                       : SCREEN_CODE_SENSORS_IR_AMBIENT,
                            SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(emittersOn ? "IR raw" : "IR apagados", 0u, SSD1306_WIDTH, 10u);
    ssd1306_DrawLine(0, 13, 127, 13);
}

void OledUtils_UpdateIRRawValues(volatile uint16_t *sensorData)
{
    char line[22];
    const uint8_t rowY[4] = { 25u, 37u, 49u, 61u };

    if (sensorData == NULL) {
        return;
    }

    Oled_SetFont(&Font_7x10);
    Oled_ClearBox(0u, 15u, SSD1306_WIDTH, 49u);

    OledUtils_FormatIRRawPair(line, sizeof(line), "PA0:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA0),
                              "PA1:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA1));
    ssd1306_SetCursor(2u, (uint8_t)(rowY[0] - Oled_FontHeight()));
    Oled_DrawStr(line);

    OledUtils_FormatIRRawPair(line, sizeof(line), "PA2:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA2),
                              "PB1:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PB1));
    ssd1306_SetCursor(2u, (uint8_t)(rowY[1] - Oled_FontHeight()));
    Oled_DrawStr(line);

    OledUtils_FormatIRRawPair(line, sizeof(line), "PA4:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA4),
                              "PA5:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA5));
    ssd1306_SetCursor(2u, (uint8_t)(rowY[2] - Oled_FontHeight()));
    Oled_DrawStr(line);

    OledUtils_FormatIRRawPair(line, sizeof(line), "PA6:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA6),
                              "PA7:", TCRT5000_GetRawById(sensorData, TCRT5000_SENSOR_PA7));
    ssd1306_SetCursor(2u, (uint8_t)(rowY[3] - Oled_FontHeight()));
    Oled_DrawStr(line);
}

static bool OledUtils_AreRouteIrLightsOn(void)
{
    return Screens_Context->services->route_emitters_on();
}

static void Oled_FormatRouteValues(char *buf, size_t len, const char *labelA, uint16_t valueA, const char *labelB, uint16_t valueB)
{
    size_t pos = 0u;

    if (!buf || len == 0u) {
        return;
    }

    pos = Oled_AppendText(buf, len, pos, labelA);
    pos = Oled_AppendUnsigned(buf, len, pos, valueA);

    if (labelB != NULL) {
        pos = Oled_AppendChar(buf, len, pos, ' ');
        pos = Oled_AppendText(buf, len, pos, labelB);
        pos = Oled_AppendUnsigned(buf, len, pos, valueB);
    }

    Oled_Terminate(buf, len, pos);
}

static uint8_t OledUtils_RouteStrongestCenterX(const TCRT5000_RouteSample_t *route)
{
    if (!route) {
        return 64u;
    }

    if (route->leftBlack && !route->rightBlack && !route->centerBlack) {
        return 49u;
    }
    if (route->rightBlack && !route->leftBlack && !route->centerBlack) {
        return 79u;
    }
    if (route->leftBlack && !route->rightBlack) {
        return 53u;
    }
    if (route->rightBlack && !route->leftBlack) {
        return 75u;
    }
    if (route->centerBlack) {
        return 64u;
    }

    if (route->leftNorm > route->centerNorm && route->leftNorm > route->rightNorm) {
        return 49u;
    }
    if (route->rightNorm > route->centerNorm && route->rightNorm > route->leftNorm) {
        return 79u;
    }

    return 64u;
}

static void OledUtils_DrawRouteSensorBox(uint8_t x, bool active, const char *label)
{
    const uint8_t y = 33u;

    Oled_ClearBox(x, y, 8u, 7u);
    if (active) {
        Oled_DrawBox(x, y, 8u, 7u);
    } else {
        Oled_DrawFrame(x, y, 8u, 7u);
    }

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor((uint8_t)(x + 1u), 42u);
    Oled_DrawStrMax(label, 1u);
}

static void OledUtils_DrawRouteStrip(uint8_t centerX, uint8_t width, bool found)
{
    const uint8_t y = 40u;
    const uint8_t h = 11u;
    const uint8_t x = (centerX > (width / 2u)) ? (uint8_t)(centerX - (width / 2u)) : 0u;

    if (found) {
        Oled_DrawBox(x, y, width, h);
        return;
    }

    for (uint8_t dashY = y; dashY < (uint8_t)(y + h); dashY = (uint8_t)(dashY + 4u)) {
        ssd1306_DrawLine((int16_t)centerX, dashY, (int16_t)centerX, (int16_t)(dashY + 1u));
    }
}

static void OledUtils_DrawRouteAlignmentVisual(const TCRT5000_RouteSample_t *route)
{
    const bool found = route && (route->leftBlack || route->centerBlack || route->rightBlack);
    const bool wide = route && route->leftBlack && route->rightBlack;
    const uint8_t stripCenterX = OledUtils_RouteStrongestCenterX(route);
    const uint8_t stripWidth = wide ? 40u : (uint8_t)(route ? route->lineWidthMm : 12u);

    Oled_DrawFrame(34u, 14u, 60u, 39u);
    OledUtils_DrawRouteStrip(stripCenterX, stripWidth, found);

    Oled_DrawXBM((uint8_t)((SSD1306_WIDTH - ICON_CAR_TOP_W) / 2u),
                 18u,
                 ICON_CAR_TOP_W,
                 ICON_CAR_TOP_H,
                 Icon_CarTop_bits);

    OledUtils_DrawRouteSensorBox(45u, route && route->leftBlack, "L");
    OledUtils_DrawRouteSensorBox(60u, route && route->centerBlack, "C");
    OledUtils_DrawRouteSensorBox(75u, route && route->rightBlack, "R");
}

void OledUtils_RenderRouteAlignmentVisualScene(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SENSORS_ROUTE_ALIGNMENT, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);
    Oled_SetFont(&Font_7x10);

    Oled_DrawCenteredLine("Alineacion ruta", 0u, SSD1306_WIDTH, 10u);
    ssd1306_DrawLine(0, 12, 127, 12);
}

void OledUtils_UpdateRouteAlignmentVisual(volatile uint16_t *sensorData)
{
    if (!TCRT5000_EmittersGroup3IsOn(&irSensors)) {
        Oled_ClearBox(0u, 13u, SSD1306_WIDTH, 51u);
        Oled_SetFont(&Font_7x10);
        Oled_DrawCenteredLine("IR linea apagados", 0u, SSD1306_WIDTH, 37u);
        Oled_DrawCenteredLine("Sin interpretar", 0u, SSD1306_WIDTH, 52u);
        return;
    }

    TCRT5000_RouteSample_t route;

    if (!APP_EvaluateRoute(&route)) {
        Oled_ClearBox(0u,13u,SSD1306_WIDTH,51u);
        Oled_SetFont(&Font_7x10);
        Oled_DrawCenteredLine("Calibracion pendiente",0u,SSD1306_WIDTH,37u);
        Oled_DrawCenteredLine("Blanco y pista",0u,SSD1306_WIDTH,52u);
        return;
    }

    Oled_ClearBox(0u, 13u, SSD1306_WIDTH, 51u);
    OledUtils_DrawRouteAlignmentVisual(&route);
    Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(TCRT5000_GetRouteAlignmentText(route.alignment), 0u, SSD1306_WIDTH, 63u);
}

void OledUtils_RenderRouteAlignmentScene(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SENSORS_ROUTE_ALIGNMENT, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);
    Oled_SetFont(&Font_7x10);

    Oled_DrawCenteredLine("Alineacion de ruta", 0u, SSD1306_WIDTH, 11u);
    ssd1306_DrawLine(0, 14, 127, 14);
    Oled_DrawFrame(0u, 16u, SSD1306_WIDTH, 48u);
}

void OledUtils_UpdateRouteAlignment(volatile uint16_t *sensorData)
{
    char line[22];
    const bool irLightsOn = OledUtils_AreRouteIrLightsOn();
    TCRT5000_RouteSample_t route;

    if (!TCRT5000_EmittersGroup3IsOn(&irSensors)) {
        Oled_SetFont(&Font_7x10);
        Oled_ClearBox(2u, 18u, 124u, 44u);
        Oled_DrawCenteredLine("IR linea apagados", 2u, 124u, 37u);
        Oled_DrawCenteredLine("Sin interpretar", 2u, 124u, 52u);
        return;
    }

    if (!APP_EvaluateRoute(&route)) {
        Oled_ClearBox(0u,13u,SSD1306_WIDTH,51u);
        Oled_SetFont(&Font_7x10);
        Oled_DrawCenteredLine("Calibracion pendiente",0u,SSD1306_WIDTH,37u);
        Oled_DrawCenteredLine("Blanco y pista",0u,SSD1306_WIDTH,52u);
        return;
    }

    Oled_SetFont(&Font_7x10);
    const uint8_t fh = Oled_FontHeight();
    Oled_ClearBox(2u, 18u, 124u, 44u);

    Oled_FormatRouteValues(line, sizeof(line), "Izq:", route.leftRaw, "Cen:", route.centerRaw);
    ssd1306_SetCursor(4u, (uint8_t)(29u - fh));
    Oled_DrawStr(line);

    Oled_FormatRouteValues(line, sizeof(line), "Der:", route.rightRaw, NULL, 0u);
    ssd1306_SetCursor(4u, (uint8_t)(41u - fh));
    Oled_DrawStr(line);
    ssd1306_SetCursor(78u, (uint8_t)(41u - fh));
    Oled_DrawStr(irLightsOn ? "IR ON" : "IR OFF");

    ssd1306_DrawLine(2, 46, 125, 46);
    Oled_DrawCenteredLine(TCRT5000_GetRouteAlignmentText(route.alignment), 2u, 124u, 60u);
}
/**
 * @brief Dibuja la escena completa del MPU (titulo + etiquetas)
 *        Se llama solo una vez al entrar a la pantalla
 */
void OledUtils_RenderMPUScene(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SENSORS_MPU_VALUES, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    // Título "Sensor MPU" con fuente grande
    Oled_DrawTitleLine("Sensor MPU", 0u, SSD1306_WIDTH, 18u);

    // Etiquetas de acelerómetro y giroscopio con fuente pequeña
    Oled_SetFont(&Font_7x10);
    const uint8_t fh_pequena = Oled_FontHeight();

    // Columna izquierda: AX, AY, AZ
    ssd1306_SetCursor(5, 32 - fh_pequena);
    Oled_DrawStr("AX");

    ssd1306_SetCursor(5, 42 - fh_pequena);
    Oled_DrawStr("AY");

    ssd1306_SetCursor(5, 52 - fh_pequena);
    Oled_DrawStr("AZ");

    // Columna derecha: GX, GY, GZ
    ssd1306_SetCursor(68, 33 - fh_pequena);
    Oled_DrawStr("GX");

    ssd1306_SetCursor(68, 43 - fh_pequena);
    Oled_DrawStr("GY");

    ssd1306_SetCursor(68, 53 - fh_pequena);
    Oled_DrawStr("GZ");
}

/**
 * @brief Actualiza solo los valores del MPU en pantalla
 * @param mpu Puntero a la estructura del MPU con los datos
 */
void OledUtils_UpdateMPUValues(MPU9250_Handle_t *mpu)
{
    char buf[8];
    const char *statusText = "Sin MPU";
    ssd1306_SetColor(White);

    Oled_SetFont(&Font_7x10);
    const uint8_t fh = Oled_FontHeight();
    const uint8_t fw = Oled_FontWidth();
    const uint8_t valueWidth = fw * 6;  // Ancho aproximado para "-32768"

    // Posiciones X para los valores
    const uint8_t axValueX = 24;
    const uint8_t gxValueX = 88;

    if (mpu == NULL || mpu->bus == NULL || mpu->bus == NULL) {
        Oled_ClearBox(2u, 54u, 124u, fh);
        Oled_DrawCenteredLine(statusText, 2u, 124u, 63u);
        return;
    }

    if (IS_FLAG_SET(mpu->flags, MPU_ERROR)) {
        statusText = "MPU error";
    } else if (IS_FLAG_SET(mpu->flags, MPU_CALIBRATING)) {
        statusText = "Calibrando";
    } else if (IS_FLAG_SET(mpu->flags, MPU_VALID)) {
        statusText = "MPU OK";
    } else {
        statusText = "Iniciando";
    }

    // Limpiar y actualizar AX
    Oled_ClearBox(axValueX, 32 - fh, valueWidth, fh);
    ssd1306_SetCursor(axValueX, 32 - fh);
    Oled_FormatSigned(buf, sizeof(buf), mpu->data.accel_x_mg);
    Oled_DrawStr(buf);

    // Limpiar y actualizar AY
    Oled_ClearBox(axValueX, 42 - fh, valueWidth, fh);
    ssd1306_SetCursor(axValueX, 42 - fh);
    Oled_FormatSigned(buf, sizeof(buf), mpu->data.accel_y_mg);
    Oled_DrawStr(buf);

    // Limpiar y actualizar AZ
    Oled_ClearBox(axValueX, 52 - fh, valueWidth, fh);
    ssd1306_SetCursor(axValueX, 52 - fh);
    Oled_FormatSigned(buf, sizeof(buf), mpu->data.accel_z_mg);
    Oled_DrawStr(buf);

    // Limpiar y actualizar GX
    Oled_ClearBox(gxValueX, 33 - fh, valueWidth, fh);
    ssd1306_SetCursor(gxValueX, 33 - fh);
    Oled_FormatSigned(buf, sizeof(buf), mpu->data.gyro_x_mdps);
    Oled_DrawStr(buf);

    // Limpiar y actualizar GY
    Oled_ClearBox(gxValueX, 43 - fh, valueWidth, fh);
    ssd1306_SetCursor(gxValueX, 43 - fh);
    Oled_FormatSigned(buf, sizeof(buf), mpu->data.gyro_y_mdps);
    Oled_DrawStr(buf);

    // Limpiar y actualizar GZ
    Oled_ClearBox(gxValueX, 53 - fh, valueWidth, fh);
    ssd1306_SetCursor(gxValueX, 53 - fh);
    Oled_FormatSigned(buf, sizeof(buf), mpu->data.gyro_z_mdps);
    Oled_DrawStr(buf);

    Oled_ClearBox(2u, 54u, 124u, fh);
    Oled_DrawCenteredLine(statusText, 2u, 124u, 63u);
}

void OLED_DrawRotatedLine(int cx, int cy, int length, float angle_deg)
{
    const float angle_rad = angle_deg * OLED_MPU_RAD_PER_DEG;
    const float half_len = (float)length * 0.5f;
    const float c = cosf(angle_rad);
    const float s = sinf(angle_rad);
    const int16_t x0 = OLED_MPU_Round((float)cx - (c * half_len));
    const int16_t y0 = OLED_MPU_Round((float)cy - (s * half_len));
    const int16_t x1 = OLED_MPU_Round((float)cx + (c * half_len));
    const int16_t y1 = OLED_MPU_Round((float)cy + (s * half_len));

    ssd1306_SetColor(White);
    ssd1306_DrawLine(x0, y0, x1, y1);
}

static bool OLED_GetMPUAxisBitmap(const char *axis,
                                  const uint8_t **bits,
                                  uint8_t *w,
                                  uint8_t *h)
{
    if (!axis || !bits || !w || !h) {
        return false;
    }

    switch (axis[0]) {
        case 'Z':
            *bits = Icon_CarTop_bits;
            *w = ICON_CAR_TOP_W;
            *h = ICON_CAR_TOP_H;
            return true;
        case 'X':
            *bits = Icon_CarSide_bits;
            *w = ICON_CAR_SIDE_W;
            *h = ICON_CAR_SIDE_H;
            return true;
        case 'Y':
            *bits = Icon_CarFront_bits;
            *w = ICON_CAR_FRONT_W;
            *h = ICON_CAR_FRONT_H;
            return true;
        default:
            return false;
    }
}

void OLED_DrawAnglePanel(uint8_t x,
                         uint8_t y,
                         uint8_t w,
                         uint8_t h,
                         const char *title,
                         const char *axis,
                         float angle_deg)
{
    char valueText[9];
    const bool isZ = (axis != NULL && axis[0] == 'Z');
    const float textAngle = isZ
        ? OLED_MPU_Normalize180(angle_deg)
        : angle_deg;
    const float visualAngle = isZ
        ? textAngle
        : OLED_MPU_Clamp(textAngle, -60.0f, 60.0f);
    const int cx = (int)x + ((int)w / 2);
    const int cy = (int)y + 32;
    const uint8_t valueY = (uint8_t)(y + h - 9u);
    const uint8_t *axisBitmap = NULL;
    uint8_t axisBitmapW = 0u;
    uint8_t axisBitmapH = 0u;
    uint8_t valueWidth;
    uint8_t valueX;

    if (w < 4u || h < 12u) {
        return;
    }

    Oled_ClearBox((uint8_t)(x + 1u), (uint8_t)(y + 1u), (uint8_t)(w - 2u), (uint8_t)(h - 2u));
    Oled_DrawFrame(x, y, w, h);

    ssd1306_SetColor(White);
    OLED_DrawMiniTextCentered(title, (uint8_t)(x + 1u), (uint8_t)(w - 2u), (uint8_t)(y + 3u));

    if (OLED_GetMPUAxisBitmap(axis, &axisBitmap, &axisBitmapW, &axisBitmapH)) {
        OLED_DrawRotatedBitmap(cx, cy, axisBitmapW, axisBitmapH, axisBitmap, visualAngle);
    } else {
        const uint8_t lineLength = (uint8_t)(((uint16_t)(w - 4u) * 65u) / 100u);
        OLED_DrawRotatedLine(cx, cy, lineLength, visualAngle);
    }

    OLED_DrawPixelSafe((int16_t)cx, (int16_t)cy);
    OLED_DrawPixelSafe((int16_t)(cx + 1), (int16_t)cy);
    OLED_DrawPixelSafe((int16_t)cx, (int16_t)(cy + 1));
    OLED_DrawPixelSafe((int16_t)(cx + 1), (int16_t)(cy + 1));

    OLED_FormatPanelAngle(valueText, sizeof(valueText), axis, textAngle);
    valueWidth = (uint8_t)(OLED_MiniTextWidth(valueText) + 3u);
    valueX = (valueWidth >= w) ? x : (uint8_t)(x + ((w - valueWidth) / 2u));
    OLED_DrawMiniText(valueX, valueY, valueText);
    OLED_DrawMiniDegree((uint8_t)(valueX + OLED_MiniTextWidth(valueText) + 1u), valueY);
}

void OLED_DrawMPUScreen(float angle_z, float angle_front, float angle_lateral)
{
    OLED_DrawAnglePanel(OLED_MPU_PANEL0_X,
                        OLED_MPU_PANEL_Y,
                        OLED_MPU_PANEL_W,
                        OLED_MPU_PANEL_H,
                        "ARRIBA",
                        "Z",
                        angle_z);
    OLED_DrawAnglePanel(OLED_MPU_PANEL1_X,
                        OLED_MPU_PANEL_Y,
                        OLED_MPU_PANEL_W,
                        OLED_MPU_PANEL_H,
                        "FRENTE",
                        "Y",
                        angle_front);
    OLED_DrawAnglePanel(OLED_MPU_PANEL2_X,
                        OLED_MPU_PANEL_Y,
                        OLED_MPU_PANEL_W,
                        OLED_MPU_PANEL_H,
                        "LATERAL",
                        "X",
                        angle_lateral);
}

void OledUtils_RenderMPUOrientationScene(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_SENSORS_MPU_VALUES, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);
}

void OledUtils_UpdateMPUOrientation(MPU9250_Handle_t *mpu)
{
    MPU9250_Snapshot_t snapshot;
    float angleZ = 0.0f;
    float angleFront = 0.0f;
    float angleLateral = 0.0f;

    if (mpu != NULL &&
        mpu->bus != NULL &&
        mpu->bus != NULL &&
        MPU9250_GetLatest(mpu, &snapshot) == AM_OK) {
        angleZ = (float)snapshot.yaw_md / 1000.0f;
        angleFront = (float)snapshot.pitch_md / 1000.0f;
        angleLateral = (float)snapshot.roll_md / 1000.0f;
    }

    OLED_DrawMPUScreen(angleZ, angleFront, angleLateral);
}

static uint8_t Oled_TextPixelWidth(const char *text)
{
    size_t len;
    uint16_t width;

    if (!text) {
        return 0u;
    }

    len = strlen(text);
    width = (uint16_t)len * (uint16_t)Oled_FontWidth();
    return width > 255u ? 255u : (uint8_t)width;
}

static uint8_t Oled_CenterTextX(const char *text, uint8_t areaX, uint8_t areaW)
{
    uint8_t textWidth = Oled_TextPixelWidth(text);

    if (textWidth >= areaW) {
        return areaX;
    }

    return (uint8_t)(areaX + ((areaW - textWidth) / 2u));
}

static void Oled_DrawCenteredLine(const char *text, uint8_t areaX, uint8_t areaW, uint8_t baselineY)
{
    ssd1306_SetCursor(Oled_CenterTextX(text, areaX, areaW),
                      (uint8_t)(baselineY - Oled_FontHeight()));
    Oled_DrawStr(text);
}

static void Oled_DrawTitleLine(const char *text, uint8_t areaX, uint8_t areaW, uint8_t baselineY)
{
    Oled_SetFont(&Font_11x18);
    if (Oled_TextPixelWidth(text) > areaW) {
        Oled_SetFont(&Font_7x10);
    }

    Oled_DrawCenteredLine(text, areaX, areaW, baselineY);
}

/**
 * @brief Dibuja la escena completa de búsqueda WiFi con temporizador (una vez)
 */
void OledUtils_RenderWiFiSearchScene(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_SetColor(White);

    // Texto "Buscando" con fuente grande
    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(14u, 20u - Oled_FontHeight());
    Oled_DrawStr("Buscando");

    // Texto "redes wifi"
    ssd1306_SetCursor(14u, 40u - Oled_FontHeight());
    Oled_DrawStr("redes wifi");

    // Texto "Cancelar" con fuente pequeña
    Oled_SetFont(&Font_7x10);
    Oled_DrawXBM(49u, 49u, 13u, 13u, Icon_Encoder_bits);
    ssd1306_SetCursor(66u, 62u - Oled_FontHeight());
    Oled_DrawStr("Cancelar");

    // Botón OK/Encoder (13x13 píxeles)
    // Icono WiFi (19x16 píxeles)
    Oled_DrawXBM(107u, 2u, 19u, 16u, Icon_Wifi_100_bits);
}

/**
 * @brief Actualiza solo el temporizador de búsqueda WiFi
 * @param secondsRemaining Segundos restantes de búsqueda
 */
void OledUtils_UpdateWiFiSearchTimer(uint8_t secondsRemaining)
{
    Oled_SetFont(&OLED_LARGE_FONT);
    const uint8_t fh = Oled_FontHeight();
    const uint8_t fw = Oled_FontWidth();

    // Limpiar zona del temporizador (ancho suficiente para "99")
    const uint8_t timerWidth = fw * 2;
    Oled_ClearBox(4, 62 - fh, timerWidth, fh);

    // Dibujar nuevo valor
    char timeStr[4];
    Oled_FormatUnsigned(timeStr, sizeof(timeStr), secondsRemaining);
    ssd1306_SetCursor(4, 62 - fh);
    Oled_DrawStr(timeStr);
}

void OledUtils_ShowWifiResults(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(2, 48, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(14, 30 - Oled_FontHeight());
    Oled_DrawStr("Resultados");
    ssd1306_SetCursor(36, 50 - Oled_FontHeight());
    Oled_DrawStr("WiFi");
}

/**
 * @brief Pantalla de WiFi no conectado
 */
void OledUtils_RenderWiFiNotConnected(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_NOT_CONNECTED, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_SetColor(White);

    // Texto "No hay red" con fuente grande
    Oled_SetFont(&Font_11x18);
    const uint8_t fh = Oled_FontHeight();
    ssd1306_SetCursor(7, 40 - fh);
    Oled_DrawStr("No hay red");

    // Icono WiFi desconectado (19x16 píxeles)
    Oled_DrawXBM(54, 3, 19, 16, Icon_Wifi_NotConnected_bits);

    // Texto "conectada"
    ssd1306_SetCursor(16, 55 - fh);
    Oled_DrawStr("conectada");
}

void OledUtils_RenderWiFiDetails(const char *ssid,
                                 const char *encryption,
                                 int8_t signal_strength,
                                 uint8_t channel,
                                 uint8_t detail_valid)
{
    const char *ssid_text = (ssid && ssid[0] != '\0') ? ssid : "[SSID RED]";
    char signal_text[18];
    char meta_text[18];

    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_DETAILS, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(3, 2, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(25, 13 - Oled_FontHeight());
    Oled_DrawStr("Detalles");

    Oled_FormatWiFiSignal(signal_text, sizeof(signal_text), signal_strength, detail_valid);
    Oled_FormatWiFiMeta(meta_text, sizeof(meta_text), encryption, channel, detail_valid);

    ssd1306_SetCursor(2, 27 - Oled_FontHeight());
    Oled_DrawStrMax(ssid_text, 17u);

    ssd1306_SetCursor(2, 39 - Oled_FontHeight());
    Oled_DrawStrMax(signal_text, 17u);

    ssd1306_SetCursor(2, 51 - Oled_FontHeight());
    Oled_DrawStrMax(meta_text, 17u);

    Oled_DrawXBM(2, 48, 15, 16, Icon_UserBtn_bits);
    Oled_DrawXBM(20, 53, 4, 7, Arrow_Left_bits);

    ssd1306_SetCursor(53, 59 - Oled_FontHeight());
    Oled_DrawStr("Conectar");
    Oled_DrawXBM(112, 49, 13, 13, Icon_Encoder_bits);
}

void OledUtils_RenderWiFiAtPassword(const char *ssid,
                                    uint8_t password_length,
                                    char selected_character)
{
    char mask[12];
    char edit_line[18];
    uint8_t mask_length = password_length;

    if (mask_length > (sizeof(mask) - 1u)) {
        mask_length = (uint8_t)(sizeof(mask) - 1u);
    }
    memset(mask, '*', mask_length);
    mask[mask_length] = '\0';
    if (selected_character == ' ') {
        selected_character = '_';
    }

    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_AT,
                            SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_Clear();
    ssd1306_SetColor(White);
    Oled_DrawXBM(3, 2, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(27, 13 - Oled_FontHeight());
    Oled_DrawStr("Clave WiFi");

    ssd1306_SetCursor(2, 27 - Oled_FontHeight());
    Oled_DrawStrMax((ssid != NULL) ? ssid : "WiFi", 17u);

    ssd1306_SetCursor(2, 39 - Oled_FontHeight());
    Oled_DrawStr("Clave:");
    Oled_DrawStrMax(mask, 11u);

    (void)snprintf(edit_line, sizeof(edit_line), "Char:%c Largo:%u",
                   selected_character,
                   (unsigned int)password_length);
    ssd1306_SetCursor(2, 51 - Oled_FontHeight());
    Oled_DrawStrMax(edit_line, 17u);

    ssd1306_SetCursor(2, 63 - Oled_FontHeight());
    Oled_DrawStr("OK:+ U:- Hold:ir");
}

void OledUtils_RenderWiFiCredentialsWebNotification(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_WEB,
                            SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(4, 4, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(28, 18 - Oled_FontHeight());
    Oled_DrawStr("Ingresar");

    ssd1306_SetCursor(8, 36 - Oled_FontHeight());
    Oled_DrawStr("credenciales");

    ssd1306_SetCursor(42, 54 - Oled_FontHeight());
    Oled_DrawStr("en web");
}

void OledUtils_SetWiFiCredentialsWebResultStatus(uint8_t status)
{
    oled_wifi_credentials_result_status = status;
}

void OledUtils_RenderWiFiCredentialsSucceededNotification(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_SUCCEEDED,
                            SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(54, 4, 19, 16, Icon_Wifi_100_bits);
    Oled_DrawXBM(57, 24, 14, 16, Icon_Checked_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(24, 49 - Oled_FontHeight());
    Oled_DrawStr("WiFi conectado");
}

void OledUtils_RenderWiFiCredentialsFailedNotification(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_FAILED,
                            SCREEN_REPORT_SOURCE_NOTIFICATION);
    const char *line1 = "Conexion";
    const char *line2 = "fallida";
    uint8_t line1_x = 38u;
    uint8_t line2_x = 42u;

    if (oled_wifi_credentials_result_status == WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_TIMEOUT) {
        line1 = "Tiempo";
        line2 = "agotado";
        line1_x = 43u;
        line2_x = 39u;
    } else if (oled_wifi_credentials_result_status == WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_CANCELED) {
        line1 = "Cancelada";
        line2 = "desde web";
        line1_x = 32u;
        line2_x = 32u;
    }

    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(54, 4, 19, 16, Icon_Wifi_NotConnected_bits);
    Oled_DrawXBM(58, 25, 11, 16, Icon_Crossed_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(line1_x, 48 - Oled_FontHeight());
    Oled_DrawStr(line1);
    ssd1306_SetCursor(line2_x, 61 - Oled_FontHeight());
    Oled_DrawStr(line2);
}

void OledUtils_SetWiFiConnectedSsid(const char *ssid)
{
    if (!ssid || ssid[0] == '\0') {
        strncpy(oled_wifi_connected_ssid, "WiFi", sizeof(oled_wifi_connected_ssid));
    } else {
        strncpy(oled_wifi_connected_ssid, ssid, sizeof(oled_wifi_connected_ssid) - 1u);
        oled_wifi_connected_ssid[sizeof(oled_wifi_connected_ssid) - 1u] = '\0';
    }

    strncpy(autoMicroSystem.connectivity.espWifiConnection.staSsid,
            oled_wifi_connected_ssid,
            sizeof(autoMicroSystem.connectivity.espWifiConnection.staSsid) - 1u);
    autoMicroSystem.connectivity.espWifiConnection.staSsid[sizeof(autoMicroSystem.connectivity.espWifiConnection.staSsid) - 1u] = '\0';
}

static void OledUtils_RenderWiFiConnecting(const char *ssid)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_CONNECTING, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(53, 14, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(24, 42 - Oled_FontHeight());
    Oled_DrawStr("Conectando a");

    ssd1306_SetCursor(24, 54 - Oled_FontHeight());
    Oled_DrawStrMax(ssid, 14u);
}

static void OledUtils_RenderWiFiConnected(const char *ssid, const char *ipAddress)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_CONNECTED, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(53, 6, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(24, 31 - Oled_FontHeight());
    Oled_DrawStr("Conectado a");

    ssd1306_SetCursor(24, 43 - Oled_FontHeight());
    Oled_DrawStrMax(ssid, 14u);

    ssd1306_SetCursor(16, 59 - Oled_FontHeight());
    Oled_DrawStr("IP:");

    ssd1306_SetCursor(34, 59 - Oled_FontHeight());
    Oled_DrawStrMax(ipAddress, 15u);
}

void OledUtils_RenderESPWifiConnectingNotification(void)
{
    OledUtils_RenderWiFiConnecting(oled_wifi_connected_ssid);
}

void OledUtils_RenderWiFiStatus(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_WIFI_STATUS, SCREEN_REPORT_SOURCE_RENDER);
    char ipStr[20];
    const bool isWebBackend =
        WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_WEB;
    const bool apActive = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    const bool wifiActive = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    const bool showApInfo = apActive;
    const bool showStationInfo = !apActive && wifiActive;

    ssd1306_Clear();
    ssd1306_SetColor(White);

    if (showApInfo) {
        if (isWebBackend && !autoMicroSystem.connectivity.espWifiConnection.apInfoValid) {
            Oled_DrawXBM(54, 4, 15, 16, Icon_APWifi_bits);
            Oled_SetFont(&Font_7x10);
            Oled_DrawCenteredLine("Consultando AP", 0u, SSD1306_WIDTH, 39u);
            Oled_DrawCenteredLine("al ESP...", 0u, SSD1306_WIDTH, 54u);
            return;
        }
        const char *ssid = autoMicroSystem.connectivity.espWifiConnection.apSsid[0] != '\0'
            ? autoMicroSystem.connectivity.espWifiConnection.apSsid
            : "AP sin nombre";
        const char *pass = autoMicroSystem.connectivity.espWifiConnection.apPassword[0] != '\0'
            ? autoMicroSystem.connectivity.espWifiConnection.apPassword
            : "Pass sin dato";
        char ssidLine[17];
        char passLine[17];

        Oled_FormatIp(&autoMicroSystem.connectivity.espWifiConnection.apIp, ipStr, sizeof(ipStr));
        (void)snprintf(ssidLine, sizeof(ssidLine), "AP:%.13s", ssid);
        (void)snprintf(passLine, sizeof(passLine), "C:%u P:%.8s",
                       (unsigned int)autoMicroSystem.connectivity.espWifiConnection.apClients, pass);

        Oled_DrawXBM(54, 4, 15, 16, Icon_APWifi_bits);
        Oled_SetFont(&Font_7x10);
        Oled_DrawCenteredLine(ssidLine, 0u, SSD1306_WIDTH, 30u);
        Oled_DrawCenteredLine(ipStr, 0u, SSD1306_WIDTH, 45u);
        Oled_DrawCenteredLine(passLine, 0u, SSD1306_WIDTH, 60u);
    } else if (showStationInfo) {
        Oled_FormatIp(&autoMicroSystem.connectivity.espWifiConnection.staIp, ipStr, sizeof(ipStr));

        Oled_DrawXBM(54, 4, 19, 16, Icon_Wifi_100_bits);
        Oled_SetFont(&Font_7x10);
        ssd1306_SetCursor(23, 31 - Oled_FontHeight());
        Oled_DrawStr("Conectado a");
        ssd1306_SetCursor(10, 44 - Oled_FontHeight());
        Oled_DrawStrMax(Oled_GetDashboardStaSsid(), 16u);
        ssd1306_SetCursor(8, 60 - Oled_FontHeight());
        Oled_DrawStrMax(ipStr, 16u);
    } else {
        Oled_DrawXBM(54, 3, 19, 16, Icon_Wifi_NotConnected_bits);
        Oled_SetFont(&Font_11x18);
        ssd1306_SetCursor(7, 40 - Oled_FontHeight());
        Oled_DrawStr("No hay red");
        ssd1306_SetCursor(16, 55 - Oled_FontHeight());
        Oled_DrawStr("conectada");
    }
    return;
#if 0
	    // Texto "No hay red" con fuente grande
	    Oled_SetFont(&OLED_LARGE_FONT);
	    const uint8_t fh = Oled_FontHeight();
	    ssd1306_SetCursor(19, 38 - fh);
	    Oled_DrawStr("Red");

	    // Icono WiFi desconectado (19x16 píxeles)
	    Oled_DrawXBM(54, 8, 19, 16, Icon_Wifi_100_bits);
	}else{
		ssd1306_SetColor(White);

		char ipStr[20];

		Oled_SetFont(&OLED_LARGE_FONT);
		const uint8_t fh = Oled_FontHeight();
		ssd1306_SetCursor(19, 20 - fh);
		Oled_DrawStr("Red");

		Oled_DrawXBM(54, 4, 19, 16, Icon_Wifi_100_bits);

		Oled_SetFont(&Font_7x10);
		Oled_FormatIp(&autoMicroSystem.connectivity.espStaIp, ipStr, sizeof(ipStr));
		ssd1306_SetCursor(8, 40 - Oled_FontHeight());
		Oled_DrawStr(ipStr);

		ssd1306_SetCursor(3, 58 - Oled_FontHeight());
		Oled_DrawStr(autoMicroSystem.connectivity.espFirmwareVersion);
	}

#endif
}

void OledUtils_RenderESPWifiConnectedNotification(void){
	char ipStr[20];

	Oled_FormatIp(&autoMicroSystem.connectivity.espWifiConnection.staIp, ipStr, sizeof(ipStr));
    OledUtils_RenderWiFiConnected(oled_wifi_connected_ssid, ipStr);
}

void OledUtils_RenderESPAPStartedNotification(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_ESP_AP_STARTED, SCREEN_REPORT_SOURCE_NOTIFICATION);
    char ipStr[20];

    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(2, 12, 19, 16, Icon_Wifi_100_bits);

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(27, 30 - Oled_FontHeight());
    Oled_DrawStr("AP OK");

    Oled_SetFont(&Font_7x10);
    Oled_FormatIp(&autoMicroSystem.connectivity.espApIp, ipStr, sizeof(ipStr));
    ssd1306_SetCursor(8, 60 - Oled_FontHeight());
    Oled_DrawStr(ipStr);
}

void OledUtils_RenderESPFirmwareRequestNotification(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_REQUEST, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    // Texto "Solicitando" con fuente grande
    Oled_SetFont(&Font_11x18);
    const uint8_t fh_grande = Oled_FontHeight();

    ssd1306_SetCursor(32, 24 - fh_grande);
    Oled_DrawStr("Pedir");

    // Texto "firmware"
    ssd1306_SetCursor(32, 44 - fh_grande);
    Oled_DrawStr("firmware");

    // Texto "Cancelar" con fuente pequeña
    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(34, 60 - Oled_FontHeight());
    Oled_DrawStr("ESP");

    // Botón OK / Encoder (13x13 px)
    Oled_DrawXBM(112, 48, 13, 13, Icon_Encoder_bits);

    // Icono Info (esquina superior izquierda)
    Oled_DrawXBM(8, 16, 16, 16, Icon_Info_bits);
}

void OledUtils_RenderESPFirmwareScreen(void)
{
	(void)0;
    const char *fw_text = (autoMicroSystem.connectivity.espFirmwareVersion[0] != '\0') ? autoMicroSystem.connectivity.espFirmwareVersion : "Sin datos";

    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_RECEIVED, SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    Oled_DrawXBM(8, 5, 16, 16, Icon_Info_bits);

    Oled_SetFont(&Font_11x18);
    ssd1306_SetCursor(34, 22 - Oled_FontHeight());
    Oled_DrawStr("FW ESP01");

    Oled_SetFont(&Font_7x10);
    ssd1306_SetCursor(6, 43 - Oled_FontHeight());
    Oled_DrawStrMax(fw_text, 17u);

    ssd1306_SetCursor(33, 62 - Oled_FontHeight());
    Oled_DrawStr("Volver");
    Oled_DrawXBM(33, 44, 16, 16, Icon_Encoder_bits);
}

void OledUtils_RenderESPResetSentNotification(void)
{
    OledUtils_SetScreenCode(SCREEN_CODE_CONNECTIVITY_ESP_RESET_SENT, SCREEN_REPORT_SOURCE_NOTIFICATION);
    ssd1306_Clear();
    ssd1306_SetColor(White);

    // Texto "Reset" con fuente grande
    Oled_SetFont(&Font_11x18);
    const uint8_t fh_grande = Oled_FontHeight();

    ssd1306_SetCursor(23, 24 - fh_grande);
    Oled_DrawStr("Reset");

    // Texto "enviado"
    ssd1306_SetCursor(14, 44 - fh_grande);
    Oled_DrawStr("enviado");

    // Texto "Cancelar" con fuente pequeña
    Oled_SetFont(&Font_7x10);
    const uint8_t fh_pequena = Oled_FontHeight();
    ssd1306_SetCursor(34, 60 - fh_pequena);
    Oled_DrawStr("ESP");

    // Botón OK / Encoder (13x13 px)
    // Icono Refrescar (esquina superior izquierda)
    Oled_DrawXBM(104, 16, 16, 16, Icon_Refrescar_bits);
}

void Screens_Init_oled_utils(void)
{
    oled_font = &Font_7x10;
    memcpy(&oled_wifi_connected_ssid, "WiFi", sizeof("WiFi"));
    oled_wifi_credentials_result_status = WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_FAILED;
    memcpy(&oled_remote_mode_notification_line, "a IDLE", sizeof("a IDLE"));
    scroll_last_offset = -1;
}

bool OledUtils_IsMenuScrollActive(void) { return menu_scroll_active; }

/* Public rendering entry points; private font/layout helpers stay in OLED. */
void OledUtils_RenderLineNotice(const char *message,const char *detail) {
    OledUtils_Clear(); Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(message,0,128,23);
    Oled_DrawCenteredLine(detail,0,128,39);
    Oled_DrawCenteredLine("OK: volver",0,128,58);
}
void OledUtils_RenderLineCalibration(const uint8_t *s,const char *failure) {
    OledUtils_Clear(); Oled_SetFont(&Font_7x10);
    Oled_DrawCenteredLine(s[3]?"Calibrar pista":"Calibrar blanco",0,128,11);
    if (s[2]==LINE_CAL_PROMPT) {
        Oled_DrawCenteredLine(s[3]?"Blanco Negro Blanco":"Tres sobre blanco",0,128,27);
        Oled_DrawCenteredLine("Vertical / quieto",0,128,42);
        Oled_DrawCenteredLine("OK: medir",0,128,53);
        Oled_DrawCenteredLine("Largo: salir",0,128,64);
    } else if (s[2]>=LINE_CAL_STILL && s[2]<=LINE_CAL_SAVING) {
        char info[22]; snprintf(info,sizeof(info),"Fase %u/4  %u%%",(unsigned)(s[4]<4?s[4]+1:4),(unsigned)s[5]);
        Oled_DrawCenteredLine(s[2]==LINE_CAL_STILL?"Esperando quietud":info,0,128,31);
        ssd1306_DrawLinearProgress(6,39,116,10,s[5],100);
        Oled_DrawCenteredLine("Largo: cancelar",0,128,61);
    } else {
        Oled_DrawCenteredLine(s[2]==LINE_CAL_DONE?"Calibracion lista":failure,0,128,32);
        Oled_DrawCenteredLine("OK: volver",0,128,58);
    }
}

void OledUtils_RenderMotorCalibration(const uint8_t *s,uint8_t request_reason,float percent,uint32_t ramp,uint8_t field){
 OledUtils_Clear();Oled_SetFont(&Font_7x10);Oled_DrawCenteredLine("Calibrar 1 metro",0,128,10);
 uint8_t reason=request_reason?request_reason:s[4];char text[28];
 if(s[2]==1&&(s[3]==1||s[3]==2)){
  Oled_DrawCenteredLine(s[105]==0?"Salir marca 1":s[105]==1?"Entre marcas":"Salir marca 2",0,128,25);
  uint32_t ms;memcpy(&ms,s+48,4);snprintf(text,sizeof(text),"%lu ms",(unsigned long)ms);
  Oled_DrawCenteredLine(text,0,128,40);Oled_DrawCenteredLine("USER: cancelar",0,128,61);
 }else if(s[2]==1&&s[3]==3){
  uint32_t ms;float left,right;memcpy(&ms,s+48,4);memcpy(&left,s+80,4);memcpy(&right,s+84,4);
  snprintf(text,sizeof(text),"%lu ms",(unsigned long)ms);Oled_DrawCenteredLine(text,0,128,23);
  snprintf(text,sizeof(text),"PWM %.0f / %.0f",(double)left,(double)right);Oled_DrawCenteredLine(text,0,128,38);
  Oled_DrawCenteredLine(s[107]?"USER: parar/guardar":"Guardado",0,128,53);
 }else if(reason){
  const char *messages[]={"Listo","Modo IDLE/GOTO","Calibrar MPU","Balance estable","Calibrar motores","Calibrar objetos","Calibrar linea","Ocupado","Dato invalido","Obstaculo/IR","Tiempo agotado","Parar y guardar","Cancelado","Marcha rechazada","Desvio de rumbo"};
  Oled_DrawCenteredLine(reason<15?messages[reason]:"No disponible",0,128,30);Oled_DrawCenteredLine("Largo: volver",0,128,58);
 }else{
  snprintf(text,sizeof(text),"%c Intensidad %.0f%%",field==0?'>':' ',(double)percent);Oled_DrawCenteredLine(text,0,128,23);
  snprintf(text,sizeof(text),"%c Rampa %lu ms",field==1?'>':' ',(unsigned long)ramp);Oled_DrawCenteredLine(text,0,128,36);
  Oled_DrawCenteredLine("Marcas iguales 1m",0,128,49);Oled_DrawCenteredLine("USER: comenzar",0,128,62);
 }
}
