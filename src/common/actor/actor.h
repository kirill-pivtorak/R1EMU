/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file actor.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "common/packet/packet_stream.h"

// ---------- Defines -------------
#define ACTOR_KEY_SIZE 17


// ------ Structure declaration -------
typedef uint8_t ActorKey[ACTOR_KEY_SIZE];
typedef uint64_t ActorId_t;

/**
 * Actor is the representation of a game actor
 */
typedef struct {
   ActorId_t uid;
} Actor;

/**
 * ActorSPacket is a server packet Actor
 */
#pragma pack(push, 1)
typedef struct {
   ActorId_t uid;
} ActorSPacket;
#pragma pack(pop)

// ----------- Functions ------------
/**
 * Allocate a new Actor structure.
 * @return A pointer to an allocated Actor, or NULL if an error occured.
 */
Actor *actorNew(ActorId_t id);

/**
 * Initialize an allocated Actor structure.
 * @param self An allocated Actor to initialize.
 * @return true on success, false otherwise.
 */
bool actorInit(Actor *self, ActorId_t id);

/**
 * Free an allocated Actor structure.
 * @param self A pointer to an allocated Actor.
 */
void actorFree(Actor *self);

/**
 * Free an allocated Actor structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Actor.
 */
void actorDestroy(Actor **self);

/**
 * Getters & Setters
 */
inline ActorId_t actorGetUId(void *_self) {
    Actor *self = _self;
    return self->uid;
}

void actorGenKey(ActorId_t uid, ActorKey key);
void actorPrint(Actor *self);

size_t actorGetSPacketSize(Actor *self);
void actorSerializeSPacket(Actor *self, PacketStream *stream);
bool actorUnserializeSPacket(Actor *self, PacketStream *stream);
