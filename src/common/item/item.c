/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */


// ---------- Includes ------------
#include "item.h"

// ------ Structure declaration -------

// ------ Static declaration -------

// ------ Extern function implementation -------

Item *itemNew(uint64_t itemId, uint32_t itemType, uint32_t amount, uint32_t inventoryIndex) {

    Item *self;

    if ((self = calloc(1, sizeof(Item))) == NULL) {
        return NULL;
    }

    if (!itemInit(self, itemId, itemType, amount, inventoryIndex)) {
        itemDestroy(&self);
        error("Item failed to initialize.");
        return NULL;
    }

    return self;
}

bool itemInit(Item *self, uint64_t itemId, uint32_t itemType, uint32_t amount, uint32_t inventoryIndex) {

    self->itemId = itemId;
    self->itemType = itemType;
    self->amount = amount;
    self->inventoryIndex = inventoryIndex;

    if (!(itemAttributesInit(&self->attributes))) {
        error("Cannot initialize item attributes.");
        return false;
    }

    return true;
}

bool itemAddAttribute(Item *self, ItemAttributeId itemAttrId, void *value) {

    if (!(itemAttributesAdd(&self->attributes, itemAttrId, value))) {
        error("Cannot add item attribute.");
        return false;
    }

    return true;
}

bool itemGetAttribute(Item *self, ItemAttributeId itemAttrId, void **value) {

    if (!(itemAttributesGet(&self->attributes, itemAttrId, value))) {
        error("Cannot get item attribute.");
        return false;
    }

    return true;
}

bool itemUpdateAttribute(Item *self, ItemAttributeId itemAttrId, void *value) {

    if (!(itemAttributesUpdate(&self->attributes, itemAttrId, value))) {
        error("Cannot update item attribute.");
        return false;
    }

    return true;
}

bool itemRemoveAttribute(Item *self, ItemAttributeId itemAttrId) {

    if (!(itemAttributesRemove(&self->attributes, itemAttrId))) {
        error("Cannot delete item attribute.");
        return false;
    }

    return true;
}

void itemGenKey(uint64_t itemIdKey, ItemKey itemKey) {
    snprintf(itemKey, ITEM_KEY_SIZE, "%I64x", itemIdKey);
}

void itemFree(Item *self) {
    itemAttributesFree(&self->attributes);
}

void itemDestroy(Item **_self) {

    Item *self = *_self;

    if (_self && self) {
        itemFree(self);
        free(self);
        *_self = NULL;
    }
}

size_t itemGetSPacketSize(Item *self) {
    size_t packetSize = 0;

    packetSize += sizeof(ItemSPacket);
    packetSize += itemAttributesGetSPacketSize(&self->attributes);

    return packetSize;
}

void itemSPacket(Item *self, PacketStream *stream) {
    packetStreamIn(stream, &self->itemId);
    packetStreamIn(stream, &self->itemType);
    packetStreamIn(stream, &self->amount);
    packetStreamIn(stream, &self->inventoryIndex);
    packetStreamIn(stream, &self->itemCategory);
    itemAttributesSPacket(&self->attributes, stream);
}
