/*
 * menusystem.c
 *
 *  Created on: Jun 11, 2025
 *      Author: kobac
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "types/menu_types.h"
#include "menusystem.h"

#define GET_MAX_VISIBLE(menu) ((menu)->max_visible_items ? (menu)->max_visible_items : 3u)
#define MENU_ITEM_Y0 4u
#define MENU_ITEM_SPACING 21u

static bool MenuSys_HasNavigableSelection(const MenuSystem *ms);
static bool MenuSys_IsCurrentScreenMenu(const MenuSystem *ms);
static bool MenuSys_GetVisibleSelection(const SubMenu *menu,
                                        uint8_t *selected_index,
                                        uint8_t *item_count);
static bool MenuSys_ResolveCurrentMenuSelection(const MenuSystem *ms,
                                                uint8_t *selected_index,
                                                uint8_t *item_count);
static void MenuSys_UpdateSelectionFromCurrentMenu(MenuSystem *ms, ScreenReportSource_t source);
static bool MenuSys_SelectParentItemForChild(SubMenu *parent, const SubMenu *child);

bool MenuSys_IsItemVisible(const MenuItem *item) {
    if (!item) return false;
    if (!item->visibilityFn) return true;
    return item->visibilityFn();
}

bool MenuSys_IsItemAvailable_H(const MenuSystem *ms, const MenuItem *item)
{
    if (!item) return false;
    return !ms || !ms->services || !ms->services->item_available ||
           ms->services->item_available(ms->services->ctx, item);
}

void MenuSys_FlushScreenReport(MenuSystem *ms)
{
    if (!ms || !ms->screen_report_pending || ms->current_screen_code == SCREEN_CODE_NONE) {
        return;
    }

    if (ms->services && ms->services->report_screen && ms->services->report_screen(ms->services->ctx,
                                       (uint32_t)ms->current_screen_code,
                                       ms->current_screen_source)) {
        ms->last_reported_screen_code = ms->current_screen_code;
        ms->screen_report_pending = false;
    }
}

void MenuSys_FlushMenuSelectionReport(MenuSystem *ms)
{
    if (!ms || ms->screen_report_pending || !ms->selection_report_pending ||
        !MenuSys_HasNavigableSelection(ms)) {
        return;
    }

    if (ms->services && ms->services->report_selection && ms->services->report_selection(ms->services->ctx,
                                              (uint32_t)ms->current_screen_code,
                                              ms->current_selected_index,
                                              ms->current_item_count,
                                              ms->current_selection_source)) {
        ms->last_reported_selected_index = ms->current_selected_index;
        ms->last_reported_item_count = ms->current_item_count;
        ms->selection_report_pending = false;
    }
}

void MenuSys_SetCurrentMenuSelection(MenuSystem *ms,
                                     uint8_t selected_index,
                                     uint8_t item_count,
                                     ScreenReportSource_t source)
{
    if (!ms || item_count == 0u || selected_index >= item_count ||
        ms->current_screen_code == SCREEN_CODE_NONE) {
        return;
    }

    if (ms->current_selected_index != selected_index ||
        ms->current_item_count != item_count ||
        ms->current_selection_source != (uint8_t)source ||
        ms->last_reported_selected_index != selected_index ||
        ms->last_reported_item_count != item_count) {
        ms->current_selected_index = selected_index;
        ms->current_item_count = item_count;
        ms->current_selection_source = (uint8_t)source;
        ms->selection_report_pending = true;
    }

    MenuSys_FlushMenuSelectionReport(ms);
}

void MenuSys_ReportCurrentMenuSelection(MenuSystem *ms, ScreenReportSource_t source)
{
    MenuSys_UpdateSelectionFromCurrentMenu(ms, source);
}

void MenuSys_SetCurrentScreenCode(MenuSystem *ms, ScreenCode_t screen_code, ScreenReportSource_t source)
{
    if (!ms || screen_code == SCREEN_CODE_NONE) {
        return;
    }

    if (ms->current_screen_code != screen_code ||
        ms->current_screen_source != (uint8_t)source ||
        ms->last_reported_screen_code != screen_code) {
        ms->current_screen_code = screen_code;
        ms->current_screen_source = (uint8_t)source;
        ms->screen_report_pending = true;
    }

    MenuSys_FlushScreenReport(ms);
    if (MenuSys_IsCurrentScreenMenu(ms)) {
        MenuSys_UpdateSelectionFromCurrentMenu(ms, source);
    } else {
        MenuSys_ClearCurrentMenuSelection(ms, source);
    }
}

void MenuSys_SetCurrentMenuScreenCode(MenuSystem *ms)
{
    if (!ms || !ms->currentMenu) {
        return;
    }

    MenuSys_SetCurrentScreenCode(ms,
                                 ms->currentMenu->screen_code,
                                 SCREEN_REPORT_SOURCE_MENU);
}

void MenuSys_ClearCurrentMenuSelection(MenuSystem *ms, ScreenReportSource_t source)
{
    if (ms == NULL) {
        return;
    }

    if (ms->current_selected_index != MENU_SELECTION_INVALID_INDEX ||
        ms->current_item_count != 0u ||
        ms->last_reported_selected_index != MENU_SELECTION_INVALID_INDEX ||
        ms->last_reported_item_count != 0u ||
        ms->current_selection_source != (uint8_t)source) {
        ms->current_selected_index = MENU_SELECTION_INVALID_INDEX;
        ms->current_item_count = 0u;
        ms->last_reported_selected_index = MENU_SELECTION_INVALID_INDEX;
        ms->last_reported_item_count = 0u;
        ms->current_selection_source = (uint8_t)source;
        ms->selection_report_pending = false;
    }
}

static bool MenuSys_HasNavigableSelection(const MenuSystem *ms)
{
    return (ms != NULL) &&
           MenuSys_IsCurrentScreenMenu(ms) &&
           (ms->current_screen_code != SCREEN_CODE_NONE) &&
           (ms->current_item_count > 0u) &&
           (ms->current_selected_index < ms->current_item_count);
}

static bool MenuSys_IsCurrentScreenMenu(const MenuSystem *ms)
{
    return (ms != NULL) &&
           (ms->currentMenu != NULL) &&
           (ms->current_screen_code == ms->currentMenu->screen_code);
}

static bool MenuSys_GetVisibleSelection(const SubMenu *menu,
                                        uint8_t *selected_index,
                                        uint8_t *item_count)
{
    uint8_t visible_count = 0u;
    uint8_t visible_selected = MENU_SELECTION_INVALID_INDEX;

    if (!menu || !selected_index || !item_count) {
        return false;
    }

    for (int8_t i = 0; i < menu->itemCount; i++) {
        if (MenuSys_IsItemVisible(&menu->items[i])) {
            if (i == menu->currentItemIndex) {
                visible_selected = visible_count;
            }
            visible_count++;
        }
    }

    if (visible_count == 0u || visible_selected == MENU_SELECTION_INVALID_INDEX) {
        return false;
    }

    *selected_index = visible_selected;
    *item_count = visible_count;
    return true;
}

void MenuSys_NotifyScreenContentChanged(MenuSystem *ms, ScreenReportSource_t source)
{
    if (!ms || ms->current_screen_code == SCREEN_CODE_NONE) {
        return;
    }

    if (MenuSys_IsCurrentScreenMenu(ms)) {
        ms->current_selection_source = (uint8_t)source;
        (void)MenuSys_RequestCurrentMenuSelectionSnapshot(ms);
        return;
    }

    ms->current_screen_source = (uint8_t)source;
    ms->screen_report_pending = true;
}

static bool MenuSys_ResolveCurrentMenuSelection(const MenuSystem *ms,
                                                uint8_t *selected_index,
                                                uint8_t *item_count)
{
    if (!MenuSys_IsCurrentScreenMenu(ms) ||
        ms->currentMenu->itemCount == 0u ||
        ms->currentMenu->currentItemIndex < 0 ||
        ms->currentMenu->currentItemIndex >= ms->currentMenu->itemCount) {
        return false;
    }

    return MenuSys_GetVisibleSelection(ms->currentMenu,
                                       selected_index,
                                       item_count);
}

static void MenuSys_UpdateSelectionFromCurrentMenu(MenuSystem *ms, ScreenReportSource_t source)
{
    uint8_t selected_index;
    uint8_t item_count;

    if (!MenuSys_ResolveCurrentMenuSelection(ms, &selected_index, &item_count)) {
        MenuSys_ClearCurrentMenuSelection(ms, source);
        return;
    }

    MenuSys_SetCurrentMenuSelection(ms,
                                    selected_index,
                                    item_count,
                                    source);
}

bool MenuSys_RequestCurrentMenuSelectionSnapshot(MenuSystem *ms)
{
    uint8_t selected_index;
    uint8_t item_count;

    if (!MenuSys_ResolveCurrentMenuSelection(ms, &selected_index, &item_count)) {
        return false;
    }

    ms->current_selected_index = selected_index;
    ms->current_item_count = item_count;
    if (ms->current_selection_source == SCREEN_REPORT_SOURCE_UNKNOWN) {
        ms->current_selection_source = SCREEN_REPORT_SOURCE_MENU;
    }

    /* Poll owns transmission; BUSY leaves this flag set for a later retry. */
    ms->selection_report_pending = true;
    return true;
}

