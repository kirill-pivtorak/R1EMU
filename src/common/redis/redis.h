/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file redis.h
 * @brief Redis set of functions for communicating with the Redis server.
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/commander/commander.h"
#include "common/session/socket_session.h"
#include "common/session/game_session.h"

#define REDIS_EMPTY_STRING (uint8_t *) "__EMPTY_STRING__"
#define CHECK_REDIS_EMPTY_STRING(_x) ((_x)[0] != '\0') ? (_x) : REDIS_EMPTY_STRING

typedef struct Redis Redis;

typedef struct RedisInfo {
    char *hostname;
    int port;
} RedisInfo;

/**
 * @brief Allocate a new Redis structure.
 * @param redisInfo The information about the Redis database connection to etablish
 * @return A pointer to an allocated Redis.
 */
Redis *redisNew(RedisInfo *info);

/**
 * @brief Initialize an allocated Redis structure.
 * @param self An allocated Redis to initialize.
 * @param redisInfo The information about the Redis database connection to etablish
 * @return true on success, false otherwise.
 */
bool redisInit(Redis *self, RedisInfo *info);

/**
 * @brief Initialize an allocated RedisInfo structure.
 * @param self An allocated RedisInfo to initialize.
 * @param hostname The IP address of the Redis server
 * @param port The port of the Redis server
 * @return true on success, false otherwise.
 */
bool redisInfoInit(RedisInfo *self, char *ip, int port);

/**
 * @brief : Connect to the Redis database
 * @param self An allocated Redis instance
 * @return true on success, false otherwise
 */
bool redisConnection(Redis *self);

/**
 * @brief : Flush the Redis database
 * @param self An allocated Redis instance
 * @return true on success, false otherwise
 */
bool redisFlushDatabase(Redis *self);

/**
 * @brief Send a command to the redis server AND display it in the console.
 * @param self An allocated Redis instance
 * @param format the format of the command
 * @param ... The values of the command
 * @return A redisReply
 */
redisReply *redisCommandDbg(Redis *self, char * format, ...);

/**
 * @brief Send data to the Redis Server.
 * @param self An allocated Redis instance
 * @param socketSession An allocated socket session
 * @param ... <Key, value> couples. *Must* finishes with Key = NULL.
 * @return true on success, false otherwise
 */
bool redisSet(Redis *self, SocketSession *socketSession, ...);

/**
 * @brief Tells if any element of the redisReply array is NULL
 * @param elements Array of redisReply
 * @param nbElements Number of elements
 * @return The position if any element is NULL, -1 otherwise
 */
size_t redisAnyElementIsNull(redisReply **elements,size_t nbElements);

/**
 * @brief Dump the elements in the console
 * @param elements Array of redisReply
 * @param nbElements Number of elements
 * @param elementsName The name of elements
 */
void redisPrintElements(redisReply **elements, size_t nbElements, const char **elementsName);

/**
 * @brief Free the members of the RedisInfo structure
 * @param self A pointer to an allocated RedisInfo.
 */
void redisInfoFree(RedisInfo *self);

/**
 * @brief Free an allocated redisReply structure and nullify the content of the pointer.
 * @param self A pointer to an allocated redisReply.
 */
void redisReplyDestroy(redisReply **reply);

/**
 * @brief Free an allocated Redis structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Redis.
 */
void redisDestroy(Redis **self);
