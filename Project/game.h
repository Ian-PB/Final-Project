#pragma once
#include "Server.h"
#include "Client.h"

#include <raylib.h>
#include "World.h"
#include "PauseMenu.h"
#include "Player.h"
#include <memory>

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

    void PlayerAdded(std::shared_ptr<Player> t_player, int t_owner);

    std::shared_ptr<MainCamera> camera;

    // Networking
    NetworkingTypes networkType;
    std::shared_ptr<Server> server;

    void ClientUpdate();
    std::shared_ptr<Client> client;

    GameStates gameState = GameStates::ChooseNetworkType;

    // Connection Error
    bool hadIssueConnecting = false;
    const std::string ERROR_CONNECTING_MSG = "There was an issue connecting to the server";
    const std::string ERROR_INVALID_IP = "Invalid IP";

    // IP check
    bool IsValidIp(const std::string& t_ip);
    bool invalidIpInput = false;

    // Joining variables
    std::string joiningText = "Joining...";
    char serverIpInput[64] = "";
    int activeButton = -1;

    World world;

    std::shared_ptr<Player> hostPlayer;

    std::shared_ptr<PauseMenu> pauseMenu;
};