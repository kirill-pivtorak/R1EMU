/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zone_builder.h
 * @brief ZoneBuilders build packets generically based on given arguments.
 *        They are called from ZoneHandlers.
 *        They don't communicate with other entities on the network.
 *        They never fail.
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"
#include "common/commander/commander.h"
#include "common/commander/inventory.h"
#include "common/actor/item/item.h"
#include "common/actor/skill/skill.h"
#include "common/actor/npc/npc.h"

typedef enum PacketTypeZoneNormal {
    ZC_NORMAL_UNKNOWN_1 = 0x11,
    ZC_NORMAL_UNKNOWN_2 = 0x18,
    ZC_NORMAL_UNKNOWN_3 = 0x10,
    ZC_NORMAL_UNKNOWN_4 = 0x2C,
    ZC_NORMAL_UNKNOWN_5 = 0x1D,
} PacketTypeZoneNormal;

/**
 * @brief Sit a target PC
 * @param targetPcId The PC ID of the target entity who sit
 */
void zoneBuilderRestSit(PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief Inform that a skill is ready
 * @param targetPcId Actor of the skill
 * @param skillId Skill ID
 * @param pos1
 * @param pos2
 */
void zoneBuilderSkillReady(
    PcId_t targetPcId,
    SkillId_t skillId,
    PositionXYZ *pos1,
    PositionXYZ *pos2,
    zmsg_t *replyMsg);

/**
 * @brief Play an animation
 */
void zoneBuilderPlayAni(zmsg_t *replyMsg);

/**
 * @brief Cast a given skill at a given position from a given PC ID
 */
void zoneBuilderSkillCast(
    PcId_t targetPcId,
    SkillId_t skillId,
    PositionXYZ *position1,
    PositionXYZ *position2,
    zmsg_t *replyMsg);

/**
 * @brief Play the cast animation of a given skill at a given position from a given PC ID
 */
void zoneBuilderPlaySkillCastAni(
    PcId_t targetPcId,
    PositionXYZ *position,
    zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk8(PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk9( PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk10_56(PcId_t targetPcId, SkillId_t skillId, PositionXYZ *position, PositionXZ *direction, bool enableSkill, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk11_1c(PcId_t targetPcId, PositionXYZ *position, PositionXZ *direction, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk12_60(ActorId_t actorId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk13_85(ActorId_t actorId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderPartyList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderPartyInfo(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderJobPts(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk6(char *commanderName, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk7(
    AccountId_t accountId,
    PcId_t targetPcId,
    char *familyName,
    char *commanderName,
    zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderFaction(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderEnterMonster(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderBuffList(PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief Makes a playable character appear in the screen.
 */
void zoneBuilderEnterPc(Commander *commander, zmsg_t *replyMsg);

/**
 * @brief Makes a playable character disappear in the screen.
 */
void zoneBuilderLeave(PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief Add a skill in the commander skill list.
 */
void zoneBuilderSkillAdd(SkillId_t skillId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderLoginTime(zmsg_t *replyMsg);

/**
 * @brief Sets the stamina to value of stamina. Note: Stamina in game is
 *        divided. 25 stamina ingame means stamina=25000.
 */
void zoneBuilderStamina(Stamina_t stamina, zmsg_t *replyMsg);

/**
 * @brief Sets the stamina to value of stamina. Note: Stamina in game is
 *        divided. I'm not sure of this packets exact functionality, but
 *        this one will not change the stamina in the client. In captures
 *        it is sent exactly at the same time with same data as ZC_STAMINA.
 */
void zoneBuilderAddStamina(Stamina_t stamina, zmsg_t *replyMsg);

/**
 * @brief Updates sp of character to value of sp.
 */
void zoneBuilderUpdateSP(PcId_t targetPcId, Sp_t sp, zmsg_t *replyMsg);

/**
 * @brief Updates sp of character to value of sp.
 */
void zoneBuilderPCLevelUp(PcId_t targetPcId, CommanderLevel_t level, zmsg_t *replyMsg);

/**
 * @brief Updates hp/maxhp/sp/maxsp of character.
 */
void zoneBuilderUpdateAllStatus(
    PcId_t targetPcId,
    Hp_t currentHp, Hp_t maxHp,
    Sp_t currentSp, Sp_t maxSp,
    zmsg_t *replyMsg);

/**
 * @brief Updates hp of character. Does not seem to show in client. This packet seems to be
 *        sent during HP regen and when the client starts the game.
 */
void zoneBuilderAddHp(PcId_t targetPcId, uint32_t hp, uint32_t maxHp, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderObjectProperty(ActorId_t actorId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderAddonMsg(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderCampInfo(uint64_t accountId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk5(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk4(SocialInfoId_t socialInfoId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk3(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk2(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNormalUnk1(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderQuickSlotList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderItemEquipList(Inventory *inventory, zmsg_t *replyMsg);

/**
 * @brief Send information about Jobs
 */
void zoneBuilderStartInfo(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderAbilityList(PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderSkillList(PcId_t targetPcId, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderGuestPageMap(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderMyPageMap(zmsg_t *replyMsg);

/**
 * @brief Send information about the UI
 */
void zoneBuilderUiInfoList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderNpcStateList(zmsg_t *replyMsg);

/**
 * @brief Alert the client that its own commander arrived in the game at a given position
 */
void zoneBuilderMyPCEnter(PositionXYZ *position, zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderChatMacroList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderHelpList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderAchievePointList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderSkillmapList(zmsg_t *replyMsg);

/**
 * @brief Send information about item inventory list
 */
void zoneBuilderItemInventoryList(Inventory *inventory, zmsg_t *replyMsg);

/**
 * @brief Send a target commander movement speed
 */
void zoneBuilderMoveSpeed(PcId_t targetPcId, float movementSpeed,zmsg_t *replyMsg);

/**
 * @brief Connect to the zone server
 */
void zoneBuilderConnectOk(
    uint8_t gameMode,
    uint8_t accountPrivileges,
    Commander *commander,
    zmsg_t *replyMsg);

/**
 * Makes a target PC jump at a given height
 */
void zoneBuilderJump(PcId_t targetPcId, float height, zmsg_t *replyMsg);

/**
 * Makes a normal chat message
 */
void zoneBuilderChat(Commander *commander, uint8_t *chatText, zmsg_t *replyMsg);

/**
 * @brief @unknown Contains information about quest position?
 */
void zoneBuilderSessionObjects(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderOptionList(zmsg_t *replyMsg);

/**
 * @brief @unknown
 */
void zoneBuilderCooldownList(SocialInfoId_t socialInfoId, zmsg_t *replyMsg);

/**
 * @brief Set the position of a commander
 */
void zoneBuilderSetPos(PcId_t targetPcId, PositionXYZ *position, zmsg_t *replyMsg);

/**
 * @brief Move a given commander to a given position
 */
void zoneBuilderMoveDir(
    PcId_t targetPcId,
    PositionXYZ *position,
    PositionXZ *direction,
    float timestamp,
    zmsg_t *replyMsg);

/**
 * @brief After the loading screen, start the game for the client
 */
void zoneBuilderStartGame(
    float timeMultiplier,
    float serverAppTimeOffset,
    float globalAppTimeOffset,
    double serverDateTime,
    zmsg_t *replyMsg);

/**
 * @brief Request a PC to stop moving
 */
void zoneBuilderPcMoveStop(
    PcId_t targetPcId,
    PositionXYZ *position,
    PositionXZ *direction,
    float timestamp,
    zmsg_t *replyMsg);

/**
 * @brief Add an item in the current commander inventory
 */
void zoneBuilderItemAdd(Item *item, ItemInventoryIndex_t inventoryIndex, InventoryAddType addType, zmsg_t *replyMsg);

/**
 * @brief Rotate head
 */
void zoneBuilderRotateHead(PcId_t pcId, PositionXZ *pos, zmsg_t *replyMsg);

/**
 * @brief Rotate body
 */
void zoneBuilderRotate(PcId_t pcId, PositionXZ *pos, zmsg_t *replyMsg);

/**
 * @brief Logout
 */
void zoneBuilderLogout(zmsg_t *replyMsg);

/**
 * @brief Change the camera
 */
void zoneBuilderChangeCamera(uint8_t mode, PositionXYZ *pos, float fspd, float ispd, zmsg_t *replyMsg);

/**
 * Makes an animation
 */
void zoneBuilderPose(PcId_t pcId, CommanderPose_t poseId, PositionXYZ *pos, PositionXZ *dir, zmsg_t *replyMsg);

/**
 * Remove an item from a commander
 */
void zoneBuilderItemRemove(Item *item, InventoryRemoval removalType, InventoryType inventoryType, zmsg_t *replyMsg);

/**
 * Set commander's Job Points
 */
void zoneBuilderJobPoints(CommanderJobId_t jobId, CommanderJobPoints_t points, zmsg_t *replyMsg);

void zoneBuilderSkillRangeFan(PcId_t pcId, PositionXYZ *position, PositionXZ *direction, zmsg_t *replyMsg);
void zoneBuilderSkillRangeSquare(PcId_t pcId, SkillId_t skillId, PositionXYZ *pos1, PositionXYZ *pos2, zmsg_t *replyMsg);
void zoneBuilderSkillMeleeGround(PcId_t pcId, SkillId_t skillId, PositionXYZ *position, PositionXZ *direction, zmsg_t *replyMsg);
void zoneBuilderBuffAdd(PcId_t pcId, Commander *commander, zmsg_t *replyMsg);
void zoneBuilderHealInfo(PcId_t pcId, uint32_t amountHealed, uint32_t totalHP, zmsg_t *replyMsg);
void zoneBuilderNormalUnk14_4c(PcId_t pcId, SkillId_t skillId, zmsg_t *replyMsg);
