#pragma once
#include "raylib.h"

class Weapon {
public:
    Weapon();
    ~Weapon();
    void Initialize();
    void Update();
    void Draw();
    void Unload();

private:
    static constexpr int TOTAL_FRAMES = 5;
    static constexpr float ANIMATION_FRAME_TIME = 0.05f;
    static constexpr float RELOAD_TIME = 2.0f;  
    static constexpr int MAGAZINE_SIZE = 6;     
    static constexpr int MAX_TOTAL_AMMO = 64;   
    static constexpr int STARTING_TOTAL_AMMO = 18; 
    
    Texture2D pistolTextures[TOTAL_FRAMES];
    Sound shootSound;
    bool soundLoaded;
    
    bool isShooting;
    bool isReloading;
    int currentFrame;
    float frameTimer;
    float reloadTimer;
    
    int currentAmmo;   
    int totalAmmo;   
    
    void LoadTextures();
    void GunSound();
    void DrawCrosshair();
    void DrawAmmoCounter();
    void Reload();
};