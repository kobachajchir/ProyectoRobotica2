/*
 * ui_event_router.h
 *
 *  Created on: Sep 22, 2025
 *      Author: kobac
 */

#ifndef INC_UI_EVENT_ROUTER_H_
#define INC_UI_EVENT_ROUTER_H_

#include "types/userEvent_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_EVENT_ROUTER_IGNORED = 0u,
    UI_EVENT_ROUTER_CONSUMED = 1u
} UiEventRouterResult_t;

UiEventRouterResult_t UiEventRouter_HandleEvent(UserEvent_t ev);

#ifdef __cplusplus
}
#endif

#endif /* INC_UI_EVENT_ROUTER_H_ */
