#include "pch.h"
#include "Room.h"
#include "Player.h"

Room::Room(string roomId) : _roomId(roomId)
{
	_spawnPoint.push_back(Point(0.f, 0.f, 0.f));

	for (int i = -3; i <= 4; i++)
	{
		_spawnPoint.push_back(Point(0.f, i * 100.f, 88.f));
	}
}

Room::~Room()
{
	_players.clear();
	_spawnPoint.clear();

	std::cout << _roomId << " is Closed" << std::endl;
}

void Room::HandleEnterHostPlayer(PlayerRef player)
{
	bool success = EnterPlayer(player);
	if (success == false)
	{
		GRoom->DoAsync(&RoomManager::GameDelete, _roomId);

		return;
	}
	uint64 count = _players.size() - 1;

	player->objectInfo->set_is_host(true);
	player->SetStartPoint(_spawnPoint[count].x, _spawnPoint[count].y, _spawnPoint[count].z);

	player->moveInfo->set_yaw(Utils::GetRandom(0.f, 100.f));

	{
		Protocol::S_CREATE_GAME createGamePkt;
		createGamePkt.set_success(success);
		createGamePkt.set_room_id(_roomId);
		createGamePkt.set_player_name(player->name);

		Protocol::ObjectInfo* objectInfo = createGamePkt.mutable_player();
		objectInfo->CopyFrom(*player->objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(createGamePkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	_hostPlayer = player;
}

void Room::HandleEnterPlayer(PlayerRef player)
{
	bool success = EnterPlayer(player);
	if (success == false)
		return;

	uint64 count = _players.size() - 1;

	player->objectInfo->set_is_host(false);
	player->SetStartPoint(_spawnPoint[count].x, _spawnPoint[count].y, _spawnPoint[count].z);

	player->moveInfo->set_yaw(Utils::GetRandom(0.f, 100.f));

	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);
		enterGamePkt.set_room_id(_roomId);
		enterGamePkt.set_player_name(player->name);

		Protocol::ObjectInfo* objectInfo = enterGamePkt.mutable_player();
		objectInfo->CopyFrom(*player->objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
		objectInfo->CopyFrom(*player->objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, player->objectInfo->object_id());
	}

	{
		Protocol::S_SPAWN spawnPkt;

		for (auto& [key, value] : _players)
		{
			Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
			objectInfo->CopyFrom(*value->objectInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}
}

void Room::HandleLeavePlayer(PlayerRef player)
{
	if (player == nullptr)
		return;

	if (player == _hostPlayer)
	{
		cout << "Host Player Is Leave" << endl;
	}

	const uint64 objectId = player->objectInfo->object_id();
	bool success = LeavePlayer(objectId);

	if (success == false)
		return;

	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectId);

		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	if (_players.size() == 0)
	{
		GRoom->DoAsync(&RoomManager::GameDelete, _roomId);
	}
}

void Room::HandleGameStart(Protocol::StageNumber stageNumber)
{
	switch (stageNumber)
	{
	case Protocol::STAGE_NULL:
		break;
	case Protocol::STAGE_ONE:
		StageOneStart();
		break;
	case Protocol::STAGE_TWO:
		StageTwoStart();
		break;
	default:
		break;
	}

	// TODO
}

void Room::ActionDoorOpen(Protocol::C_ACTION_DOOR actionPkt)
{
	uint64 playerId = actionPkt.player_id();
	if (CheckPlayer(playerId) == false)
		return;

	cout << "DoorOpen " << actionPkt.player_id() << endl;

	Protocol::S_ACTION_DOOR sendPkt;
	sendPkt.set_object_name(actionPkt.object_name());
	sendPkt.set_player_id(playerId);
	sendPkt.set_door_type(actionPkt.door_type());
	sendPkt.set_is_open_door(actionPkt.is_open_door());

	SEND_BROADCAST_EXPECTED(sendPkt, playerId);
}

void Room::ActionRightOn(Protocol::C_ACTION_HAND_LIGHT actionPkt)
{
	uint64 playerId = actionPkt.player_id();
	if (CheckPlayer(playerId) == false)
		return;

	Protocol::S_ACTION_HAND_LIGHT sendPkt;
	sendPkt.set_player_id(playerId);
	sendPkt.set_is_on_light(actionPkt.is_on_light());

	SEND_BROADCAST_EXPECTED(sendPkt, playerId);
}

void Room::ActionLockFree(Protocol::C_ACTION_LOCK_FREE actionPkt)
{
	uint64 playerId = actionPkt.player_id();
	if (CheckPlayer(playerId) == false)
		return;

	cout << "ActionLockFree" << actionPkt.player_id() << endl;

	Protocol::S_ACTION_LOCK_FREE sendPkt;
	sendPkt.set_player_id(playerId);
	sendPkt.set_object_name(actionPkt.object_name());
	sendPkt.set_type(actionPkt.type());
	sendPkt.set_is_success(actionPkt.is_success());

	SEND_BROADCAST_EXPECTED(sendPkt, playerId);
}

void Room::ActionPickup(Protocol::C_ACTION_PICK_UP actionPkt)
{
	cout << "PickUp" << endl;

	Protocol::S_ACTION_PICK_UP sendPkt;
	sendPkt.set_object_name(actionPkt.object_name());
	sendPkt.set_type(actionPkt.type());

	SEND_BROADCAST(sendPkt);
}

void Room::ActionDrawers(Protocol::C_ACTION_DRAWERS actionPkt)
{
	uint64 playerId = actionPkt.player_id();
	if (CheckPlayer(playerId) == false)
		return;

	cout << "ActionDrawers" << actionPkt.player_id() << endl;

	Protocol::S_ACTION_DRAWERS sendPkt;
	sendPkt.set_player_id(playerId);
	sendPkt.set_object_name(actionPkt.object_name());
	sendPkt.set_is_open(actionPkt.is_open());

	SEND_BROADCAST_EXPECTED(sendPkt, playerId);
}

void Room::HandleGameEnd(Protocol::C_GAME_END endPkt)
{
	Protocol::S_GAME_END sendPkt;
	sendPkt.set_object_name(endPkt.object_name());

	BroadcastPlayerInfo(sendPkt);

	_isStart.store(false);
}

void Room::PlayerMove(Protocol::C_MOVE pkt)
{
	auto playerPair = _players.find(pkt.player().object_id());
	if (playerPair == _players.end())
		return;

	PlayerRef player = playerPair->second;
	player->SetObjectInfo(pkt.player());

	{
		Protocol::S_MOVE movePkt;
		Protocol::ObjectInfo* objectInfo = movePkt.mutable_player();
		objectInfo->CopyFrom(pkt.player());

		SEND_BROADCAST(movePkt);
	}
}

bool Room::EnterPlayer(PlayerRef player)
{
	if (_isStart.load())
		return false;

	uint64 playerId = player->objectInfo->object_id();

	if (_players.find(playerId) != _players.end())
		return false;

	_players.insert(make_pair(playerId, player));

	player->room.store(static_pointer_cast<Room>(shared_from_this()));

	return true;
}

bool Room::LeavePlayer(uint64 objectId)
{
	if (_players.find(objectId) == _players.end())
		return false;

	PlayerRef player = _players[objectId];
	player->room.store(weak_ptr<Room>());

	_players.erase(objectId);

	return true;
}

void Room::StageOneStart()
{
	StageStart(-10280, 1230, -100);
}

void Room::StageTwoStart()
{
	StageStart(3300, -1000, -169);
}

void Room::StageStart(float startx, float starty, float startz)
{
	_isStart.store(true);

	Protocol::S_GAME_START startPkt;
	for (auto& [id, player] : _players)
	{
		player->moveInfo->set_object_id(id);
		player->moveInfo->set_x(startx);
		player->moveInfo->set_y(starty);
		player->moveInfo->set_z(startz);
		player->moveInfo->set_yaw(175.f);

		Protocol::MoveInfo* moveInfo = startPkt.add_player();
		moveInfo->CopyFrom(*player->moveInfo);
	}

	SEND_BROADCAST(startPkt);
}

bool Room::CheckPlayer(uint64 playerId)
{
	auto playerPair = _players.find(playerId);
	if (playerPair == _players.end())
		return false;

	return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 expectedId)
{
	for (auto& [key, value] : _players)
	{
		if (key == expectedId)
			continue;

		if (auto session = value->session.lock())
			session->Send(sendBuffer);
	}
}

void Room::BroadcastPlayerInfo(Protocol::S_GAME_END packet)
{
	for (auto& [key, player] : _players)
	{
		if (auto session = player->session.lock())
		{
			player->SetStart();
			Protocol::ObjectInfo* objectInfo = packet.add_players();
			objectInfo->CopyFrom(*player->objectInfo);
		}
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	Broadcast(sendBuffer);
}