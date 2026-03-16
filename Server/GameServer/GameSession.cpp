#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
#include "Room.h"
#include "Player.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	if (RoomRef room = player.load()->room.load().lock())
	{
		room->HandleLeavePlayer(player.load());
	}

	player.store(nullptr);

	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		
	// TODO : packetId
	ServerPacketHandler::PakcetHandle(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{

}