static bool MenuSys_SelectParentItemForChild(SubMenu *parent, const SubMenu *child)
{
    if (parent == NULL || child == NULL) {
        return false;
    }

    for (int8_t i = 0; i < parent->itemCount; i++) {
        if (parent->items[i].submenu == child && MenuSys_IsItemVisible(&parent->items[i])) {
            parent->currentItemIndex = i;
            return true;
        }
    }

    return false;
}

ScreenCode_t MenuSys_GetCurrentScreenCode(const MenuSystem *ms)
{
    return ms ? ms->current_screen_code : SCREEN_CODE_NONE;
}

uint8_t MenuSys_GetCurrentScreenSource(const MenuSystem *ms)
{
    return ms ? ms->current_screen_source : SCREEN_REPORT_SOURCE_UNKNOWN;
}

static int8_t MenuSys_FindFirstVisibleIndexFrom(const SubMenu *menu, int8_t start) {
    if (!menu) return -1;
    if (start < 0) start = 0;
    for (int8_t i = start; i < menu->itemCount; i++) {
        if (MenuSys_IsItemVisible(&menu->items[i])) {
            return i;
        }
    }
    return -1;
}

static int8_t MenuSys_FindLastVisibleIndexBefore(const SubMenu *menu, int8_t start) {
    if (!menu) return -1;
    if (start >= menu->itemCount) start = menu->itemCount - 1;
    for (int8_t i = start; i >= 0; i--) {
        if (MenuSys_IsItemVisible(&menu->items[i])) {
            return i;
        }
    }
    return -1;
}

