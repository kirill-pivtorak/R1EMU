/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file inventory.h
 * @brief
 *
 *
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "common/actor/item/item.h"
#include "common/actor/item/equipable/item_equipable.h"

// ---------- Defines -------------
#define ITEM_CAT_SIZE 5000

typedef enum {
    INVENTORY_TYPE_COMMANDER = 0,
    INVENTORY_TYPE_WAREHOUSE = 1,
}   InventoryType;

typedef enum {
    INVENTORY_ADD_UNK0 = 0,
    INVENTORY_ADD_PICKUP = 3,
    INVENTORY_ADD_NOT_NEW = 4,
}   InventoryAddType;

typedef enum {
    EQSLOT_HEAD_TOP = 0,
    EQSLOT_HEAD_MIDDLE,
    EQSLOT_UNKOWN1,
    EQSLOT_BODY_ARMOR,
    EQSLOT_GLOVES,
    EQSLOT_BOOTS,
    EQSLOT_HELMET,
    EQSLOT_BRACELET,
    EQSLOT_WEAPON,
    EQSLOT_SHIELD,
    EQSLOT_COSTUME,
    EQSLOT_UNKOWN3,
    EQSLOT_UNKOWN4,
    EQSLOT_UNKOWN5,
    EQSLOT_LEG_ARMOR,
    EQSLOT_UNKOWN6,
    EQSLOT_UNKOWN7,
    EQSLOT_RIGHT_LEFT,
    EQSLOT_RIGHT_RIGHT,
    EQSLOT_NECKLACE,
    EQSLOT_COUNT
} EquipmentSlot;

typedef enum {
    EMPTYEQSLOT_NoHat = 2,
    EMPTYEQSLOT_NoOuter = 4,
    EMPTYEQSLOT_NoPants = 5,
    EMPTYEQSLOT_NoGloves = 6,
    EMPTYEQSLOT_NoBoots = 7,
    EMPTYEQSLOT_NoShirt = 8,
    EMPTYEQSLOT_NoRing = 9,
    EMPTYEQSLOT_NoNeck = 10,
    EMPTYEQSLOT_NoHelmet = 10000,
    EMPTYEQSLOT_NoArmband = 11000,
    EMPTYEQSLOT_NoWeapon_THSpear = 9999983,
    EMPTYEQSLOT_NoWeapon_Wand = 9999988,
    EMPTYEQSLOT_NoWeapon_Spear = 9999989,
    EMPTYEQSLOT_NoWeapon_Sword = 9999991,
    EMPTYEQSLOT_NoWeapon_THSword = 9999992,
    EMPTYEQSLOT_NoWeapon_Shield = 9999993,
    EMPTYEQSLOT_NoWeapon_Staff = 9999994,
    EMPTYEQSLOT_NoWeapon_Bow = 9999995,
    EMPTYEQSLOT_NoWeapon = 9999996,
    EMPTYEQSLOT_NoWeapon_Shotsword = 9999997,
    EMPTYEQSLOT_NoWeapon_Mace = 9999998,
    EMPTYEQSLOT_NoWeapon_THStaff = 9999999,
} EmptyEquipmentSlot;

// ------ Structure declaration -------

typedef struct Inventory Inventory;

/**
 * @brief Inventory contains
 */
struct Inventory
{
    zhash_t *items;
    zlist_t *bags[ITEM_CAT_COUNT];
    ItemEquipable *equippedItems[EQSLOT_COUNT];
};

#define DEFINE_InventoryItemCPacket(attrSize)             \
typedef struct {                                          \
    ItemId_t itemId;                                      \
    uint16_t sizeOfAttributes;                            \
    uint16_t unkown1;                                     \
    ActorId_t itemUId;                                    \
    ItemAmount_t amount;                                  \
    ItemPrice_t price;                                    \
    ItemInventoryIndex_t inventoryIndex;                  \
    uint32_t unkown2;                                     \
    uint8_t attributes[attrSize];                         \
} InventoryItemCPacket;

// ----------- Functions ------------
/**
 * Allocate a new Inventory structure.
 * @return A pointer to an allocated Inventory, or NULL if an error occured.
 */
Inventory *inventoryNew(void);

/**
 * Initialize an allocated Inventory structure.
 * @param self An allocated Inventory to initialize.
 * @return true on success, false otherwise.
 */
bool inventoryInit(Inventory *self);

/**
 * Free an allocated Inventory structure.
 * @param self A pointer to an allocated Inventory.
 */
void inventoryFree(Inventory *self);

/**
 * Free an allocated Inventory structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Inventory.
 */
void inventoryDestroy(Inventory **self);

/**
 * Adds an item to inventory
 * @param self A pointer to an allocated Inventory.
 * @param item an Item.
 */
bool inventoryAddItem(Inventory *self, Item *itemToAdd);
bool inventoryRemoveItem(Inventory *self, Item *itemToRemove);

/**
 * Get count (size) of items in inventory
 * @param self A pointer to an allocated Inventory.
 * @return count of items in inventory
 */
size_t inventoryGetItemsCount(Inventory *self);

Item *inventoryGetFirstItem(Inventory *self, ItemCategory category);
Item *inventoryGetNextItem(Inventory *self, ItemCategory category);

bool inventoryEquipItem(Inventory *self, ActorId_t actorId, EquipmentSlot eqSlot);
void inventoryPrintEquipment(Inventory *self);
uint32_t inventoryGetEquipmentEmptySlot(EquipmentSlot slot);
void inventoryPrintBag(Inventory *self, ItemCategory category);
bool inventoryGetItemByActorId(Inventory *self, ActorId_t actorId, Item **_item);
bool inventorySwapItems(Inventory *self, Item **_item1, Item **_item2);
void itemGenActorKey(Item *self, ActorKey key);

