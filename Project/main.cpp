#include "raylib.h"
#include "stdio.h"

#define RAYGUI_IMPLEMENTATION

#include "game.h"
#include "StructureBuilding.h"

const int screenWidth = 800;
const int screenHeight = 600;

void GameLoop(void);

enum class LaunchTypes
{
    Game,
    StructureBuilding
};

static const LaunchTypes LAUNCH_TYPE = LaunchTypes::StructureBuilding;
StructureBuilding structureBuilding;
Game game;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Raylib StarterKit GPPI");
    ToggleFullscreen();

    SetExitKey(KEY_L);

    // Initialise
    switch (LAUNCH_TYPE)
    {
    case LaunchTypes::Game:
        game.Init();
        break;
    case LaunchTypes::StructureBuilding:
        structureBuilding.Init();
        break;
    default:
        break;
    }

    // For web builds, do not use WindowShouldClose
    // see https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)#41-avoid-raylib-whilewindowshouldclose-loop

#if defined(WEB_BUILD)
    emscripten_set_main_loop(GameLoop, 0, 1);
#else
    SetTargetFPS(120);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Call GameLoop
        GameLoop();
    }
#endif


    CloseWindow();

    return 0;
}

int counter = 0;

void GameLoop(void)
{
    BeginDrawing();

    // Update Game Data
    switch (LAUNCH_TYPE)
    {
    case LaunchTypes::Game:
        game.Update();
        break;
    case LaunchTypes::StructureBuilding:
        structureBuilding.Update();
        break;
    default:
        break;
    }

    if (IsKeyPressed(KEY_F))
    {
        ToggleFullscreen();
    }

    ClearBackground(BLACK);

    // Draw the Game Objects
    switch (LAUNCH_TYPE)
    {
    case LaunchTypes::Game:
        game.Draw();
        break;
    case LaunchTypes::StructureBuilding:
        structureBuilding.Draw();
        break;
    default:
        break;
    }
    counter++;

    EndDrawing();
}