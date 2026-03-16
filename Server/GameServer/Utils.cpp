#include "pch.h"
#include "Utils.h"
#include "Player.h"

atomic<uint64> Utils::objectId = 1;

PlayerRef Utils::MakePlayerRef(GameSessionRef session)
{
	PlayerRef player = make_shared<Player>();
	player->objectInfo->set_object_id(objectId.fetch_add(1));

	player->session = session;
	session->player.store(player);

	return player;
}