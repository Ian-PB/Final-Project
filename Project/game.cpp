#include "game.h"

void Game::Init()
{
    Lighting::Init();

    world.InitColliders();

    AddPlayer(0);

    pauseMenu = std::make_shared<PauseMenu>(*players[0], world); // player[0] is wrong currently

    EnableCursor();
}

void Game::Draw()
{
    switch (gameState)
    {
    case GameStates::ChooseNetworkType:
        DrawPreGame();
        break;
    case GameStates::JoinGame:
        DrawJoinGame();
        break;
    case GameStates::Game:
        DrawGame();
        break;
    default:
        break;
    }
}

void Game::DrawPreGame()
{
    Rectangle rect;
    rect.width = GetScreenWidth();
    rect.height = GetScreenHeight() / 2.0f;
    rect.x = 0.0f;
    rect.y = 0.0f;
    if (GuiButton(rect, "Host"))
    {
        networkType = NetworkingTypes::Host;
        world.Init((int)(rand() % 9999)); // Set world

        server = std::make_shared<Server>(world);
        server->Init();

        gameState = GameStates::Game;
        DisableCursor();

        players[0]->SetOwner(true);

        return;
    }

    rect.y += GetScreenHeight() / 2.0f;
    if (GuiButton(rect, "Join"))
    {
        networkType = NetworkingTypes::Client;
        client = std::make_shared<Client>(world);

        gameState = GameStates::JoinGame;

        return;
    }
}

void Game::DrawJoinGame()
{
    Rectangle rect;
    rect.width = GetScreenWidth() * 0.8f;
    rect.height = GetScreenHeight() * 0.8f;
    rect.x = GetScreenWidth() * 0.1f;
    rect.y = GetScreenHeight() * 0.1f;

    GuiTextInputBox(rect, "Server IP: ", "Enter the Servers IP: ", serverIpInput, rect.height * 0.9f, "Join;Cancel", &activeButton, nullptr);

    if (activeButton == 1) // Join Pressed
    {
        client->JoinServer(serverIpInput);
        gameState = GameStates::Game;
        DisableCursor();
    }
    else if (activeButton == 2 || activeButton == 0) // Cancel
    {
        gameState = GameStates::ChooseNetworkType;
    }
}

void Game::DrawGame()
{
    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1)
    {
        std::shared_ptr<Player> localPlayer = GetPlayerByOwner(client->GetClientsObjs());
        BeginMode3D(localPlayer->GetCameraRef());
    }
    else if (networkType == NetworkingTypes::Host)
        BeginMode3D(players[0]->GetCameraRef());

        world.Draw();

        for (const std::shared_ptr<Player> player : players)
        {
            player->Draw();
        }

    EndMode3D();

    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1)
    {
        {
            std::shared_ptr<Player> localPlayer = GetPlayerByOwner(client->GetClientsObjs());
            localPlayer->Draw2D();
        }
    }
    else if (networkType == NetworkingTypes::Host)
        players[0]->Draw2D();
    
    DrawFPS(0, 0);

    pauseMenu->Draw();

    if (networkType == NetworkingTypes::Host)
        DrawText(server->GetServerIp().c_str(), 0, GetScreenHeight() - 20, 20, RED);
}

void Game::Update()
{
    if (gameState != GameStates::Game)
        return;

    switch (networkType)
    {
    case NetworkingTypes::Host:
        server->Update();

        if (server->GetNewPlayersSize() > 0)
        {
            for (int owner : server->GetNewPlayers())
            {
                AddPlayer(owner);
            }

            server->ClearNewPlayers();
        }

        break;

    case NetworkingTypes::Client:
        client->Update();

        if (client->GetNewPlayersSize() > 0)
        {
            for (int owner : client->GetNewPlayers())
            {
                AddPlayer(owner);
            }

            client->ClearNewPlayers();
        }
        break;

    default:
        break;
    }

    if (IsKeyPressed(KEY_ESCAPE))
        pauseMenu->ToggleActive();

    // Don't update the rest if paused
    if (pauseMenu->IsActive())
        return;

    world.Update();

    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1)
    {
        std::shared_ptr<Player> localPlayer = GetPlayerByOwner(client->GetClientsObjs());
        localPlayer->Update();
    }
    else if (networkType == NetworkingTypes::Host)
        players[0]->Update(); 
}

std::shared_ptr<Player> Game::GetPlayerByOwner(int t_owner)
{
    for (const std::shared_ptr<Player>& player : players)
    {
        if (player->GetOwner() == t_owner)
            return player;
    }

    return nullptr;
}

void Game::AddPlayer(int t_owner)
{
    // Create the player
    auto player = std::make_shared<Player>(t_owner, world);
    player->Init();

    // Set spawn position
    Vector3 worldCenter = Vector3Scale(world.GetSizeDimensions(), 0.5f);
    player->SetPos(worldCenter);

    // Allow mesh tracking
    MeshCollider::AddTrackedPos(player->GetPosRef());

    // Set as owner if needed
    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1 && t_owner == client->GetClientsObjs())
        player->SetOwner(true);

    players.push_back(player);
}
