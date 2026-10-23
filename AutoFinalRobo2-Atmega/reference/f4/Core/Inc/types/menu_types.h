/*
 * menu_types.h
 *
 *  Created on: Jun 11, 2025
 *      Author: kobac
 */

#ifndef INC_TYPES_MENU_TYPES_H_
#define INC_TYPES_MENU_TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "screen_codes.h"
#include "userEvent_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MenuFunction)();
typedef void (*ClearFunction)();
typedef void (*RenderFunction)();
typedef void (*RenderWrapperFn)(void);
typedef void (*RenderScreenFunction)();
typedef void (*UserEventManagerFn)(UserEvent_t ev);
typedef bool (*MenuVisibilityFn)(void);
typedef const uint8_t *(*MenuIconProviderFn)(void);
typedef const char *(*MenuNameProviderFn)(void);

#define MENU_SELECTION_INVALID_INDEX 0xFFu

typedef struct SubMenu SubMenu;

typedef struct MenuItem {
    const char *name;                         ///< Nombre del ítem del menú
    MenuFunction action;                      ///< Acción a ejecutar al seleccionar el ítem
    struct SubMenu *submenu;                  ///< Submenú asociado (si existe)
    const uint8_t *icon;                      ///< Icono asociado (opcional)
    RenderScreenFunction screenRenderFn;      ///< Puntero a la pantalla asociada (si existe)
    UserEventManagerFn eventManagerFn;        ///< Callback manager para la pantalla asociada (si existe)
    MenuVisibilityFn visibilityFn;            ///< Condición para mostrar/seleccionar el ítem (si existe)
    ScreenCode_t screen_code;                 ///< Identificador estable de la pantalla destino
    MenuIconProviderFn dynamicIconFn;         ///< Icono calculado en runtime (opcional)
    MenuNameProviderFn dynamicNameFn;         ///< Texto calculado en runtime (opcional)
    uint32_t required_ext_mask;               ///< Bitmask of required extensions. 0 means available in any mode.
} MenuItem;

#define MENU_EXT_NONE           0u
#define MENU_EXT_MASK(profile)  (1u << (uint32_t)(profile))

typedef void (*DrawItemFunction)(const MenuItem *item, int y, bool selected);

typedef struct SubMenu {
    const char *name;                         ///< Nombre del submenú
    MenuItem *items;                          ///< Lista de ítems del submenú
    uint8_t itemCount;                        ///< Cantidad de ítems
    int8_t currentItemIndex;                  ///< Índice del ítem seleccionado actualmente
    int8_t lastSelectedItemIndex;
    int8_t firstVisibleItem;                  ///< Primer ítem visible para scroll
    int8_t lastVisibleItem;
    struct SubMenu *parent;                   ///< Puntero al submenú padre (para 'Volver')
    const uint8_t *icon;                      ///< Icono asociado (opcional)
    ScreenCode_t screen_code;                 ///< Identificador estable de esta pantalla de menu
    uint8_t max_visible_items;                ///< 0 uses the normal three-row layout.
} SubMenu;

typedef struct MenuServices {
    void *ctx;
    bool (*item_available)(void *ctx, const MenuItem *item);
    bool (*report_screen)(void *ctx, uint32_t code, uint8_t source);
    bool (*report_selection)(void *ctx, uint32_t code, uint8_t index, uint8_t count, uint8_t source);
} MenuServices;

typedef struct MenuSystem {
    SubMenu *root;
    const MenuServices *services;
    volatile uint8_t *fast_scroll_enabled;
    SubMenu *currentMenu;                     ///< Submenú actual
    ClearFunction clearScreen;                ///< Callback para limpiar la pantalla
    DrawItemFunction drawItem;                ///< Callback para dibujar ítems del menú
    RenderFunction renderFn;                  ///< Callback para renderizar la pantalla
    RenderFunction dashboardRender;
    UserEventManagerFn userEventManagerFn;
    volatile uint8_t *insideMenuFlag;         ///< Puntero a una bandera externa que indica si estamos dentro del menú
    volatile bool renderFlag;
    bool suppressClearOnNextRender;           ///< Si es true, no limpia la pantalla antes del próximo renderizado
    bool allowPeriodicRefresh;
    ScreenCode_t current_screen_code;
    ScreenCode_t last_reported_screen_code;
    uint8_t current_screen_source;
    bool screen_report_pending;
    uint8_t current_selected_index;
    uint8_t last_reported_selected_index;
    uint8_t current_item_count;
    uint8_t last_reported_item_count;
    uint8_t current_selection_source;
    bool selection_report_pending;
} MenuSystem;

#ifdef __cplusplus
}
#endif

#endif /* INC_TYPES_MENU_TYPES_H_ */