static int8_t MenuSys_FindNextVisibleIndex(const SubMenu *menu, int8_t start, int8_t direction) {
    if (!menu || (direction != 1 && direction != -1)) return -1;
    int8_t i = start + direction;
    while (i >= 0 && i < menu->itemCount) {
        if (MenuSys_IsItemVisible(&menu->items[i])) {
            return i;
        }
        i += direction;
    }
    return -1;
}

static int8_t MenuSys_ComputeLastVisibleIndex(const SubMenu *menu, int8_t firstVisibleIndex) {
    if (!menu) return -1;
    if (firstVisibleIndex < 0) return -1;
    int8_t lastVisible = -1;
    uint8_t count = 0;
    for (int8_t i = firstVisibleIndex; i < menu->itemCount; i++) {
        if (MenuSys_IsItemVisible(&menu->items[i])) {
            lastVisible = i;
            count++;
            if (count >= GET_MAX_VISIBLE(menu)) {
                break;
            }
        }
    }
    return lastVisible;
}

static int8_t MenuSys_FindWrappedVisibleIndex(const SubMenu *menu, int8_t start, int8_t direction) {
    int8_t nextIndex;

    if (!menu || menu->itemCount == 0u || (direction != 1 && direction != -1)) return -1;

    nextIndex = MenuSys_FindNextVisibleIndex(menu, start, direction);
    if (nextIndex >= 0) {
        return nextIndex;
    }

    return (direction > 0)
        ? MenuSys_FindFirstVisibleIndexFrom(menu, 0)
        : MenuSys_FindLastVisibleIndexBefore(menu, menu->itemCount - 1);
}

