#include "pch.h"
#include "RoomManager.h"
#include "Room.h"
#include <random>

RoomManagerRef GRoom = make_shared<RoomManager>();

RoomManager::RoomManager()
{

}

RoomManager::~RoomManager()
{
	_rooms.clear();
}

void RoomManager::GameCreate(PlayerRef player)
{
#ifdef DEBUG
	string roomId = "AAAA";
#else
	string roomId = "";
	while (1)
	{
		roomId = "";
		for (int i = 0; i < 10; i++)
		{
			int r = Utils::GetRandom(0, 25);
			roomId += (r + 'A');
		}

		if (_rooms.find(roomId) == _rooms.end())
			break;
	}
#endif // DEBUG

	_rooms.emplace(pair{ roomId, make_shared<Room>(roomId) });

	_rooms[roomId]->DoAsync(&Room::HandleEnterHostPlayer, player);
}

void RoomManager::GameDelete(string  roomId)
{
	if (_rooms.find(roomId) == _rooms.end())
		return;

	_rooms.erase(roomId);
}

void RoomManager::GameInside(PlayerRef player, string  roomId)
{
	if (_rooms.find(roomId) == _rooms.end())
		return;

	_rooms[roomId]->DoAsync(&Room::HandleEnterPlayer, player);
}