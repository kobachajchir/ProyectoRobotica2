#ifndef SYSTEM_FLAGS_H
#define SYSTEM_FLAGS_H
#include "utils/macros_utils.h"
#define INIT_CAR                                 BIT0_MASK
#define PROCESS_IR_DATA                         BIT1_MASK
#define TCRT_CALIB_BLACK_AND_WHITE_COMPLETE     BIT2_MASK
#define TCRT_CALIB_OBSTACLES_COMPLETE           BIT3_MASK
#define MPU_GET_DATA                            BIT4_MASK
#define OLED_READY                              BIT5_MASK
#define OLED_TENMS_PASSED                       BIT6_MASK
#define OLED_REFRESH                            BIT7_MASK

#define OLED_ACTIVE                             BIT0_MASK
#define WIFI_ACTIVE                             BIT1_MASK
#define AP_ACTIVE                               BIT2_MASK
#define USB_ACTIVE                              BIT3_MASK
#define RF_ACTIVE                               BIT4_MASK
#define MODIFYING_CARMODE                       BIT5_MASK
#define ESP_PRESENT                             BIT6_MASK
#define SHOWSECONDSCREEN                        BIT7_MASK

#define CHK_ESP_CONN                            BIT0_MASK
#define WIFI_SEARCHING                          BIT1_MASK

#define SYSTEM_CAR_MODE(flags) NIBBLEH_GET_STATE(flags)
#endif