static void MenuSys_EnsureCurrentSelectionVisible(SubMenu *menu) {
    int8_t firstVisible;
    int8_t lastVisible;

    if (!menu || menu->currentItemIndex < 0) return;

    firstVisible = MenuSys_FindFirstVisibleIndexFrom(menu, menu->firstVisibleItem);
    if (firstVisible < 0) {
        firstVisible = MenuSys_FindFirstVisibleIndexFrom(menu, 0);
    }
    if (firstVisible < 0) {
        return;
    }

    while (menu->currentItemIndex < firstVisible) {
        int8_t prevFirst = MenuSys_FindLastVisibleIndexBefore(menu, firstVisible - 1);
        if (prevFirst < 0) break;
        firstVisible = prevFirst;
    }

    lastVisible = MenuSys_ComputeLastVisibleIndex(menu, firstVisible);
    while (lastVisible >= 0 && menu->currentItemIndex > lastVisible) {
        int8_t nextFirst = MenuSys_FindFirstVisibleIndexFrom(menu, firstVisible + 1);
        if (nextFirst < 0) break;
        firstVisible = nextFirst;
        lastVisible = MenuSys_ComputeLastVisibleIndex(menu, firstVisible);
    }

    menu->firstVisibleItem = firstVisible;
}

bool MenuSys_SelectVisibleItem(MenuSystem *ms, uint8_t visible_position_one_based)
{
    SubMenu *menu;
    int8_t target;

    if (!MenuSys_IsCurrentScreenMenu(ms) ||
        visible_position_one_based == 0u ||
        visible_position_one_based > GET_MAX_VISIBLE(ms->currentMenu)) {
        return false;
    }

    menu = ms->currentMenu;
    target = MenuSys_FindFirstVisibleIndexFrom(menu, menu->firstVisibleItem);
    if (target < 0) {
        target = MenuSys_FindFirstVisibleIndexFrom(menu, 0);
    }

    for (uint8_t position = 1u;
         target >= 0 && position < visible_position_one_based;
         ++position) {
        target = MenuSys_FindNextVisibleIndex(menu, target, 1);
    }

    if (target < 0) {
        return false;
    }

    menu->lastSelectedItemIndex = menu->currentItemIndex;
    menu->lastVisibleItem = menu->firstVisibleItem;
    menu->currentItemIndex = target;
    MenuSys_EnsureCurrentSelectionVisible(menu);
    ms->renderFlag = true;
    MenuSys_ReportCurrentMenuSelection(ms, SCREEN_REPORT_SOURCE_MENU);
    return true;
}

bool MenuSys_MoveVisiblePage(MenuSystem *ms, int8_t direction)
{
    SubMenu *menu;
    int8_t first;
    int8_t target;

    if (!MenuSys_IsCurrentScreenMenu(ms) ||
        (direction != -1 && direction != 1)) {
        return false;
    }

    menu = ms->currentMenu;
    first = MenuSys_FindFirstVisibleIndexFrom(menu, menu->firstVisibleItem);
    if (first < 0) {
        first = MenuSys_FindFirstVisibleIndexFrom(menu, 0);
    }
    if (first < 0) {
        return false;
    }

    target = first;
    if (direction > 0) {
        for (uint8_t step = 0u; step < GET_MAX_VISIBLE(menu); ++step) {
            int8_t n = MenuSys_FindNextVisibleIndex(menu, target, 1);
            if (n < 0) {
                /* A page request at the end is a valid no-op. */
                return true;
            }
            target = n;
        }
    } else {
        for (uint8_t step = 0u; step < GET_MAX_VISIBLE(menu); ++step) {
            int8_t previous = MenuSys_FindNextVisibleIndex(menu, target, -1);
            if (previous < 0) {
                break;
            }
            target = previous;
        }
        if (target == first) {
            /* A page request at the beginning is a valid no-op. */
            return true;
        }
    }

    menu->lastSelectedItemIndex = menu->currentItemIndex;
    menu->lastVisibleItem = menu->firstVisibleItem;
    menu->firstVisibleItem = target;
    menu->currentItemIndex = target;
    ms->renderFlag = true;
    MenuSys_ReportCurrentMenuSelection(ms, SCREEN_REPORT_SOURCE_MENU);
    return true;
}

