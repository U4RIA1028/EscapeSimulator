#pragma once
#include "JobQueue.h"

struct Point
{
	float x;
	float y;
	float z;
};

class Room : public JobQueue
{
public:
	Room(string roomId);
	virtual ~Room();

	void HandleEnterHostPlayer(PlayerRef player);
	void HandleEnterPlayer(PlayerRef player);
	void HandleLeavePlayer(PlayerRef player);
	void HandleGameStart(Protocol::StageNumber stageNumber);
	
	void ActionDoorOpen(Protocol::C_ACTION_DOOR actionPkt);
	void ActionRightOn(Protocol::C_ACTION_HAND_LIGHT actionPkt);
	void ActionLockFree(Protocol::C_ACTION_LOCK_FREE actionPkt);
	void ActionPickup(Protocol::C_ACTION_PICK_UP actionPkt);
	void ActionDrawers(Protocol::C_ACTION_DRAWERS actionPkt);

	void HandleGameEnd(Protocol::C_GAME_END endPkt);

	void PlayerMove(Protocol::C_MOVE pkt);

private:
	bool EnterPlayer(PlayerRef player);
	bool LeavePlayer(uint64 id);

	void StageOneStart();
	void StageTwoStart();
	void StageStart(float startx, float starty, float startz);

	bool CheckPlayer(uint64 playerId);

private:
	void Broadcast(SendBufferRef sendBuffer, uint64 expectedId = 0);

	void BroadcastPlayerInfo(Protocol::S_GAME_END pakcet);

private:
	string								_roomId = 0;
	atomic<bool>						_isStart = false;
	PlayerRef							_hostPlayer = nullptr;
	unordered_map<uint64, PlayerRef>	_players;
	vector<Point>						_spawnPoint;
};