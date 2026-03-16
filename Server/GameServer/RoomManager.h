#pragma once
#include "JobQueue.h"
class Room;

class RoomManager : public JobQueue
{
public:
	RoomManager();
	~RoomManager();

	void GameCreate(PlayerRef player);
	void GameDelete(string  roomId);

	void GameInside(PlayerRef player, string  roomId);

	RoomManagerRef GetRef() { return static_pointer_cast<RoomManager>(shared_from_this()); }

private:
	// RoomId를 String값으로 두었는데,엄청 많은 양의 데이터를 검색하는 것이 아니면 그렇게 속도차이가
	// 나지 않는 다는 내용을 토대로 구현.
	// https://blog.naver.com/PostView.naver?blogId=kal9107&logNo=223044041516

	map<string, RoomRef>	_rooms;
};

extern RoomManagerRef GRoom;