void MenuSys_Init(MenuSystem *ms) {
    if (!ms || !ms->currentMenu) return;
    int8_t firstVisible = MenuSys_FindFirstVisibleIndexFrom(ms->currentMenu, 0);
    if (firstVisible < 0) {
        return;
    }
    ms->currentMenu->currentItemIndex = firstVisible;
    ms->currentMenu->lastSelectedItemIndex = firstVisible;
    ms->currentMenu->firstVisibleItem = firstVisible;
    ms->renderFlag = false;
    ms->allowPeriodicRefresh = false;
    ms->userEventManagerFn = NULL;
    if (ms->current_screen_code == SCREEN_CODE_NONE) {
        ms->current_screen_code = SCREEN_CODE_CORE_STARTUP;
    }
    ms->last_reported_screen_code = SCREEN_CODE_NONE;
    ms->current_screen_source = SCREEN_REPORT_SOURCE_SYSTEM;
    ms->screen_report_pending = false;
    ms->current_selected_index = MENU_SELECTION_INVALID_INDEX;
    ms->last_reported_selected_index = MENU_SELECTION_INVALID_INDEX;
    ms->current_item_count = 0u;
    ms->last_reported_item_count = 0u;
    ms->current_selection_source = SCREEN_REPORT_SOURCE_UNKNOWN;
    ms->selection_report_pending = false;
}

void MenuSys_SetCallbacks(MenuSystem *ms,
                          ClearFunction    clear,
                          DrawItemFunction draw,
                          RenderFunction   render,
                          volatile uint8_t *insideFlag,
						  RenderFunction dashboardRender)
{
    if (!ms) return;
    ms->clearScreen    = clear;
    ms->drawItem       = draw;
    ms->renderFn       = render;
    ms->dashboardRender = dashboardRender;
    ms->insideMenuFlag = insideFlag;
}


/**
 * @brief  Mueve el cursor hacia arriba (si puede), ajusta el “bucket” de 3 ítems
 *         y no levanta renderFlag (lo hace el llamador).
 */

void MenuSys_MoveCursorUp(MenuSystem *ms) {
    if (!ms || !ms->currentMenu) return;
    SubMenu *m = ms->currentMenu;

    int8_t nextIndex;

    nextIndex = MenuSys_FindWrappedVisibleIndex(m, m->currentItemIndex, -1);
    if (nextIndex < 0) return;
    if (nextIndex == m->currentItemIndex) return;

    m->lastSelectedItemIndex = m->currentItemIndex;
    m->lastVisibleItem       = m->firstVisibleItem;   // ← guardo bucket viejo
    m->currentItemIndex      = nextIndex;

    MenuSys_EnsureCurrentSelectionVisible(m);

    MenuSys_ReportCurrentMenuSelection(ms, SCREEN_REPORT_SOURCE_MENU);
}

void MenuSys_MoveCursorDown(MenuSystem *ms) {
    if (!ms || !ms->currentMenu) return;
    SubMenu *m = ms->currentMenu;

    int8_t nextIndex;

    nextIndex = MenuSys_FindWrappedVisibleIndex(m, m->currentItemIndex, 1);
    if (nextIndex < 0) return;
    if (nextIndex == m->currentItemIndex) return;

    m->lastSelectedItemIndex = m->currentItemIndex;
    m->lastVisibleItem       = m->firstVisibleItem;   // ← guardo bucket viejo
    m->currentItemIndex      = nextIndex;

    MenuSys_EnsureCurrentSelectionVisible(m);

    MenuSys_ReportCurrentMenuSelection(ms, SCREEN_REPORT_SOURCE_MENU);
}

/**
 * @brief  abre un submenú, reinicia índices y levanta renderFlag.
 */
void MenuSys_OpenSubMenu(MenuSystem *ms, SubMenu *submenu) {
    if (!ms || !submenu) return;
    // Al abrir cualquier menú, activamos el contexto de navegación de menú.
    if (ms->insideMenuFlag) {
        *(ms->insideMenuFlag) = true;
    }
    ms->currentMenu            = submenu;
    if (ms->fast_scroll_enabled) *ms->fast_scroll_enabled = 0;
    int8_t firstVisible = MenuSys_FindFirstVisibleIndexFrom(submenu, 0);
    if (firstVisible < 0) return;
    submenu->currentItemIndex  = firstVisible;
    submenu->firstVisibleItem  = firstVisible;
    submenu->lastSelectedItemIndex = -1; //Asi renderiza todo nuevamente
    submenu->lastVisibleItem = -1; //Asi renderiza todo nuevamente
    ms->renderFlag             = true;
    MenuSys_SetCurrentScreenCode(ms, submenu->screen_code, SCREEN_REPORT_SOURCE_MENU);
}

