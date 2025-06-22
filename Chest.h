#pragma once
#include <SDL3/SDL.h>
#include <string>


// Chest.h
struct Chest {
    SDL_FRect rect;
    bool opened = false;
    std::string name, item;
    int amount;
    SDL_Texture* closedTex = nullptr;
    SDL_Texture* openedTex = nullptr;
};