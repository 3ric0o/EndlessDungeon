#pragma once
#include "raylib.h"
#include "Map.h"

class CameraController
{
public:
    CameraController(Map& mapRef);
    void initialize();
    void update();
    Camera GetCamera() { return camera; }

private:
    void update_camera_angle();
    void update_camera_normalized();
    Vector3 get_spawn_position(); 

    static constexpr float MOVE_SPEED = 0.1f;
    Camera camera;
    Vector3 oldPosition;
    Map& map;
};