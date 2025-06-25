#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>  // добавь для вектора
#include "Player.h"
#include "MainMenu.h"
#include "Camera.h"
#include "TileMap.h"
#include "Enemy.h"   // заменили Dummy на Enemy
#include "TileMap.h"
#include "StartMenu.h"
struct FloatingText {
    std::string text;
    SDL_FPoint position;
    Uint64 startTime;    
    Uint32 duration;    

    FloatingText(const std::string& t, SDL_FPoint pos, Uint64 start, Uint32 dur)
        : text(t), position(pos), startTime(start), duration(dur) {
    }
};

extern std::vector<FloatingText> floatingTexts;

class Game
{
public:
    Game();
    ~Game();

    SDL_AppResult SDL_AppInit();
    SDL_AppResult SDL_AppEvent(SDL_Event* event);
    SDL_AppResult SDL_AppIterate();
    void SDL_AppQuit(SDL_AppResult result);
    TileMap* getTileMap() { return tileMap; }
    bool& getTeleportDialogFlag() { return teleportConfirmDialogOpen; }
    // ✅ вот так — МОЖНО присваивать
    Portal& getPendingTeleport() { return pendingTeleport; }
    const Portal* lastPortalInRange = nullptr;
private:
  

    bool isTeleporting = false;
    std::string teleportTargetMap = "";
    std::string teleportTargetSpawn = "";
    float teleportTimer = 0.0f;


    StartMenu* startMenu = nullptr;

    bool showMenu = false;
    bool quit = false;
    bool started = false; // Игра еще не начата — показываем главное меню
    bool startGame = false;
    bool quitGame = false;
    bool showStartMenu = true;  // Показывать стартовое меню (StartMenu)
    bool shouldLoadNextMap = false;
    SDL_Texture* dialogBoxTexture = nullptr;



    bool teleportConfirmDialogOpen = false;
    Portal pendingTeleport;
    void startTeleport(const std::string& map, const std::string& spawn);
    TTF_Font* dialogFont = nullptr;
    TTF_Font* dialogNameFont = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Player* player = nullptr;
    TTF_Font* font;
    MainMenu* menu = nullptr;
    Camera* camera = nullptr;
    const Uint32 restartDelay = 5000;
    TileMap* tileMap = nullptr;
    bool gameOver = false;
    Uint64 deathTime = 0;

    std::vector<Enemy*> enemies;  // вместо одного dummy — вектор врагов

    SDL_FRect getWindowSize();
};
