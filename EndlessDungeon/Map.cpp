#include "Map.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

Map::Map() : model(), texture(), position(MAP_POSITION)
{
    // Initialize the map data
    mapData.resize(MAP_HEIGHT, std::vector<CellType>(MAP_WIDTH, CellType::WALL));
    srand(static_cast<unsigned>(time(nullptr)));
}

Map::~Map()
{
    UnloadTexture(texture);
    UnloadModel(model);
}

void Map::generate()
{
    int attempts = 0;
    const int MAX_ATTEMPTS = 100;  // Prevent infinite loops
    
    do
        {
        initializeMap();
        
        // Generate rooms
        for (int i = 0; i < MAX_ROOMS; i++)
        {
            // Generate random room dimensions and position
            int width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
            int height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
            int x = rand() % (MAP_WIDTH - width - 2) + 1;
            int y = rand() % (MAP_HEIGHT - height - 2) + 1;

            Room newRoom{x, y, width, height};

            if (isRoomValid(newRoom))
            {
                createRoom(newRoom);
                
                // Connect to previous room
                if (!rooms.empty())
                {
                    // Get center points of rooms
                    int newX = newRoom.x + newRoom.width / 2;
                    int newY = newRoom.y + newRoom.height / 2;
                    int prevX = rooms.back().x + rooms.back().width / 2;
                    int prevY = rooms.back().y + rooms.back().height / 2;

                    // Randomly decide whether to do horizontal or vertical corridor first
                    if (rand() % 2 == 0)
                    {
                        createCorridor(prevX, prevY, newX, prevY);
                        createCorridor(newX, prevY, newX, newY);
                    }
                    else
                    {
                        createCorridor(prevX, prevY, prevX, newY);
                        createCorridor(prevX, newY, newX, newY);
                    }
                }

                rooms.push_back(newRoom);
            }
        }
        
        attempts++;
    }
    while (rooms.size() < 6 && attempts < MAX_ATTEMPTS);  // Ensure at least 4 rooms

    if (rooms.size() < 6)
        {
        initializeMap();
        }

    // Generate the 3D mesh from the map data
    generateMesh();
    
    // Initialize visibility map
    visibilityMap = std::vector<std::vector<bool>>(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));
    
    // Make the spawn room visible initially
    if (!rooms.empty())
        {
        const Room& firstRoom = rooms.front();
        for (int y = firstRoom.y; y < firstRoom.y + firstRoom.height; y++)
            {
            for (int x = firstRoom.x; x < firstRoom.x + firstRoom.width; x++)
                {
                visibilityMap[y][x] = true;
            }
        }
    }
}

void Map::initializeMap()
{
    // Fill the map with walls
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            mapData[y][x] = CellType::WALL;
        }
    }
    rooms.clear();
}

void Map::createRoom(const Room& room)
{
    for (int y = room.y; y < room.y + room.height; y++)
    {
        for (int x = room.x; x < room.x + room.width; x++)
        {
            mapData[y][x] = CellType::FLOOR;
        }
    }
}

void Map::createCorridor(int x1, int y1, int x2, int y2)
{
    // Create a corridor between two points
    for (int x = std::min(x1, x2); x <= std::max(x1, x2); x++)
    {
        mapData[y1][x] = CellType::FLOOR;
    }
    for (int y = std::min(y1, y2); y <= std::max(y1, y2); y++)
    {
        mapData[y][x2] = CellType::FLOOR;
    }
}

bool Map::isRoomValid(const Room& room) const
{
    // Check if the room fits within the map with padding
    if (room.x < 1 || room.y < 1 || 
        room.x + room.width >= MAP_WIDTH - 1 || 
        room.y + room.height >= MAP_HEIGHT - 1)
        return false;

    // Check if the room overlaps with any existing rooms (including padding)
    for (int y = room.y - 1; y < room.y + room.height + 1; y++)
    {
        for (int x = room.x - 1; x < room.x + room.width + 1; x++)
        {
            if (mapData[y][x] == CellType::FLOOR)
                return false;
        }
    }

    return true;
}

void Map::generateMesh()
{
    // Create a temporary image for the map
    Color* mapPixels = createMapPixels();
    Image imMap = { 0 }; 
    imMap.data = mapPixels;
    imMap.width = MAP_WIDTH;
    imMap.height = MAP_HEIGHT;
    imMap.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    imMap.mipmaps = 1;

    // Generate the 3D mesh
    Mesh mesh = GenMeshCubicmap(imMap, Vector3{ 1.0f, 1.0f, 1.0f });
    model = LoadModelFromMesh(mesh);

    // Load or create the texture for the walls
    texture = LoadTexture("resources/cubicmap_atlas.png"); 
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    // Clean up
    RL_FREE(mapPixels);
}

Color* Map::createMapPixels() const
{
    Color* pixels = (Color*)RL_MALLOC(MAP_WIDTH * MAP_HEIGHT * sizeof(Color));
    
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            pixels[y * MAP_WIDTH + x] = (mapData[y][x] == CellType::WALL) ?
                Color{255, 255, 255, 255} :  // White for walls
                Color{0, 0, 0, 255};         // Black for floors
        }
    }
    
    return pixels;
}

