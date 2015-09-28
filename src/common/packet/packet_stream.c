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
#include "packet_stream.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation ------

PacketStream *packetStreamNew(void *buffer) {
    PacketStream *self;

    if ((self = calloc(1, sizeof(PacketStream))) == NULL) {
        return NULL;
    }

    packetStreamInit(self, buffer);

    return self;
}

void packetStreamInit(PacketStream *self, void *buffer) {
    self->buffer = buffer;
    self->position = 0;
}

void packetStreamAppend(PacketStream *self, void *data, size_t dataSize) {
    memcpy(&self->buffer[self->position], data, dataSize);
    self->position += dataSize;
}

void *packetStreamGetCurrentBuffer(PacketStream *self) {
    return &self->buffer[self->position];
}

void packetStreamAddOffset(PacketStream *self, unsigned int offset) {
    self->position += offset;
}

void packetStreamGet(PacketStream *self, void *data, size_t dataSize) {
    memcpy(data, &self->buffer[self->position], dataSize);
    self->position += dataSize;
}

void packetStreamDestroy(PacketStream **_self) {
    PacketStream *self = *_self;

    free(self);
    *_self = NULL;
}
