#include "raylib.h"
#include "stdio.h"

#include "game.h"

const int screenWidth = 800;
const int screenHeight = 600;

void GameLoop(void);

Game game;

int main(void)
{

    InitWindow(screenWidth, screenHeight, "Raylib StarterKit GPPI");

    // Initialise Game
    game.Init();

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
char message[11];

void GameLoop(void)
{
    BeginDrawing();

    // Update Game Data
    // Should be outside BeginDrawing(); and EndDrawing();
    game.Update();

    ClearBackground(BLACK);

    // Draw the Game Objects
    game.Draw();

    counter++;

    EndDrawing();
}