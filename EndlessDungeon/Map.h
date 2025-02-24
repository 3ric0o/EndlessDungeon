#pragma once
#include "raylib.h"
#include <vector>

// Define cell types for the map
enum class CellType {
    WALL = 0,
    FLOOR = 1
};

// Structure to represent a room in the dungeon
struct Room {
    int x;
    int y;
    int width;
    int height;
};


class Map
{
public:
    Map();
    ~Map();
    
    void generate();
    void draw();
    void draw_minimap(const Vector2& playerPosition);
    bool check_collision(const Vector2& position, float radius);

    Vector3 get_spawn_position() const {
        if (rooms.empty()) {
            return position;  // Fallback position
        }
    
        const Room& firstRoom = rooms.front();
        // Calculate center of the first room in map coordinates
        float centerX = firstRoom.x + (firstRoom.width / 2.0f);
        float centerY = firstRoom.y + (firstRoom.height / 2.0f);
    
        return Vector3{
            position.x + centerX,  // Map position offset + room center X
            0.4f,                 // Fixed height
            position.z + centerY  // Map position offset + room center Y
        };
    }
    
private:
    // Generation helper methods
    void initializeMap();
    void createRoom(const Room& room);
    void createCorridor(int x1, int y1, int x2, int y2);
    bool isRoomValid(const Room& room) const;
    void generateMesh();
    Color* createMapPixels() const;

    // Map generation parameters
    static constexpr int MAP_WIDTH = 32;
    static constexpr int MAP_HEIGHT = 16;
    static constexpr int MIN_ROOM_SIZE = 4;
    static constexpr int MAX_ROOM_SIZE = 8;
    static constexpr int MAX_ROOMS = 8;
    
    // Original members
    Model model;
    Texture2D cubicmap;
    Texture2D texture;
    Color* mapPixels;
    Vector3 position;
    static constexpr Vector3 MAP_POSITION{ -16.0f, 0.0f, -8.0f };

    // New members for procedural generation
    std::vector<std::vector<CellType>> mapData;
    std::vector<Room> rooms;

     // Add visibility tracking
    std::vector<std::vector<bool>> visibilityMap;
    static constexpr float VISIBILITY_RADIUS = 5.0f;  // How far the player can "see"
    
    // Helper method to update visibility
    void updateVisibility(const Vector2& playerPos);
};