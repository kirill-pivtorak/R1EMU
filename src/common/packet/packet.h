/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file packet.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "packet_type.h"
#include "common/crypto/bf/blowfish.h"

#define PACKET_HEADER(x) (typeof(x)[]){x}

#define BUILD_REPLY_PACKET(packetName, msgName)                                             \
    for (bool __sent = false;                                                               \
         !__sent && memset(&packetName, 0, sizeof(packetName));                             \
         zmsg_add(msgName, zframe_new(&packetName, sizeof(packetName))), __sent = true     \
    )

#define CHECK_SERVER_PACKET_SIZE(packet, packetType)                                        \
    do {                                                                                    \
        if (packetTypeInfo.packets[packetType].size != 0                                    \
         && sizeof(packet) != packetTypeInfo.packets[packetType].size) {                    \
            error("The packet size sent isn't the equal to the one in packet_type.h");      \
            error("The packet size is %d bytes. The waited size is %d bytes.",              \
                sizeof(packet), packetTypeInfo.packets[packetType].size);                   \
        }                                                                                   \
    } while (0);

#define CHECK_CLIENT_PACKET_SIZE(packet, packetSize, packetType)                                    \
    do {                                                                                            \
        size_t __clientPacketSize = packetTypeInfo.packets[packetType].size                         \
                                  - sizeof(CPacketHeader);                                     \
        if (packetTypeInfo.packets[packetType].size != 0                                            \
         && sizeof(packet) != __clientPacketSize) {                                                 \
            error("The packet size sent isn't the equal to the one in packet_type.h");              \
            error("The packet size is %d bytes. The waited size is %d bytes.",                      \
                sizeof(packet), __clientPacketSize);                                                \
                                                                                                    \
                return PACKET_HANDLER_ERROR;                                                        \
        }                                                                                           \
        if (packetSize > (sizeof(packet) + BF_BLOCK) || packetSize < sizeof(packet) ) {             \
            error("The packet size received isn't correct. (packet size = %d, correct size = %d)",  \
                packetSize, sizeof(packet));                                                        \
            return PACKET_HANDLER_ERROR;                                                            \
        }                                                                                           \
    } while (0)

/**
 * @brief CPacketHeader is the header of each packet sent by the client.
 */
#pragma pack(push, 1)
typedef struct CPacketHeader {
    uint16_t type;
    uint32_t sequence;
    uint32_t checksum;
} CPacketHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct CryptPacketHeader {
    uint16_t plainSize;
} CryptPacketHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct ServerPacketHeader {
    uint16_t type;
    uint32_t reserved;
} ServerPacketHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VariableSizePacketHeader {
    ServerPacketHeader serverHeader;
    uint16_t packetSize;
} VariableSizePacketHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct PacketNormalHeader {
    VariableSizePacketHeader variableSizeHeader;
    uint32_t subtype;
} PacketNormalHeader;
#pragma pack(pop)

/**
 * @brief Unwrap the client packet header and stores it in the CPacketHeader argument
 * @param[in,out] packet A pointer to the packet to unwrap. Afterward, it points to the data of the packet.
 * @param[in,out] packetSize A pointer to the packetSize. Afterward, it is updated.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void clientPacketUnwrapHeader(uint8_t **packet,size_t *packetSize, CPacketHeader *header, bool isCrypted);

/**
 * @brief Unwrap the crypt packet header and stores it in the header argument
 * @param[in,out] packet A pointer to the packet to unwrap. Afterward, it points to the data of the packet.
 * @param[in,out] packetSize A pointer to the packetSize. Afterward, it is updated.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void cryptPacketUnwrapHeader(uint8_t **packet, size_t *packetSize, CryptPacketHeader *header);

/**
 * @brief Get the crypt packet header and stores it in the header argument
 * @param[in] packet A pointer to the packet to decode.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void cryptPacketGetHeader(uint8_t *packet, CryptPacketHeader *header);

/**
 * @brief Creates a server header
 * @param[out] variableSizeHeader An allocated server header
 * @param[in] packetSize The type of the packet
 * @return
 */
void serverPacketHeaderInit(ServerPacketHeader *serverHeader, uint16_t packetType);

/**
 * @brief Creates a variable size header
 * @param[out] variableSizeHeader An allocated variable size header
 * @param[in] packetSize The type of the packet
 * @param[in] packetSize The total size of the packet
 * @return
 */
void variableSizePacketHeaderInit(
    VariableSizePacketHeader *variableSizeHeader,
    uint16_t packetType,
    uint32_t packetSize);

/**
 * @brief Creates a normal header based on the subtype argument
 * @param[out] normalHeader An allocated normal header
 * @param[in] subtype The subtype of the normal barrack packet
 * @param[in] packetSize The total size of the packet
 * @return
 */
void packetNormalHeaderInit(PacketNormalHeader *normalHeader, uint32_t subtype, uint32_t packetSize);
