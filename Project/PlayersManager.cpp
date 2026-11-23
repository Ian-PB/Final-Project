#include "PlayersManager.h"
#include "MeshCollider.h"
#include <raylib.h>
#include <memory>

std::vector<std::shared_ptr<Player>> PlayersManager::players;
World* PlayersManager::world = nullptr;

std::shared_ptr<Player> PlayersManager::GetPlayerByOwner(int t_owner)
{
    for (const std::shared_ptr<Player>& player : players)
    {
        if (player->GetOwner() == t_owner)
            return player;
    }

    return nullptr;
}

std::shared_ptr<Player> PlayersManager::AddPlayer(int t_owner)
{
    // Create the player
    std::shared_ptr<Player> newPlayer = std::make_shared<Player>(t_owner, *world);
    newPlayer->Init();

    // Set spawn position
    Vector3 worldCenter = Vector3Scale(world->GetSizeDimensions(), 0.5f);
    newPlayer->SetPos(worldCenter);

    // Allow mesh tracking
    MeshCollider::AddTrackedPos(newPlayer->GetPosRef());

    players.push_back(newPlayer);

    return newPlayer;
}
