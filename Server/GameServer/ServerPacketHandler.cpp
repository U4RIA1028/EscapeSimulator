#include "pch.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"

map<uint16, handleFunction> _packetHandler;

RoomRef GetRoomRef(PacketSessionRef session)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return nullptr;

	return player->room.load().lock();
}

bool Handle_ENTER_GAME(PacketSessionRef& ref, Protocol::C_ENTER_GAME pkt)
{
	std::cout << "Enter Game" << std::endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(ref);
	if (gameSession->player.load() != nullptr)
		return false;

	PlayerRef player = Utils::MakePlayerRef(gameSession);
	if (player == nullptr)
		return false;

	player->name = pkt.name();
	cout << "Player Inside " << player->objectInfo->object_id() << endl;

	GRoom->DoAsync(&RoomManager::GameInside, player, pkt.room_id());

	return true;
}

bool Handle_CREATE_GAME(PacketSessionRef& ref, Protocol::C_CREATE_GAME pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(ref);
	if (gameSession->player.load() != nullptr)
		return false;

	PlayerRef player = Utils::MakePlayerRef(gameSession);
	if (player == nullptr)
		return false;

	player->name = pkt.name();
	cout << "Player Create " << player->name << endl;

	GRoom->DoAsync(&RoomManager::GameCreate, player);

	return true;
}

bool Handle_GAME_START(PacketSessionRef& ref, Protocol::C_GAME_START pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(ref);

	PlayerRef player = gameSession->player.load();
	if (player != nullptr)
	{
		RoomRef room = player->room.load().lock();
		if (room != nullptr)
		{
			room->DoAsync(&Room::HandleGameStart, pkt.stage_number());

			return true;
		}
	}

	return false;
}

bool Handle_LEAVE_GAME(PacketSessionRef& ref, Protocol::C_LEAVE_GAME pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(ref);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	cout << "Player OutSide " << player->objectInfo->object_id() << endl;

	room->DoAsync(&Room::HandleLeavePlayer, player);

	return true;
}

bool Handle_GAME_END(PacketSessionRef& ref, Protocol::C_GAME_END pkt)
{
	RoomRef room = GetRoomRef(ref);
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleGameEnd, pkt);

	return true;
}

bool Handle_SPAWN(PacketSessionRef& ref, Protocol::C_SPAWN pkt)
{
	return false;
}

bool Handle_DESPAWN(PacketSessionRef& ref, Protocol::C_DESPAWN pkt)
{
	return false;
}

bool Handle_MOVE(PacketSessionRef& ref, Protocol::C_MOVE pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(ref);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	cout << "Handle Move  : " << pkt.player().object_id() << endl;
	room->DoAsync(&Room::PlayerMove, pkt);

	return true;
}

bool Handle_ACTION_PICK_UP(PacketSessionRef& ref, Protocol::C_ACTION_PICK_UP pkt)
{
	RoomRef room = GetRoomRef(ref);
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::ActionPickup, pkt);

	return true;
}

bool Handle_ACTION_DOOR(PacketSessionRef& ref, Protocol::C_ACTION_DOOR pkt)
{
	RoomRef room = GetRoomRef(ref);
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::ActionDoorOpen, pkt);

	return true;
}

bool Handle_ACTION_HAND_LIGHT(PacketSessionRef& ref, Protocol::C_ACTION_HAND_LIGHT pkt)
{
	RoomRef room = GetRoomRef(ref);
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::ActionRightOn, pkt);

	return true;
}

bool Handle_ACTION_KEY(PacketSessionRef& ref, Protocol::C_ACTION_KEY pkt)
{
	return true;
}

bool Handle_ACTION_DRAWERS(PacketSessionRef& ref, Protocol::C_ACTION_DRAWERS pkt)
{
	RoomRef room = GetRoomRef(ref);
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::ActionDrawers, pkt);

	return false;
}

bool Handle_ACTION_PUZZLE(PacketSessionRef& ref, Protocol::C_ACTION_PUZZLE pkt)
{
	return false;
}

bool Handle_ACTION_LOCK_FREE(PacketSessionRef& ref, Protocol::C_ACTION_LOCK_FREE pkt)
{
	RoomRef room = GetRoomRef(ref);
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::ActionLockFree, pkt);

	return true;
}

