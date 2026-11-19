#include "game.h"

void Game::Init()
{
    Lighting::Init();
    world.Init();

    players.push_back(std::make_shared<Player>(world)); // P1
    players.push_back(std::make_shared<Player>(world)); // P2

    world.InitColliders();
    Vector3 worldCenter = Vector3Scale(world.GetSizeDimensions(), 0.5f);

    for (const std::shared_ptr<Player> player : players)
    {
        player->Init();
        MeshCollider::AddTrackedPos(player->GetPosRef());
        player->SetPos(worldCenter);
    }

    pauseMenu = std::make_shared<PauseMenu>(*players[0], world);

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
        players[1]->SetOwner(true); // TEMP
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
        BeginMode3D(players[client.GetClientsPlayer()]->GetCameraRef());
    else if (networkType == NetworkingTypes::Host)
        BeginMode3D(players[0]->GetCameraRef());

        world.Draw();

        for (const std::shared_ptr<Player> player : players)
        {
            player->Draw();
        }

    EndMode3D();

    if (networkType == NetworkingTypes::Client && client.GetClientID() != -1)
        players[client.GetClientsPlayer()]->Draw2D();
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
        break;

    case NetworkingTypes::Client:
        client.Update();
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
        players[client.GetClientsPlayer()]->Update();
    else if (networkType == NetworkingTypes::Host)
        players[0]->Update();
}
