#include "game.h"

void Game::Init()
{
    Lighting::Init();
    world.Init();


    world.InitColliders();
    Vector3 worldCenter = Vector3Scale(world.GetSizeDimensions(), 0.5f);

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
    bool optionPicked = false;
    Rectangle rect;
    rect.width = GetScreenWidth();
    rect.height = GetScreenHeight() / 2.0f;
    rect.x = 0.0f;
    rect.y = 0.0f;
    if (GuiButton(rect, "Host"))
    {
        networkType = NetworkingTypes::Host;
        server.Init();
        optionPicked = true;
        players[0]->SetOwner(true); // TEMP
    }

    rect.y += GetScreenHeight() / 2.0f;
    if (GuiButton(rect, "Join"))
    {
        networkType = NetworkingTypes::Client;
        client.Init();
        optionPicked = true;
    }

    if (optionPicked)
    {
        gameState = GameStates::Game;
        DisableCursor();
    }
}

void Game::DrawJoinGame()
{
}

void Game::DrawGame()
{
    if (networkType == NetworkingTypes::Client && client.GetClientID() != -1)
        BeginMode3D(players[client.GetClientsObjs()]->GetCameraRef());
    else if (networkType == NetworkingTypes::Host)
        BeginMode3D(players[0]->GetCameraRef());

        world.Draw();

        for (const std::shared_ptr<Player> player : players)
        {
            player->Draw();
        }

    EndMode3D();

    if (networkType == NetworkingTypes::Client && client.GetClientID() != -1)
        players[client.GetClientsObjs()]->Draw2D();
    else if (networkType == NetworkingTypes::Host)
        players[0]->Draw2D();
    
    DrawFPS(0, 0);

    pauseMenu->Draw();

    if (networkType == NetworkingTypes::Host)
        DrawText(server.GetServerIp().c_str(), 0, GetScreenHeight() - 20, 20, RED);
}

void Game::Update()
{
    if (gameState != GameStates::Game)
        return;

    switch (networkType)
    {
    case NetworkingTypes::Host:
        server.Update();

        if (server.GetNewPlayersSize() > 0)
        {
            for (int owner : server.GetNewPlayers())
            {
                AddPlayer(owner);
            }

            server.ClearNewPlayers();
        }

        break;

    case NetworkingTypes::Client:
        client.Update();

        if (client.GetNewPlayersSize() > 0)
        {
            for (int owner : client.GetNewPlayers())
            {
                AddPlayer(owner);
            }

            client.ClearNewPlayers();
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

    if (networkType == NetworkingTypes::Client && client.GetClientID() != -1)
        players[client.GetClientsObjs()]->Update();
    else if (networkType == NetworkingTypes::Host)
        players[0]->Update();
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
    if (networkType == NetworkingTypes::Client && client.GetClientID() != -1 && t_owner == client.GetClientsObjs())
        player->SetOwner(true);

    players.push_back(player);
}