void MenuSys_ResetMenu(MenuSystem *ms) {
	if (!ms) return;
	ms->currentMenu            = ms->root;
	if (ms->fast_scroll_enabled) *ms->fast_scroll_enabled = 0;
	int8_t firstVisible = MenuSys_FindFirstVisibleIndexFrom(ms->currentMenu, 0);
	if (firstVisible < 0) return;
	ms->currentMenu->currentItemIndex  = firstVisible;
	ms->currentMenu->firstVisibleItem  = firstVisible;
	ms->currentMenu->lastSelectedItemIndex = -1; //Asi renderiza todo nuevamente
	ms->currentMenu->lastVisibleItem = -1; //Asi renderiza todo nuevamente
	MenuSys_SetCurrentScreenCode(ms,
	                             ms->currentMenu->screen_code,
	                             SCREEN_REPORT_SOURCE_MENU);
}

/**
 * @brief  sube un nivel o sale al main, y levanta renderFlag.
 */
void MenuSys_NavigateBack(MenuSystem *ms) {
    if (!ms || !ms->currentMenu) return;

    if (ms->fast_scroll_enabled) *ms->fast_scroll_enabled = 0;

    SubMenu *child = ms->currentMenu;
    SubMenu *parent = child->parent;
    if (parent) {
        ms->currentMenu = parent;

        (void)MenuSys_SelectParentItemForChild(parent, child);

        if (!MenuSys_IsItemVisible(&parent->items[parent->currentItemIndex])) {
            int8_t nextVisible = MenuSys_FindFirstVisibleIndexFrom(parent, parent->currentItemIndex);
            if (nextVisible < 0) {
                nextVisible = MenuSys_FindLastVisibleIndexBefore(parent, parent->currentItemIndex);
            }
            if (nextVisible >= 0) {
                parent->currentItemIndex = nextVisible;
            }
        }

        int8_t firstVisible = parent->firstVisibleItem;
        if (firstVisible < 0 || firstVisible >= parent->itemCount ||
            !MenuSys_IsItemVisible(&parent->items[firstVisible])) {
            firstVisible = MenuSys_FindFirstVisibleIndexFrom(parent, 0);
        }

        while (firstVisible >= 0 && parent->currentItemIndex < firstVisible) {
            int8_t prevFirst = MenuSys_FindLastVisibleIndexBefore(parent, firstVisible - 1);
            if (prevFirst < 0) break;
            firstVisible = prevFirst;
        }

        int8_t lastVisible = MenuSys_ComputeLastVisibleIndex(parent, firstVisible);
        while (firstVisible >= 0 && lastVisible >= 0 && parent->currentItemIndex > lastVisible) {
            int8_t nextFirst = MenuSys_FindFirstVisibleIndexFrom(parent, firstVisible + 1);
            if (nextFirst < 0) break;
            firstVisible = nextFirst;
            lastVisible = MenuSys_ComputeLastVisibleIndex(parent, firstVisible);
        }

        if (firstVisible >= 0) {
            parent->firstVisibleItem = firstVisible;
        }

        parent->lastSelectedItemIndex = -1;
        parent->lastVisibleItem = -1;
        ms->renderFlag = true;
        /* Publicar la salida desde el evento local. El render completo vuelve
         * a confirmarla, pero la Web no debe esperar otro giro del encoder. */
        MenuSys_SetCurrentScreenCode(ms,
                                     parent->screen_code,
                                     SCREEN_REPORT_SOURCE_MENU);
        return;
    }

    // en el Main: vamos al dashboard
    if (ms->dashboardRender) {
        ms->dashboardRender();
    }
    // salimos del menú
    if (ms->insideMenuFlag) *ms->insideMenuFlag = false;
}

/**
 * @brief  va directamente al menú principal, levanta renderFlag.
 */
void MenuSys_NavigateToMain(MenuSystem *ms) {
    if (ms->fast_scroll_enabled) *ms->fast_scroll_enabled = 0;
    MenuSys_OpenSubMenu(ms, ms->root);
}

