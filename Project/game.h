#ifndef GAME_H
#define GAME_H

class Game
{
public:
    void Init();
    void Draw();
    void Update();

private:
    Camera camera;

    Model model;
    Mesh mesh;
};

#endif // GAME_H