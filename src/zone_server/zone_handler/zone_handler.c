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

#include "zone_handler.h"
#include "zone_builder.h"
#include "zone_event_handler.h"
#include "admin_cmd.h"
#include "common/packet/packet.h"
#include "common/redis/fields/redis_game_session.h"
#include "common/redis/fields/redis_socket_session.h"
#include "common/server/event_handler.h"
#include "common/commander/inventory.h"
#include "common/mysql/fields/mysql_commander.h"
#include "common/mysql/fields/mysql_account_session.h"
#include "common/actor/item/item_factory.h"

/** Connect to the zone server */
static PacketHandlerState zoneHandlerConnect        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** Client is ready to enter the zone */
static PacketHandlerState zoneHandlerGameReady      (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** Jump handler */
static PacketHandlerState zoneHandlerJump           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On air handler */
static PacketHandlerState zoneHandlerOnAir          (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On ground handler */
static PacketHandlerState zoneHandlerOnGround       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On move with keyboard handler */
static PacketHandlerState zoneHandlerKeyboardMove   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On stop commander movement */
static PacketHandlerState zoneHandlerMoveStop       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On movement information */
static PacketHandlerState zoneHandlerMovementInfo   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** @unknown */
static PacketHandlerState zoneHandlerCampInfo       (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerCzQuickSlotList(Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On change config */
static PacketHandlerState zoneHandlerChangeConfig   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerItemUse        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerINeedParty     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On log out */
static PacketHandlerState zoneHandlerLogout         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On cast a spell on the ground */
static PacketHandlerState zoneHandlerSkillGround    (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander sit */
static PacketHandlerState zoneHandlerRestSit        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander chat */
static PacketHandlerState zoneHandlerChat           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander chat log */
static PacketHandlerState zoneHandlerChatLog        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander head rotation */
static PacketHandlerState zoneHandlerHeadRotate     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander rotation */
static PacketHandlerState zoneHandlerRotate         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander pose */
static PacketHandlerState zoneHandlerPose           (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander dash run */
static PacketHandlerState zoneHandlerDashRun        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On delete an item from inventory/warehouse */
static PacketHandlerState zoneHandlerItemDelete        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander delete item */
static PacketHandlerState zoneHandlerSwapEtcInvChangeIndex        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander learn skills */
static PacketHandlerState zoneHandlerReqNormalTxNumArg        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);
/** On commander execute dynamic skills */
static PacketHandlerState zoneHandlerDynamicCastingStart        (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *replyMsg);

/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers[PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
       [packetName] = {handler, STRINGIFY(packetName)}

    REGISTER_PACKET_HANDLER(CZ_CONNECT, zoneHandlerConnect),
    REGISTER_PACKET_HANDLER(CZ_GAME_READY, zoneHandlerGameReady),
    REGISTER_PACKET_HANDLER(CZ_JUMP, zoneHandlerJump),
    REGISTER_PACKET_HANDLER(CZ_ON_AIR, zoneHandlerOnAir),
    REGISTER_PACKET_HANDLER(CZ_ON_GROUND, zoneHandlerOnGround),
    REGISTER_PACKET_HANDLER(CZ_KEYBOARD_MOVE, zoneHandlerKeyboardMove),
    REGISTER_PACKET_HANDLER(CZ_MOVE_STOP, zoneHandlerMoveStop),
    REGISTER_PACKET_HANDLER(CZ_MOVEMENT_INFO, zoneHandlerMovementInfo),
    REGISTER_PACKET_HANDLER(CZ_CAMPINFO, zoneHandlerCampInfo),
    REGISTER_PACKET_HANDLER(CZ_QUICKSLOT_LIST, zoneHandlerCzQuickSlotList),
    REGISTER_PACKET_HANDLER(CZ_CHANGE_CONFIG, zoneHandlerChangeConfig),
    REGISTER_PACKET_HANDLER(CZ_LOGOUT, zoneHandlerLogout),
    REGISTER_PACKET_HANDLER(CZ_ITEM_USE, zoneHandlerItemUse),
    REGISTER_PACKET_HANDLER(CZ_I_NEED_PARTY, zoneHandlerINeedParty),
    REGISTER_PACKET_HANDLER(CZ_SKILL_GROUND, zoneHandlerSkillGround),
    REGISTER_PACKET_HANDLER(CZ_REST_SIT, zoneHandlerRestSit),
    REGISTER_PACKET_HANDLER(CZ_CHAT, zoneHandlerChat),
    REGISTER_PACKET_HANDLER(CZ_CHAT_LOG, zoneHandlerChatLog),
    REGISTER_PACKET_HANDLER(CZ_HEAD_ROTATE, zoneHandlerHeadRotate),
    REGISTER_PACKET_HANDLER(CZ_ROTATE, zoneHandlerRotate),
    REGISTER_PACKET_HANDLER(CZ_POSE, zoneHandlerPose),
    REGISTER_PACKET_HANDLER(CZ_DASHRUN, zoneHandlerDashRun),
    REGISTER_PACKET_HANDLER(CZ_ITEM_DELETE, zoneHandlerItemDelete),
    REGISTER_PACKET_HANDLER(CZ_SWAP_ETC_INV_CHANGE_INDEX, zoneHandlerSwapEtcInvChangeIndex),
    REGISTER_PACKET_HANDLER(CZ_REQ_NORMAL_TX_NUMARG, zoneHandlerReqNormalTxNumArg),
    REGISTER_PACKET_HANDLER(CZ_DYNAMIC_CASTING_START, zoneHandlerDynamicCastingStart),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState zoneHandlerChatLog(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning ("Not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerChat(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    // the first 2 bytes of ZC_CHAT are the packet size
    size_t msgSize = *((uint16_t *) packet) - sizeof(ClientPacketHeader) - sizeof(uint16_t);

    #pragma pack(push, 1)
    struct {
        uint16_t msgSize;
        uint8_t msgText[msgSize];
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_CHAT);

    // The chat message sent by the client should always finish by a null byte
    clientPacket->msgText[msgSize - 1] = '\0';

    // Check for custom admin commands
    if (session->game.accountSession.privilege <= ACCOUNT_SESSION_PRIVILEGES_ADMIN
    && (strncmp(clientPacket->msgText, "/", strlen("/")) == 0)) {
        if (adminCmdProcess(self, clientPacket->msgText + strlen("/"), session, replyMsg)) {
            // OK
            return PACKET_HANDLER_UPDATE_SESSION;
        }
    }

    // normal message : Dispatch a GameEventChat
    DECLARE_GameEventChat(msgSize);
    GameEventChat event;
    memcpy(&event.commander, session->game.commanderSession.currentCommander, sizeof(event.commander));
    memcpy(event.chatText, clientPacket->msgText, msgSize);
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_CHAT, &event, sizeof(event));


    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState zoneHandlerRestSit(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        // This packet is actually empty
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_REST_SIT);

    // notify the players around
    GameEventRestSit event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_REST_SIT, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerSkillGround(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        SkillId_t skillId;
        uint32_t unk2;
        PositionXYZ pos1;
        PositionXYZ pos2;
        PositionXZ direction;
        uint32_t unk5;
        uint8_t unk6;
        uint8_t unk7;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    dbg("unk1 %d", clientPacket->unk1);
    dbg("skillId %d", clientPacket->skillId);
    dbg("unk2 %d", clientPacket->unk2);
    dbg("POS1 [%f] [%f] [%f]", clientPacket->pos1.x, clientPacket->pos1.y, clientPacket->pos1.z);
    dbg("POS2 [%f] [%f] [%f]", clientPacket->pos2.x, clientPacket->pos2.y, clientPacket->pos2.z);
    dbg("DIRECTION [%f] [%f]", clientPacket->direction.x, clientPacket->direction.z);
    dbg("unk5 %d", clientPacket->unk5);
    dbg("unk6 %d", clientPacket->unk6);
    dbg("unk7 %d", clientPacket->unk7);

    special("Current pos : ");
    positionXYZDump(&session->game.commanderSession.currentCommander->pos);

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_SKILL_GROUND);

    /*   CzSkillGroundPacket :
         u1 skillId  unk2     x        y        z        x2       y2       z2       u3       u4       u5       u6 u7
         00 419C0000 00000000 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3 FFFF7FBF 0020B539 00000000 00 01
         00 419C0000 00000000 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3 F30435BF F40435BF 00000000 00 01
         00 419C0000 00000000 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4 FFFF7F3F 0020B5B9 00000000 00 00
         00 429C0000 00000000 5A00FAC3 1F7CA143 B1D3E843 5A00FAC3 1F7CA143 B1D3E843 EE04353F F60435BF 00000000 00 00
         00 439C0000 00000000 1E43FFC3 1F7CA143 E130D443 1E43FFC3 1F7CA143 E130D443 EF04353F F80435BF 00000000 00 00
    */

    //zoneBuilderPlayAni(replyMsg);

    /*
    zoneBuilderNormalUnk8(
        session->game.commanderSession.currentCommander->pcId,
        replyMsg
    );
    */

    zoneBuilderNormalUnk10_56(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &clientPacket->pos1,
        &clientPacket->direction,
        false,
        replyMsg
    );

    // Melstis !
    // clientPacket->skillId = 40102;

    // Skill is ready to be casted
    zoneBuilderSkillReady(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &clientPacket->pos1,
        &clientPacket->pos2,
        replyMsg
    );

    PositionXYZ skillPos = clientPacket->pos1;
    skillPos.x += clientPacket->direction.x * 20; // 20 = tile size
    skillPos.z += clientPacket->direction.z * 20;

    dbg("skillPos: %f, %f, %f", skillPos.x, skillPos.y, skillPos.z);

    // Make the skill appear
    zoneBuilderNormalUnk10_56(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &skillPos,
        &clientPacket->direction,
        true,
        replyMsg
    );

    // Unknown
    zoneBuilderNormalUnk11_1c(
        session->game.commanderSession.currentCommander->pcId,
        &clientPacket->pos1,
        &clientPacket->direction,
        replyMsg
    );

    // Set range of effect for this skill? (cone)
    zoneBuilderSkillRangeFan(
        session->game.commanderSession.currentCommander->pcId,
        &clientPacket->pos1,
        &clientPacket->direction,
        replyMsg
    );

    // Dont know.. everything works fine without this packet anyway.
    zoneBuilderSkillMeleeGround(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &clientPacket->pos1,
        &clientPacket->direction,
        replyMsg
    );

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerCampInfo(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        AccountId_t accountId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_CAMPINFO);

    zoneBuilderCampInfo(session->socket.accountId, replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerItemUse(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_ITEM_USE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerINeedParty(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_I_NEED_PARTY not implemented yet.");
    // ZoneBuilder_partyInfo(replyMsg);
    // ZoneBuilder_partyList(replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerLogout(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_LOGOUT);
    zoneBuilderLogout(replyMsg);

    // notify leave to clients
    GameEventLeave event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_LEAVE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerMovementInfo(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg) {
    warning("CZ_MOVEMENT_INFO not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerMoveStop(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        PositionXYZ position;
        PositionXZ direction;
        float timestamp;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_MOVE_STOP);

    /*  u1 posX     posY     posZ     dirX     dirZ     time
        00 36E203C4 74768243 826F39C4 FFFFFF3F 00000000 D0651543
        00 22F402C4 74768243 826F39C4 FFFFFF3F 00000000 60941543
        00 22F402C4 74768243 4EFC3AC4 00000000 FFFF7FBF 7EF21543
        00 22F402C4 74768243 BE393CC4 00000000 FFFF7FBF B4321643
        00 22F402C4 74768243 8AC63DC4 00000000 FFFF7FBF 526A1643
        00 22F402C4 74768243 62EA3BC4 00000000 FFFF7F3F 51B71643
        00 22F402C4 74768243 F2AC3AC4 00000000 FFFF7F3F 5DE61643
        00 22F402C4 74768243 826F39C4 00000000 FFFF7F3F 74151743
        00 22F402C4 74768243 123238C4 00000000 FFFF7F3F 42401743
        00 22F402C4 74768243 FE4337C4 00000000 FFFF7F3F 0A6B1743
        00 22F402C4 74768243 8E0636C4 00000000 FFFF7F3F D3951743
    */

    // Notify the players around
    GameEventMoveStop event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *session->game.commanderSession.currentCommander,
        },
        .position = clientPacket->position,
        .direction = clientPacket->direction,
        .timestamp = clientPacket->timestamp,
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_MOVE_STOP, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerKeyboardMove(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        PositionXYZ position;
        PositionXZ direction;
        uint8_t unk7;
        float movementSpeed;
        uint8_t unk8;
        float timestamp;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    /*  u1 posX     posY     posZ     dirX     dirZ     u7 mspeed   u8 time
        00 18190DC4 B663A143 17520644 00000000 FFFF7FBF 01 0000F841 01 7B2C5C44
        00 D4CE1AC4 74768243 4CEF7EC4 F30435BF F204353F 01 00000000 00 5E16FA42
        00 103F1BC4 74768243 107F7EC4 F204353F F304353F 01 00000000 00 3141FA42
        00 5CEE19C4 74768243 5C2E7DC4 1D566733 FFFF7F3F 01 00000000 00 8174FA42
        00 8C5112C4 74768243 78A374C4 FFFF7F3F 00000000 01 00000000 00 727CC244
    */
    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_KEYBOARD_MOVE);

    // TODO : Check coordinates

    // update session
    session->game.commanderSession.currentCommander->pos = clientPacket->position;
    session->game.commanderSession.currentCommander->dir = clientPacket->direction;

    // notify the players around
    GameEventCommanderMove event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *session->game.commanderSession.currentCommander
        },
        .position = clientPacket->position,
        .direction = clientPacket->direction,
        .timestamp = clientPacket->timestamp,
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_COMMANDER_MOVE, &event, sizeof(event));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState zoneHandlerCzQuickSlotList(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_QUICKSLOT_LIST not implemented yet.");
    // Answer PacketType : ZC_QUICKSLOT_REGISTER
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerChangeConfig(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_CHANGE_CONFIG not implemented yet.");

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerGameReady(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_GAME_READY);
    Commander *commander = session->game.commanderSession.currentCommander;

    zoneBuilderSessionObjects(replyMsg);
    zoneBuilderOptionList(replyMsg);
    zoneBuilderSkillmapList(replyMsg);
    zoneBuilderAchievePointList(replyMsg);
    zoneBuilderChatMacroList(replyMsg);
    zoneBuilderUiInfoList(replyMsg);
    zoneBuilderNpcStateList(replyMsg);

    zoneBuilderHelpList(replyMsg);
    zoneBuilderMyPageMap(replyMsg);
    zoneBuilderGuestPageMap(replyMsg);
    zoneBuilderStartInfo(replyMsg);

    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;
    zoneBuilderItemEquipList(inventory, replyMsg);
    zoneBuilderItemInventoryList(inventory, replyMsg);

    zoneBuilderSkillList(commander->pcId, replyMsg);
    zoneBuilderAbilityList(commander->pcId, replyMsg);
    zoneBuilderCooldownList(commander->socialInfoId, replyMsg);

    zoneBuilderQuickSlotList(replyMsg);

    zoneBuilderNormalUnk1(replyMsg);
    zoneBuilderNormalUnk2(replyMsg);
    zoneBuilderNormalUnk3(replyMsg);
    zoneBuilderNormalUnk4(commander->socialInfoId, replyMsg);
    zoneBuilderNormalUnk5(replyMsg);

    zoneBuilderStartGame(1.0, 0.0, 0.0, 0.0, replyMsg);
    zoneBuilderObjectProperty(commander->socialInfoId, replyMsg);
    // zoneBuilderStamina(replyMsg);
    zoneBuilderLoginTime(replyMsg);

    zoneBuilderMyPCEnter(&commander->pos, replyMsg);
    // zoneBuilderSkillAdd(replyMsg);

    // Notify players around that a new PC has entered
    GameEventEnterPc pcEnterEvent = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *commander
        }
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_ENTER_PC, &pcEnterEvent, sizeof(pcEnterEvent));
    // zoneBuilderEnterPc(&pcEnterEvent.updatePosEvent.info, replyMsg);

    // zoneBuilderBuffList(commander->appearance.pcId, replyMsg);

    // add NPC at the start screen
    zoneBuilderEnterMonster(replyMsg);
    zoneBuilderFaction(replyMsg);

    /*
    zoneBuilderNormalUnk6(
        commander->appearance.commanderName,
        replyMsg
    );

    zoneBuilderNormalUnk7(
        session->socket.accountId,
        session->game.commanderSession.currentCommander->appearance.pcId,
        session->game.commanderSession.currentCommander->appearance.familyName,
        session->game.commanderSession.currentCommander->appearance.commanderName,
        replyMsg
    );

    zoneBuilderJobPts(replyMsg);
    zoneBuilderNormalUnk9(session->game.commanderSession.currentCommander->appearance.pcId, replyMsg);
    zoneBuilderAddonMsg(replyMsg);
    */

    zoneBuilderMoveSpeed(session->game.commanderSession.currentCommander->pcId, 31.0f, replyMsg);

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState zoneHandlerConnect(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    PacketHandlerState status = PACKET_HANDLER_ERROR;

    #pragma pack(push, 1)
    struct {
        uint32_t unk1;
        uint64_t accountId;
        uint64_t zoneServerId;
        uint8_t accountName[ACCOUNT_SESSION_ACCOUNT_NAME_MAXSIZE];
        uint8_t unk4;
        uint32_t zoneServerIndex;
        uint16_t unk3;
        uint8_t channelListId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    dbg("zoneHandlerConnect");
    dbg("unk1 %x", clientPacket->unk1);
    dbg("accountId %d", clientPacket->accountId);
    dbg("zoneServerId %d", clientPacket->zoneServerId);
    dbg("zoneServerIndex %d", clientPacket->zoneServerIndex);
    dbg("unk3 %x", clientPacket->unk3);
    dbg("unk4 %x", clientPacket->unk4);
    dbg("channelListId %d", clientPacket->channelListId);
    dbg("accountName %s", clientPacket->accountName);

    // TODO : Reverse CZ_CONNECT correctly
    // CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_CONNECT);

    GameSession tmpGameSession = {0};
    AccountSession *tmpAccountSession = &tmpGameSession.accountSession;
    CommanderSession *tmpCommanderSession = &tmpGameSession.commanderSession;

    // TODO : Get the account session from Db.
    RedisAccountSessionKey accountKey = {
        .routerId = self->info.routerId,
        .mapId = SOCKET_SESSION_UNDEFINED_MAP,
        .accountId = clientPacket->accountId
    };
    if (!(redisGetAccountSession(self->redis, &accountKey, tmpAccountSession))) {
        error("Cannot retrieve the account session.");
        goto cleanup;
    }

    // Check the client packet here (authentication)
    // TODO improve it
    if (strncmp(tmpAccountSession->accountName,
                clientPacket->accountName,
                sizeof(tmpAccountSession->accountName)) != 0)
    {
        error("Wrong account authentication. (clientPacket account = <%s>, Session account = <%s>",
            clientPacket->accountName, tmpAccountSession->accountName);
        goto cleanup;
    }

    // === Authentication OK ! ===

    // Get list of Commanders for this AccountId
    size_t commandersCount;
    if (!(mySqlLoadAccountCommanders(self->sqlConn, tmpAccountSession, clientPacket->accountId, &commandersCount))) {
        error("Cannot load commanders.");
        goto cleanup;
    }

    // FIXME : Determine how to get the correct commander in the commander array
    tmpCommanderSession->currentCommander = commanderDup(tmpAccountSession->commanders[0]);

    // Get the Game Session that the Barrack Server moved
    // TODO : Should be replaced by Db
    RedisGameSessionKey gameKey = accountKey;
    if (!(redisGetGameSession(self->redis, &gameKey, &tmpGameSession))) {
        error("Cannot retrieve the game session.");
        goto cleanup;
    }

    // Update the Socket Session
    socketSessionInit(&session->socket,
        clientPacket->accountId,
        self->info.routerId,
        tmpCommanderSession->currentCommander->mapId,
        session->socket.sessionKey,
        true
    );

    // Move the game Session to the current mapId
    RedisGameSessionKey fromKey = {
        .routerId = session->socket.routerId,
        .mapId = -1,
        .accountId = session->socket.accountId
    };
    RedisGameSessionKey toKey = {
        .routerId = session->socket.routerId,
        .mapId = session->socket.mapId,
        .accountId = session->socket.accountId
    };
    // TODO : Should be replaced by Db
    if (!(redisMoveGameSession(self->redis, &fromKey, &toKey))) {
        error("Cannot move the game session to the current mapId.");
        goto cleanup;
    }

    // Update the session
    session->game = tmpGameSession;

    // Build a reply packet
    zoneBuilderConnectOk(
        0, // GameMode
        ACCOUNT_SESSION_PRIVILEGES_ADMIN, // accountPrivileges
        session->game.commanderSession.currentCommander,
        replyMsg
    );

    status = PACKET_HANDLER_UPDATE_SESSION;

cleanup:
    return status;
}

static PacketHandlerState zoneHandlerJump(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_JUMP);

    // Notify the players around
    GameEventJump event = {
        .updatePosEvent = {
            .mapId = session->socket.mapId,
            .commander = *session->game.commanderSession.currentCommander
        },
        .height = COMMANDER_HEIGHT_JUMP
    };
    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_JUMP, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerOnAir(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    warning("CZ_ON_AIR not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerOnGround(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg) {
    warning("CZ_ON_GROUND not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerHeadRotate(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        PositionXZ direction;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_HEAD_ROTATE);

    // notify the players around
    GameEventHeadRotate event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
        .direction = clientPacket->direction
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_HEAD_ROTATE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerRotate(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        PositionXZ direction;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_ROTATE);

    // notify the players around
    GameEventRotate event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
        .direction = clientPacket->direction
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_ROTATE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerPose(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        CommanderPose_t poseId;
        PositionXYZ position;
        PositionXZ direction;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_POSE);

    // notify the players around
    GameEventPose event = {
        .pcId = session->game.commanderSession.currentCommander->pcId,
        .poseId = clientPacket->poseId,
        .position = session->game.commanderSession.currentCommander->pos,
        .direction = clientPacket->direction
    };

    workerDispatchEvent(self, session->socket.sessionKey, EVENT_TYPE_POSE, &event, sizeof(event));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerDashRun(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    /*#pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_DASHRUN);
    zoneBuilderMoveSpeed(session->game.commanderSession.currentCommander->pcId, 38.0f, replyMsg);*/

    warning("CZ_DASHRUN not implemented yet.");

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerItemDelete(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint16_t unk1;
        uint32_t unk2;
        uint64_t itemId;
        uint64_t unk3;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_ITEM_DELETE);

    // Delete item from inventory
    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;

    Item *item;
    if (inventoryGetItemByActorId(inventory, clientPacket->itemId, &item)) {
        inventoryRemoveItem(&session->game.commanderSession.currentCommander->inventory, item);
    } else {
        error("Item not found in inventory");
        return PACKET_HANDLER_ERROR;
    }

    ///TODO
    InventoryRemoval removalType = INVENTORY_REMOVAL_DESTROYED;
    InventoryType inventoryType = INVENTORY_TYPE_COMMANDER;

    zoneBuilderItemRemove(item, removalType, inventoryType, replyMsg);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerSwapEtcInvChangeIndex(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint8_t inventoryType;
        uint64_t actorId1;
        uint32_t inventoryIndex1;
        uint64_t actorId2;
        uint32_t inventoryIndex2;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    dbg("Swaping items");
    dbg("actorId1 %d", clientPacket->actorId1);
    dbg("inventoryIndex1 %d", clientPacket->inventoryIndex1);
    dbg("actorId2 %d", clientPacket->actorId2);
    dbg("inventoryIndex2 %d", clientPacket->inventoryIndex2);

    CHECK_CLIENT_PACKET_SIZE(*clientPacket, packetSize, CZ_SWAP_ETC_INV_CHANGE_INDEX);

    if (clientPacket->inventoryIndex1 == clientPacket->inventoryIndex2) {
        return PACKET_HANDLER_ERROR;
    }

    // Delete item from inventory
    Inventory *inventory = &session->game.commanderSession.currentCommander->inventory;

    if (!inventorySwapItems(inventory, clientPacket->actorId1, clientPacket->actorId2)) {
        error("Error when swapping items in inventory");
        return PACKET_HANDLER_ERROR;
    }

    // No packet in return?

    return PACKET_HANDLER_OK;
}

static PacketHandlerState zoneHandlerReqNormalTxNumArg(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint16_t unk1;
        uint16_t unk2;
        uint32_t maxLevel; // ??
        uint32_t classId;
        uint32_t skill1Level;
        uint32_t skill2Level;
        uint32_t skill3Level;
        uint32_t skill4Level;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    dbg("unk1 %d", clientPacket->unk1);
    dbg("unk2 %d", clientPacket->unk2);
    dbg("maxLevel %d", clientPacket->maxLevel);
    dbg("classId %d", clientPacket->classId);
    dbg("skill1Level %d", clientPacket->skill1Level);
    dbg("skill2Level %d", clientPacket->skill2Level);
    dbg("skill3Level %d", clientPacket->skill3Level);
    dbg("skill4Level %d", clientPacket->skill4Level);

    zoneBuilderJobPoints(4001, 1, replyMsg);
    zoneBuilderSkillAdd(clientPacket->classId, replyMsg);

    return PACKET_HANDLER_OK;

}

static PacketHandlerState zoneHandlerDynamicCastingStart(
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *replyMsg)
{
    #pragma pack(push, 1)
    struct {
        uint32_t skillId;
        float unk2; // time?
        uint32_t unk3;
        uint8_t unk4;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    dbg("skillId %d", clientPacket->skillId);
    dbg("unk2 %f", clientPacket->unk2);
    dbg("unk3 %d", clientPacket->unk3);
    dbg("unk4 %d", clientPacket->unk4);

    PositionXZ dir;

    dir.x = (float) SWAP_UINT32(0xEF04353F);
    dir.z = (float) SWAP_UINT32(0xF70435BF);

    // Stop Commander in current position
    zoneBuilderPcMoveStop(
        session->game.commanderSession.currentCommander->pcId,
        &session->game.commanderSession.currentCommander->pos,
        &dir,
        0,
        replyMsg
    );

    // Unknown
    zoneBuilderNormalUnk14_4c(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        replyMsg
    );

    // Skill is ready to be casted
    zoneBuilderSkillReady(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &session->game.commanderSession.currentCommander->pos,
        &session->game.commanderSession.currentCommander->pos,
        replyMsg
    );

    // Unknown
    zoneBuilderNormalUnk11_1c(
        session->game.commanderSession.currentCommander->pcId,
        &session->game.commanderSession.currentCommander->pos,
        &dir,
        replyMsg
    );

    // Set range of effect for this skill? (cone)
    zoneBuilderSkillRangeSquare(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &session->game.commanderSession.currentCommander->pos,
        &session->game.commanderSession.currentCommander->pos,
        replyMsg
    );

    // Skill melee ground
    ///

    // Make the skill appear
    zoneBuilderNormalUnk10_56(
        session->game.commanderSession.currentCommander->pcId,
        clientPacket->skillId,
        &session->game.commanderSession.currentCommander->pos,
        &dir,
        true,
        replyMsg
    );

    zoneBuilderBuffAdd(
        session->game.commanderSession.currentCommander->pcId,
        session->game.commanderSession.currentCommander,
        replyMsg
    );

    return PACKET_HANDLER_OK;

}