/**
 * @brief  Renderiza el menú actual usando los callbacks, borrando pantalla al cambiar de página.
 */
void MenuSys_RenderMenu(MenuSystem *ms) {
    if (!ms || !ms->currentMenu || !ms->clearScreen || !ms->drawItem)
        return;

    SubMenu *m = ms->currentMenu;
    MenuSys_SetCurrentMenuScreenCode(ms);

    int8_t firstVisible = MenuSys_FindFirstVisibleIndexFrom(m, m->firstVisibleItem);
    if (firstVisible < 0) return;

    if (!MenuSys_IsItemVisible(&m->items[m->currentItemIndex])) {
        int8_t nextVisible = MenuSys_FindFirstVisibleIndexFrom(m, m->currentItemIndex);
        if (nextVisible < 0) {
            nextVisible = MenuSys_FindLastVisibleIndexBefore(m, m->currentItemIndex);
        }
        if (nextVisible >= 0) {
            m->currentItemIndex = nextVisible;
        }
    }

    m->firstVisibleItem = firstVisible;

    // 1) Limpiar toda la pantalla
    if (!ms->suppressClearOnNextRender && ms->clearScreen) {
        ms->clearScreen();
    }
    ms->suppressClearOnNextRender = false;

    // 2) Rango de ítems a mostrar
    int8_t drawIndex = firstVisible;
    uint8_t drawn = 0;

    // 3) Dibujar cada uno
    while (drawIndex >= 0 && drawIndex < m->itemCount && drawn < GET_MAX_VISIBLE(m)) {
        uint8_t y        = MENU_ITEM_Y0 + drawn * MENU_ITEM_SPACING;
        bool    selected = (drawIndex == m->currentItemIndex);
        ms->drawItem(&m->items[drawIndex], y, selected);
        drawn++;
        drawIndex = MenuSys_FindNextVisibleIndex(m, drawIndex, 1);
    }

    // 4) Actualizo el estado para el wrapper
    m->lastVisibleItem       = m->firstVisibleItem;
    m->lastSelectedItemIndex = m->currentItemIndex;
}

/**
 * @brief  maneja “select”: abre submenú o llama action, luego re-renderiza menú.
 */
void MenuSys_HandleClick(MenuSystem *ms) {
    if (!ms || !ms->currentMenu) return;

    SubMenu  *m   = ms->currentMenu;
    int8_t    idx = m->currentItemIndex;
    if (idx >= m->itemCount) return;

    if (!MenuSys_IsItemVisible(&m->items[idx])) {
        int8_t nextVisible = MenuSys_FindFirstVisibleIndexFrom(m, idx);
        if (nextVisible < 0) {
            nextVisible = MenuSys_FindLastVisibleIndexBefore(m, idx);
        }
        if (nextVisible < 0) {
            return;
        }
        m->currentItemIndex = nextVisible;
        idx = nextVisible;
    }

    MenuItem *it = &m->items[idx];

    /*
     * Publish terminal navigation from the click itself. The renderer repeats
     * this assignment when it draws, but it may run after the bridge poll; in
     * that window the live Web view would otherwise keep showing the menu.
     */
    if (it->screenRenderFn && it->screen_code != SCREEN_CODE_NONE && !it->submenu) {
        MenuSys_SetCurrentScreenCode(ms,
                                     it->screen_code,
                                     SCREEN_REPORT_SOURCE_RENDER);
    }

    if (!MenuSys_IsItemAvailable_H(ms, it)) {
        return;
    }

    // 1) Navegación o acción
    if (it->action) {
        it->action(ms);
    }
    else if (it->submenu) {
        MenuSys_OpenSubMenu(ms, it->submenu);
    }

    // 2) Decidir qué renderFn usar
    if (it->screenRenderFn) {
        // pantalla “terminal” asociada al ítem
        ms->renderFn = it->screenRenderFn;
        m->lastSelectedItemIndex = -1;
        m->lastVisibleItem = -1;
    }

    // 3) Asignar callbacks del manager si aplica
    if (it->eventManagerFn) {
        ms->userEventManagerFn = it->eventManagerFn;
    }

    // 4) Limpiar y encender el flag para que OLED_MainTask lo pinte
    if (!ms->suppressClearOnNextRender && ms->clearScreen) {
        ms->clearScreen();
    }
    ms->renderFlag = true;
}
