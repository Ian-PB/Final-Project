#pragma once
#include "Server.h"
#include "Client.h"

#include <raylib.h>
#include "World.h"
#include "Player.h"
#include "PauseMenu.h"

enum class NetworkingTypes
{
    Host,
    Client
};

enum class GameStates
{
    ChooseNetworkType,
    JoinGame,
    Game
};

class Game
{
public:
    void Init();
    void Draw();
    void DrawPreGame();
    void DrawJoinGame();
    void DrawGame();

    void Update();

private:
    // Networking
    NetworkingTypes networkType;
    Server server;
    Client client;

    GameStates gameState = GameStates::ChooseNetworkType;

    std::shared_ptr<Player> player;

    World world;

    std::shared_ptr<PauseMenu> pauseMenu;
};