/*
 * userEvent_type.h
 *
 *  User event type shared by the interaction layer.
 */

#ifndef INC_TYPES_USEREVENT_TYPE_H_
#define INC_TYPES_USEREVENT_TYPE_H_

typedef enum {
    UE_NONE = 0,
    UE_SHORT_PRESS,
    UE_LONG_PRESS,
    UE_ENC_SHORT_PRESS,
    UE_ENC_LONG_PRESS,
    UE_ROTATE_CW,
    UE_ROTATE_CCW
} UserEvent_t;

#endif /* INC_TYPES_USEREVENT_TYPE_H_ */
