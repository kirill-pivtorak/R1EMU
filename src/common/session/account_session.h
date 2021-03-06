/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file account_session.h
 * @brief
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/session/socket_session.h"
#include "common/commander/commander.h"

#define ACCOUNT_SESSION_ACCOUNT_NAME_MAXSIZE 33

/**
 * @brief AccountSessionPrivileges enumerates the different levels of privileges
 *  for an account.
 */
typedef uint8_t AccountSessionPrivileges_t;
typedef enum AccountSessionPrivileges {
    ACCOUNT_SESSION_PRIVILEGES_ADMIN   = 0,
    ACCOUNT_SESSION_PRIVILEGES_UNKNOWN = 1,
    ACCOUNT_SESSION_PRIVILEGES_GM      = 2,
    ACCOUNT_SESSION_PRIVILEGES_PLAYER  = 3
}   AccountSessionPrivileges;

typedef struct AccountSession {
    // The account login
    uint8_t accountName[ACCOUNT_SESSION_ACCOUNT_NAME_MAXSIZE];

    // Session key
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE];

    // Account privilege level
    AccountSessionPrivileges privilege;

    uint64_t accountId;
    bool isBanned;
    time_t timeBanned;
    float credits;
    time_t timeLastLogin;
    uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];
    uint32_t barrackType;

    // Array of commanders in the barrack
    Commander **commanders;
    size_t commandersCountMax;
} AccountSession;

typedef struct {
    // The account login
    uint8_t accountName[ACCOUNT_SESSION_ACCOUNT_NAME_MAXSIZE];
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE];
    uint8_t familyName[COMMANDER_FAMILY_NAME_SIZE];

    // Account privilege level
    AccountSessionPrivileges privilege;

    uint64_t accountId;
    bool isBanned;
    time_t timeBanned;
    float credits;
    time_t timeLastLogin;
    uint32_t barrackType;

    // Array of commanders in the barrack
    size_t commandersCount;
    CommanderSPacket commanders[0];
} AccountSessionSPacket;

/**
 * @brief Allocate a new AccountSession structure.
 * @return A pointer to an allocated AccountSession.
 */
AccountSession *accountSessionNew(
    uint8_t *accountName,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege);

/**
 * @brief Initialize an allocated AccountSession structure.
 * @param self An allocated AccountSession to initialize.
 * @return true on success, false otherwise.
 */
bool accountSessionInit(
    AccountSession *self,
    uint8_t *accountName,
    uint8_t *socketId,
    AccountSessionPrivileges accountPrivilege);

/**
 * @brief Get the number of commanders in the current account session
 * @return A pointer to an initialized AccountSession.
 */
size_t accountSessionGetCommandersCount(AccountSession *self);

/**
 * @brief Get a commander by its array index
 * @return true on success, false otherwise
 */
bool accountSessionGetCommanderByIndex(AccountSession *self, int index, Commander **commander);

/**
 * @brief Initialize commanders in the session
 * @return true on success, false otherwise
 */
bool accountSessionCommandersInit(AccountSession *self, size_t commandersCountMax, size_t commandersCount);

/**
 * @brief Prints a AccountSession structure.
 */
void accountSessionPrint(AccountSession *self);

/**
 * @brief Free an allocated AccountSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated AccountSession.
 */
void accountSessionDestroy(AccountSession **self);

/**
 * @brief Free an allocated AccountSession structure
 * @param self A pointer to an allocated AccountSession.
 */
void accountSessionFree(AccountSession *self);

/**
 * @brief Check if a given slot index is empty in account commanders
 * @param self A pointer to an allocated AccountSession.
 * @param commanderIndex the slot index to look into
 */
bool accountSessionIsCommanderSlotEmpty(AccountSession *self, int commanderIndex);
