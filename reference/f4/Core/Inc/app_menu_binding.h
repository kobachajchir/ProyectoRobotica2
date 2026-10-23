#ifndef APP_MENU_BINDING_H
#define APP_MENU_BINDING_H
#include "menusystem.h"
extern MenuSystem menuSystem;
#define MenuSys_IsItemAvailable(item) MenuSys_IsItemAvailable_H(&menuSystem, (item))
#endif