void Map::draw()
{
    DrawModel(model, position, 1.0f, WHITE);
}

void Map::updateVisibility(const Vector2& playerPos)
{
    // Convert world position to map coordinates
    int playerCellX = static_cast<int>(playerPos.x - position.x + 0.5f);
    int playerCellY = static_cast<int>(playerPos.y - position.z + 0.5f);
    
    // Update visibility around player
    for (int y = -VISIBILITY_RADIUS; y <= VISIBILITY_RADIUS; y++)
    {
        for (int x = -VISIBILITY_RADIUS; x <= VISIBILITY_RADIUS; x++)
        {
            int checkX = playerCellX + x;
            int checkY = playerCellY + y;
            
            // Check if within map bounds
            if (checkX >= 0 && checkX < MAP_WIDTH && 
                checkY >= 0 && checkY < MAP_HEIGHT)
            {
                
                // Check if within visibility radius
                float distance = sqrtf(x*x + y*y);
                if (distance <= VISIBILITY_RADIUS)
                {
                    // Simple line of sight check
                    bool hasLineOfSight = true;
                    float dx = static_cast<float>(x);
                    float dy = static_cast<float>(y);
                    float steps = distance * 2;
                    
                    for (float i = 0; i < steps; i++)
                    {
                        int checkStepX = playerCellX + static_cast<int>(dx * (i / steps));
                        int checkStepY = playerCellY + static_cast<int>(dy * (i / steps));
                        
                        if (mapData[checkStepY][checkStepX] == CellType::WALL)
                        {
                            hasLineOfSight = false;
                            break;
                        }
                    }
                    
                    if (hasLineOfSight)
                    {
                        visibilityMap[checkY][checkX] = true;
                    }
                }
            }
        }
    }
}

void Map::draw_minimap(const Vector2& playerPosition)
{
    const int MINIMAP_SCALE = 4;
    Vector2 minimapPos = { static_cast<float>(GetScreenWidth() - MAP_WIDTH * MINIMAP_SCALE - 20), 20.0f };
    
    // Update visibility before drawing
    updateVisibility(playerPosition);
    
    // Draw the minimap background
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (visibilityMap[y][x])
            {
                Color cellColor = (mapData[y][x] == CellType::WALL) ? WHITE : BLACK;
                DrawRectangle(
                    static_cast<int>(minimapPos.x + x * MINIMAP_SCALE),
                    static_cast<int>(minimapPos.y + y * MINIMAP_SCALE),
                    MINIMAP_SCALE,
                    MINIMAP_SCALE,
                    cellColor
                );
            }
            else
            {
                // Draw fog of war
                DrawRectangle(
                    static_cast<int>(minimapPos.x + x * MINIMAP_SCALE),
                    static_cast<int>(minimapPos.y + y * MINIMAP_SCALE),
                    MINIMAP_SCALE,
                    MINIMAP_SCALE,
                    DARKGRAY  // Fog of war color
                );
            }
        }
    }

    // Draw minimap border
    DrawRectangleLines(
        static_cast<int>(minimapPos.x),
        static_cast<int>(minimapPos.y),
        MAP_WIDTH * MINIMAP_SCALE,
        MAP_HEIGHT * MINIMAP_SCALE,
        GREEN
    );

    // Draw player position
    int playerCellX = static_cast<int>(playerPosition.x - position.x + 0.5f);
    int playerCellY = static_cast<int>(playerPosition.y - position.z + 0.5f);
    
    playerCellX = std::max(0, std::min(playerCellX, MAP_WIDTH - 1));
    playerCellY = std::max(0, std::min(playerCellY, MAP_HEIGHT - 1));

    DrawRectangle(
        static_cast<int>(minimapPos.x + playerCellX * MINIMAP_SCALE),
        static_cast<int>(minimapPos.y + playerCellY * MINIMAP_SCALE),
        MINIMAP_SCALE,
        MINIMAP_SCALE,
        RED
    );
}

bool Map::check_collision(const Vector2& playerPos, float playerRadius)
{
    int playerCellX = static_cast<int>(playerPos.x - position.x + 0.5f);
    int playerCellY = static_cast<int>(playerPos.y - position.z + 0.5f);

    playerCellX = std::max(0, std::min(playerCellX, MAP_WIDTH - 1));
    playerCellY = std::max(0, std::min(playerCellY, MAP_HEIGHT - 1));

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (mapData[y][x] == CellType::WALL)
            {
                Rectangle cellBounds{
                    position.x - 0.5f + x * 1.0f,
                    position.z - 0.5f + y * 1.0f,
                    1.0f,
                    1.0f
                };

                if (CheckCollisionCircleRec(playerPos, playerRadius, cellBounds))
                {
                    return true;
                }
            }
        }
    }
    return false;
}