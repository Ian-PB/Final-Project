#pragma once
#include <vector>
#include <memory>
#include "Player.h"

class World;

class PlayersManager
{
public:
	static void SetWorldPointer(World* t_world) { world = t_world; }

	static std::shared_ptr<Player> GetPlayerByOwner(int t_owner);
	static std::shared_ptr<Player> AddPlayer(int t_owner);

	static std::vector<std::shared_ptr<Player>> GetPlayers() { return players; }
private:
	static std::vector<std::shared_ptr<Player>> players;

	static World* world;
};

#include "World.h"