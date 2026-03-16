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

{0}

class {3}
{
public:
	static void Init()
	{
		{1}
	}
	static bool PakcetHandle(PacketSessionRef & session, BYTE * buffer, int32 len)
	{
		PacketHeader* head = reinterpret_cast<PacketHeader*>(buffer);

		if (_packetHandler.find(head->type) == _packetHandler.end())
			return false;
		return _packetHandler[head->type](session, buffer, head->size);
	}

public:
	{2}

	template<typename PacketData, typename PacketType>
	static SendBufferRef _MakeSendBuffer(PacketData & pkt, PacketType type);
};

template<typename PacketData, typename PacketType>
inline SendBufferRef {3}::_MakeSendBuffer(PacketData& pkt, PacketType type)
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