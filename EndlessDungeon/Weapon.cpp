#include "Weapon.h"

#include <cstdio>

Weapon::Weapon() : 
    isShooting(false), 
    isReloading(false),
    currentFrame(0), 
    frameTimer(0.0f),
    reloadTimer(0.0f),
    soundLoaded(false),
    currentAmmo(MAGAZINE_SIZE),        
    totalAmmo(STARTING_TOTAL_AMMO - MAGAZINE_SIZE)  
{}

Weapon::~Weapon()
{
    Unload();
}

void Weapon::Initialize()
{
    InitAudioDevice();
    LoadTextures();
    GunSound();
}

void Weapon::LoadTextures()
{
    for (int i = 0; i < TOTAL_FRAMES; i++)
        {
        char filename[256];
        sprintf_s(filename, "resources/pistol%d.png", i + 1);
        pistolTextures[i] = LoadTexture(filename);
    }
}

void Weapon::GunSound() { shootSound = LoadSound("resources/GunShot.wav"); }

void Weapon::Update()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !isShooting && !isReloading && currentAmmo > 0)
    {
        isShooting = true;
        currentFrame = 0;
        frameTimer = 0.0f;
        PlaySound(shootSound);
        currentAmmo--;
    }
    
    // Auto-reload when magazine is empty
    if (currentAmmo == 0 && !isReloading && totalAmmo > 0)
    {
        isReloading = true;
        reloadTimer = RELOAD_TIME;
    }
    
    // Manual reload with R key
    if (IsKeyPressed(KEY_R) && !isReloading && currentAmmo < MAGAZINE_SIZE && totalAmmo > 0)
    {
        isReloading = true;
        reloadTimer = RELOAD_TIME;
    }
    
    // Handle reloading
    if (isReloading)
    {
        reloadTimer -= GetFrameTime();
        if (reloadTimer <= 0)
        {
            Reload();
        }
    }
    
    if (isShooting)
    {
        frameTimer += GetFrameTime();
        
        if (frameTimer >= ANIMATION_FRAME_TIME)
        {
            frameTimer = 0.0f;
            currentFrame++;
            
            if (currentFrame >= TOTAL_FRAMES)
            {
                currentFrame = 0;
                isShooting = false;
            }
        }
    }
}
void Weapon::Reload()
{
    int ammoNeeded = MAGAZINE_SIZE - currentAmmo;
    int ammoToAdd = (ammoNeeded < totalAmmo) ? ammoNeeded : totalAmmo;
    
    currentAmmo += ammoToAdd;
    totalAmmo -= ammoToAdd;
    
    isReloading = false;
}

void Weapon::DrawAmmoCounter()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    const int MARGIN = 20;
    const int fontSize = 30;
    
    char ammoText[32];
    sprintf_s(ammoText, "%d/%d", currentAmmo, totalAmmo);
    
    Vector2 textSize = MeasureTextEx(GetFontDefault(), ammoText, fontSize, 1);
    
    DrawText(ammoText, 
        screenWidth - textSize.x - MARGIN, 
        screenHeight - fontSize - MARGIN,
        fontSize, 
        WHITE);
    
    if (isReloading)
    {
        const char* reloadText = "RELOADING";
        Vector2 reloadTextSize = MeasureTextEx(GetFontDefault(), reloadText, fontSize, 1);
        
        DrawText(reloadText,
            screenWidth - reloadTextSize.x - MARGIN,
            screenHeight - (fontSize * 2) - MARGIN - 5,
            fontSize,
            RED);
    }
}

void Weapon::Draw()
{
    DrawCrosshair();
    DrawAmmoCounter();
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    const float WEAPON_SCALE = 4.0f; 
    Texture2D currentTexture = pistolTextures[currentFrame];
    
    float weaponWidth = currentTexture.width * WEAPON_SCALE;
    float weaponHeight = currentTexture.height * WEAPON_SCALE;
    
    float posX = screenWidth - weaponWidth - 70; 
    float posY = screenHeight - weaponHeight ;  
    
    // Draw the current frame
    DrawTextureEx(
        currentTexture,
        { posX, posY },
        0.0f,  // rotation
        WEAPON_SCALE,
        WHITE
    );
}

void Weapon::Unload()
{
    for (int i = 0; i < TOTAL_FRAMES; i++)
    {
        UnloadTexture(pistolTextures[i]);
    }
    
    UnloadSound(shootSound);
}
void Weapon::DrawCrosshair()
{
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2;
    const int SIZE = 10;
    const int GAP = 4;
    const int THICKNESS = 2;
    const Color CROSSHAIR_COLOR = RAYWHITE;
    
    // Horizontal lines
    DrawRectangle(centerX - SIZE - GAP, centerY - THICKNESS/2, SIZE, THICKNESS, CROSSHAIR_COLOR);
    DrawRectangle(centerX + GAP, centerY - THICKNESS/2, SIZE, THICKNESS, CROSSHAIR_COLOR);
    
    // Vertical lines
    DrawRectangle(centerX - THICKNESS/2, centerY - SIZE - GAP, THICKNESS, SIZE, CROSSHAIR_COLOR);
    DrawRectangle(centerX - THICKNESS/2, centerY + GAP, THICKNESS, SIZE, CROSSHAIR_COLOR);
}