#pragma once

class GameSession;
class Room;

class Player : public enable_shared_from_this<Player>
{
public:
	Player();
	virtual ~Player();

	void SetObjectInfo(const Protocol::ObjectInfo info);
	void SetStartPoint(float x, float y, float z);
	void SetStart();

public:
	Protocol::ObjectInfo* objectInfo;
	Protocol::MoveInfo* moveInfo;

	float					startX;
	float					startY;
	float					startZ;

	weak_ptr<GameSession>	session;
	std::string				name;

public:
	atomic<weak_ptr<Room>> room;
};

