#pragma once
#include "World.h"

class Game
{
public:
    void Init();
    void Draw();
    void Update();

private:
    void SetupCamera();
    Camera camera;

    Model model;
    Mesh mesh;

    World world;
};