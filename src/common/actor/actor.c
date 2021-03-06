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

#include "actor.h"

// Inlined functions
extern inline ActorId_t actorGetUId(void *self);

Actor *actorNew(ActorId_t uid) {
    Actor *self;

    if ((self = malloc(sizeof(Actor))) == NULL) {
        return NULL;
    }

    if (!actorInit(self, uid)) {
        actorDestroy(&self);
        error("Actor failed to initialize.");
        return NULL;
    }

    return self;
}

void actorFree(Actor *self) {

}

void actorDestroy(Actor **_self) {
    Actor *self = *_self;

    if (_self && self) {
        actorFree(self);
        free(self);
        *_self = NULL;
    }
}

bool actorInit(Actor *self, ActorId_t uid) {
    memset(self, 0, sizeof(*self));
    self->uid = uid;
    return true;
}

void actorGenKey(ActorId_t uid, ActorKey key) {
    snprintf(key, ACTOR_KEY_SIZE, "%I64x", uid);
}

void actorPrint(Actor *self) {
    PRINT_STRUCTURE {
        dbg("=== Actor %p ===", self);
        dbg("UID = %llx", self->uid);
    }
}

size_t actorGetSPacketSize(Actor *self) {
    return sizeof(ActorSPacket);
}

void actorSerializeSPacket(Actor *self, PacketStream *stream) {
    packetStreamDebugStart(stream, actorGetSPacketSize(self));
    packetStreamIn(stream, &self->uid);
    packetStreamDebugEnd(stream);
}

bool actorUnserializeSPacket(Actor *self, PacketStream *stream) {

    if (!(actorInit(self, -1))) {
        error("Cannot initialize an actor to unserialize.");
        return false;
    }

    packetStreamDebugStart(stream, actorGetSPacketSize(self));
    packetStreamOut(stream, &self->uid);
    packetStreamDebugEnd(stream);

    return true;
}
