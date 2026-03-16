#pragma once

#define WIN32_LEAN_AND_MEAN 

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CorePch.h"
#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"

using GameSessionRef = shared_ptr<class GameSession>;
using PlayerRef = shared_ptr<class Player>;
using RoomRef = shared_ptr<class Room>;
using RoomManagerRef = shared_ptr<class RoomManager>;

#define TIME_SYNC 100

#include "SendBuffer.h"
#include "Utils.h"
#include "GameSession.h"
#include "RoomManager.h"
#include "ServerPacketHandler.h"

#define SEND_PACKET(pkt)												\
SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);	\
session->Send(sendBuffer);			

#define SEND_BROADCAST(pkt)												\
SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);	\
Broadcast(sendBuffer);

#define SEND_BROADCAST_EXPECTED(pkt, id)								\
SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);	\
Broadcast(sendBuffer, id);