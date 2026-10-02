#pragma once
#include "Chunk.h"

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

    Chunk world[3][3][3];
};