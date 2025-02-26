#include "Game.h"

Game::Game(int width, int height) : cameraController(map), screenWidth(width), screenHeight(height)
{
    InitWindow(screenWidth, screenHeight, "Endless Dungeon");
    DisableCursor();
    SetTargetFPS(60);
}

void Game::Initialize()
{
    map.generate();
    cameraController.initialize();
    weapon.Initialize();
}

void Game::Run()
{
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
    CloseWindow();
}

void Game::Update()
{
    if (IsKeyPressed(KEY_SPACE))
    {
        map.generate();
        cameraController.initialize();
    }
    
    cameraController.update();
    weapon.Update();
}

void Game::Draw()
{
    BeginDrawing();
    ClearBackground(BLACK);
    
    BeginMode3D(cameraController.GetCamera());
    map.draw();
    EndMode3D();

    weapon.Draw();
    
    Vector2 playerPos = { cameraController.GetCamera().position.x,cameraController.GetCamera().position.z };
    map.draw_minimap(playerPos);
    
    EndDrawing();
}