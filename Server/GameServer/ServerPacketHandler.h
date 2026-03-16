#pragma once

#include "Protocol.pb.h"
#include <map>

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "Capstone.h"
#endif

using handleFunction = std::function<bool(PacketSessionRef&, BYTE*, uint16)>;
extern std::map<uint16, handleFunction> _packetHandler;
template<typename PacketData, typename handleFunc>
inline bool ParsingPacket(handleFunc func, PacketSessionRef& ref, BYTE* buffer, uint16 len);

bool Handle_ENTER_GAME(PacketSessionRef& ref, Protocol::C_ENTER_GAME pkt);
bool Handle_CREATE_GAME(PacketSessionRef& ref, Protocol::C_CREATE_GAME pkt);
bool Handle_GAME_START(PacketSessionRef& ref, Protocol::C_GAME_START pkt);
bool Handle_LEAVE_GAME(PacketSessionRef& ref, Protocol::C_LEAVE_GAME pkt);
bool Handle_GAME_END(PacketSessionRef& ref, Protocol::C_GAME_END pkt);
bool Handle_SPAWN(PacketSessionRef& ref, Protocol::C_SPAWN pkt);
bool Handle_DESPAWN(PacketSessionRef& ref, Protocol::C_DESPAWN pkt);
bool Handle_MOVE(PacketSessionRef& ref, Protocol::C_MOVE pkt);
bool Handle_ACTION_PICK_UP(PacketSessionRef& ref, Protocol::C_ACTION_PICK_UP pkt);
bool Handle_ACTION_DOOR(PacketSessionRef& ref, Protocol::C_ACTION_DOOR pkt);
bool Handle_ACTION_HAND_LIGHT(PacketSessionRef& ref, Protocol::C_ACTION_HAND_LIGHT pkt);
bool Handle_ACTION_KEY(PacketSessionRef& ref, Protocol::C_ACTION_KEY pkt);
bool Handle_ACTION_DRAWERS(PacketSessionRef& ref, Protocol::C_ACTION_DRAWERS pkt);
bool Handle_ACTION_PUZZLE(PacketSessionRef& ref, Protocol::C_ACTION_PUZZLE pkt);
bool Handle_ACTION_LOCK_FREE(PacketSessionRef& ref, Protocol::C_ACTION_LOCK_FREE pkt);


class ServerPacketHandler
{
public:
	static void Init()
	{
		        _packetHandler[Protocol::INGAME::ENTER_GAME] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ENTER_GAME>(Handle_ENTER_GAME, ref, buf, size); };
        _packetHandler[Protocol::INGAME::CREATE_GAME] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_CREATE_GAME>(Handle_CREATE_GAME, ref, buf, size); };
        _packetHandler[Protocol::INGAME::GAME_START] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_GAME_START>(Handle_GAME_START, ref, buf, size); };
        _packetHandler[Protocol::INGAME::LEAVE_GAME] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_LEAVE_GAME>(Handle_LEAVE_GAME, ref, buf, size); };
        _packetHandler[Protocol::INGAME::GAME_END] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_GAME_END>(Handle_GAME_END, ref, buf, size); };
        _packetHandler[Protocol::INGAME::SPAWN] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_SPAWN>(Handle_SPAWN, ref, buf, size); };
        _packetHandler[Protocol::INGAME::DESPAWN] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_DESPAWN>(Handle_DESPAWN, ref, buf, size); };
        _packetHandler[Protocol::INGAME::MOVE] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_MOVE>(Handle_MOVE, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_PICK_UP] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_PICK_UP>(Handle_ACTION_PICK_UP, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_DOOR] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_DOOR>(Handle_ACTION_DOOR, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_HAND_LIGHT] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_HAND_LIGHT>(Handle_ACTION_HAND_LIGHT, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_KEY] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_KEY>(Handle_ACTION_KEY, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_DRAWERS] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_DRAWERS>(Handle_ACTION_DRAWERS, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_PUZZLE] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_PUZZLE>(Handle_ACTION_PUZZLE, ref, buf, size); };
        _packetHandler[Protocol::INGAME::ACTION_LOCK_FREE] = [](PacketSessionRef& ref, BYTE* buf, uint16 size) { return ParsingPacket<Protocol::C_ACTION_LOCK_FREE>(Handle_ACTION_LOCK_FREE, ref, buf, size); };

	}
	static bool PakcetHandle(PacketSessionRef & session, BYTE * buffer, int32 len)
	{
		PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);

		if (_packetHandler.find(head->type) == _packetHandler.end())
			return false;
		return _packetHandler[head->type](session, buffer, head->size);
	}

public:
	    static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_GAME pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ENTER_GAME); }
    static SendBufferRef MakeSendBuffer(Protocol::S_CREATE_GAME pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::CREATE_GAME); }
    static SendBufferRef MakeSendBuffer(Protocol::S_GAME_START pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::GAME_START); }
    static SendBufferRef MakeSendBuffer(Protocol::S_LEAVE_GAME pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::LEAVE_GAME); }
    static SendBufferRef MakeSendBuffer(Protocol::S_GAME_END pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::GAME_END); }
    static SendBufferRef MakeSendBuffer(Protocol::S_SPAWN pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::SPAWN); }
    static SendBufferRef MakeSendBuffer(Protocol::S_DESPAWN pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::DESPAWN); }
    static SendBufferRef MakeSendBuffer(Protocol::S_MOVE pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::MOVE); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_PICK_UP pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_PICK_UP); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_DOOR pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_DOOR); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_HAND_LIGHT pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_HAND_LIGHT); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_KEY pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_KEY); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_DRAWERS pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_DRAWERS); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_PUZZLE pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_PUZZLE); }
    static SendBufferRef MakeSendBuffer(Protocol::S_ACTION_LOCK_FREE pkt) { return _MakeSendBuffer(pkt, Protocol::INGAME::ACTION_LOCK_FREE); }


	template<typename PacketData, typename PacketType>
	static SendBufferRef _MakeSendBuffer(PacketData & pkt, PacketType type);
};

template<typename PacketData, typename PacketType>
inline SendBufferRef ServerPacketHandler::_MakeSendBuffer(PacketData& pkt, PacketType type)
{
	const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
	const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
	SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
	SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
#endif

	PacketHeader* head = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	head->type = type;
	head->size = packetSize;

	pkt.SerializeToArray(&head[1], dataSize);
	sendBuffer->Close(packetSize);

	return sendBuffer;
}

template<typename PacketData, typename handleFunc>
inline bool ParsingPacket(handleFunc func, PacketSessionRef& ref, BYTE* buffer, uint16 len)
{
	PacketData pkt;
	pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));

	return func(ref, pkt);
}
