#include "Camera.h"
#include <raymath.h>

CameraController::CameraController(Map& mapRef) : map(mapRef)
{
}

Vector3 CameraController::get_spawn_position()
{
    // Get spawn position directly from map
    return map.get_spawn_position();
}

void CameraController::initialize()
{
    // Get spawn position
    Vector3 spawnPos = get_spawn_position();
    
    camera.position = spawnPos;
    // Set initial target slightly ahead of spawn position
    camera.target = Vector3{ spawnPos.x + 0.1f, spawnPos.y, spawnPos.z };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    oldPosition = camera.position;
}

void CameraController::update()
{
    oldPosition = camera.position;
    update_camera_normalized();
}

void CameraController::update_camera_angle()
{
    float mouseOffsetX = GetMouseDelta().x * 0.003f;
    Vector3 forward = Vector3Subtract(camera.target, camera.position);
    float rotationAngle = atan2f(forward.x, forward.z);
    rotationAngle -= mouseOffsetX;
    forward.x = sinf(rotationAngle);
    forward.z = cosf(rotationAngle);
    forward = Vector3Normalize(forward);
    camera.target = Vector3Add(camera.position, forward);
}

void CameraController::update_camera_normalized()
{
    Vector3 direction = {};
    if (IsKeyDown(KEY_W)) direction.z += 1.0f;
    if (IsKeyDown(KEY_S)) direction.z -= 1.0f;
    if (IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_D)) direction.x += 1.0f;

    if (Vector3Length(direction) > 0)
    {
        direction = Vector3Normalize(direction);
    }

    direction.x *= MOVE_SPEED;
    direction.z *= MOVE_SPEED;

    Vector3 forward = Vector3Subtract(camera.target, camera.position);
    forward.y = 0;
    forward = Vector3Normalize(forward);
    Vector3 right = Vector3CrossProduct(forward, camera.up);

    Vector3 newPosition = Vector3Add(camera.position,
        Vector3Add(
            Vector3Scale(right, direction.x),
            Vector3Scale(forward, direction.z)));

    Vector2 position2D = { newPosition.x, newPosition.z };
    if (map.check_collision(position2D, 0.1f))
    {
        return; // Can't move, hit a wall
    }
    camera.position = newPosition;
    camera.target = Vector3Add(camera.position, forward);

    if (GetMouseDelta().x != 0.0f)
    {
        update_camera_angle();
    }
}