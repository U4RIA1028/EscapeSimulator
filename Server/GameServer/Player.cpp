#include "pch.h"
#include "Player.h"

Player::Player()
{
	objectInfo = new Protocol::ObjectInfo();
	moveInfo = objectInfo->mutable_move_info();
}

Player::~Player()
{
	delete objectInfo;
}

void Player::SetObjectInfo(const Protocol::ObjectInfo info)
{
	objectInfo->CopyFrom(info);
	moveInfo = objectInfo->mutable_move_info();
}

void Player::SetStartPoint(float x, float y, float z)
{
	startX = x;
	startY = y;
	startZ = z;

	SetStart();
}

void Player::SetStart()
{
	moveInfo->set_x(startX);
	moveInfo->set_y(startY);
	moveInfo->set_z(startZ);
}