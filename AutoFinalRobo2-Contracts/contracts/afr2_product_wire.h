/* AFR2-20261023-19. Opt-in product extension EX01 of ROBO2/profile2.
 * No change to afr2_wire.h framing, profile2 hello or legacy capability mask.
 */
#ifndef AFR2_PRODUCT_WIRE_H
#define AFR2_PRODUCT_WIRE_H
#include "afr2_wire.h"
#include "afr2_network_wire.h"
#include "afr2_sensor_wire.h"
#include "afr2_battery_wire.h"
#define AFR2_EXT_HELLO 0x10u
#define AFR2_EXT_STATE 0x11u
#define AFR2_EXT_WEB_PULSE 0x12u
#define AFR2_EXT_DRIVE 0x13u
#define AFR2_EXT_WHEEL 0x14u
#define AFR2_EXT_MODE 0x15u
#define AFR2_EXT_GOTO 0x16u
#define AFR2_EXT_CANCEL 0x17u
#define AFR2_EXT_POSE 0x18u
#define AFR2_EXT_HALL_CAL 0x19u
#define AFR2_EXT_CONFIG 0x1au
#define AFR2_EXT_PREFS 0x1bu
#define AFR2_EXT_LINE_CAL 0x1cu
#define AFR2_EXT_LIMITS 0x1du
/* AFR2-20261029-27: signed percentages forward/strafe/turn, explicit units. */
#define AFR2_EXT_DRIVE_PWM 0x1eu
#define AFR2_EXT_F_MANUAL_PWM 0x80000UL
#define AFR2_EXT_UI_META 0x20u
#define AFR2_EXT_UI_PAGE 0x21u
#define AFR2_EXT_UI_ACTION 0x22u
#define AFR2_EXT_UI_RELEASE 0x23u
/* AFR2-20261029-19: presentation-only touch ownership and explicit menu return. */
#define AFR2_EXT_TOUCH_UI 0x24u
#define AFR2_EXT_F_TOUCH_UI 0x20000UL
#define AFR2_EXT_WIFI_POLL 0x30u
#define AFR2_EXT_WIFI_UPDATE 0x31u
#define AFR2_EXT_DIAG_POLL 0x32u
#define AFR2_EXT_DIAG_RESULT 0x33u
/* AFR2-20261024-01: sequence range does not replace EX01 product IDs. */
#define AFR2_EXT_SEQ_BEGIN 0x40u
#define AFR2_EXT_SEQ_APPEND 0x41u
#define AFR2_EXT_SEQ_COMMIT 0x42u
#define AFR2_EXT_SEQ_START 0x43u
#define AFR2_EXT_SEQ_CANCEL 0x44u
#define AFR2_EXT_SEQ_STATUS 0x45u
#define AFR2_EXT_F_SEQUENCES 0x100UL
#define AFR2_EXT_INFO 0x50u
#define AFR2_EXT_REBOOT 0x51u
#define AFR2_EXT_BOOT 0x52u
#define AFR2_EXT_VERSION 13u
#define AFR2_EXT_F_SENSOR_HEAD 0x8000UL
#define AFR2_EXT_F_RADAR 0x10000UL
#define AFR2_EXT_F_DISTANCE 0x4000UL
#define AFR2_EXT_CHUNK 40u
#define AFR2_EXT_F_UI 0x01UL
#define AFR2_EXT_F_STOP 0x02UL
#define AFR2_EXT_F_LINE 0x04UL
#define AFR2_EXT_F_HALL 0x08UL
#define AFR2_EXT_F_MOTION 0x10UL
#define AFR2_EXT_F_POSE 0x20UL
#define AFR2_EXT_F_WIFI 0x40UL
#define AFR2_EXT_F_PREFS 0x80UL
#define AFR2_EXT_F_GOTO 0x200UL
#define AFR2_EXT_F_CONFIG 0x400UL
#define AFR2_EXT_F_WHEEL 0x800UL
#define AFR2_EXT_F_SYSTEM 0x1000UL
#define AFR2_EXT_F_NETWORK_UI 0x2000UL
#define AFR2_EXT_FEATURE_MASK 0xfffffUL
/* Result byte retains legacy 0..4. Busy/denied are mapped to unavailable and
 * never OK; detailed requirement/status can be read through EX_STATE.
 */
#endif
