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
    std::shared_ptr<Player> GetPlayerByOwner(int t_owner);
    void AddPlayer(int t_owner);

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
    char serverIpInput[64] = "";
    int activeButton = -1;

    std::vector<std::shared_ptr<Player>> players;
    World world;

    std::shared_ptr<PauseMenu> pauseMenu;
};