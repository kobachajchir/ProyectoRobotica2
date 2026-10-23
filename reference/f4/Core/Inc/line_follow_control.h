#ifndef INC_LINE_FOLLOW_CONTROL_H_
#define INC_LINE_FOLLOW_CONTROL_H_
#include "line_follow_control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void LineFollowControl_GetDefaultConfig(LineFollow_Config_t *config);
AM_Result LineFollowControl_Init(LineFollowControl_Handle *line,
                                         const LineFollow_Config_t *config);
void LineFollowControl_Reset(LineFollowControl_Handle *line);
void LineFollowControl_SetEnabled(LineFollowControl_Handle *line, uint8_t enabled);
void LineFollowControl_SetLineBlackWidth(LineFollowControl_Handle *line,
                                         uint16_t width_mm);
void LineFollowControl_Task(LineFollowControl_Handle *line, uint32_t now_ms);
const LineFollow_Status_t *LineFollowControl_GetStatus(const LineFollowControl_Handle *line);
const char *LineFollowControl_GetFsmText(LineFollow_Fsm_t fsm);

#ifdef __cplusplus
}
#endif

#endif /* INC_LINE_FOLLOW_CONTROL_H_ */
