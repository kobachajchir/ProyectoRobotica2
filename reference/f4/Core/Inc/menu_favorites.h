#ifndef INC_MENU_FAVORITES_H_
#define INC_MENU_FAVORITES_H_

#include <stdbool.h>
#include <stdint.h>
#include "types/menu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FAVORITES 3

/**
 * @brief Initialize the favorites system.
 * @param ms MenuSystem instance.
 */
void MenuSys_FavoritesInit(MenuSystem *ms);

/**
 * @brief Toggle whether an item is in favorites.
 * @param item The menu item to toggle.
 * @return  1 if added, 0 if removed, -1 if not possible (full or invalid).
 */
int8_t MenuSys_ToggleFavorite(const MenuItem *item);

/**
 * @brief Check if a menu item is a favorite.
 * @param item The menu item to check.
 * @return true if it is a favorite.
 */
bool MenuSys_IsFavorite(const MenuItem *item);

/**
 * @brief Get the Favorites SubMenu pointer.
 * @return SubMenu pointer for the favorites screen.
 */
SubMenu* MenuSys_GetFavoritesSubMenu(void);

/**
 * @brief Checks if the given menu item is valid to be a favorite.
 *        Items like "Volver" are excluded.
 */
bool MenuSys_CanBeFavorite(const MenuItem *item);

/**
 * @brief Check if the given submenu is the favorites submenu.
 */
bool MenuSys_IsFavoritesMenu(const SubMenu *menu);

#ifdef __cplusplus
}
#endif

#endif /* INC_MENU_FAVORITES_H_ */
