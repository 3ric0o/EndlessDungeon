#pragma once
#include "Camera.h"
#include "Map.h"

class Game
{
public:
    Game(int screenWidth, int screenHeight);
    void Initialize();
    void Run();
    
private:
    void Update();
    void Draw();

    CameraController cameraController;
    Map map;
    int screenWidth;
    int screenHeight;
    static constexpr float PLAYER_RADIUS = 0.1f;
};