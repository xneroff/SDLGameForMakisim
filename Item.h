#pragma once
#include <string>
#include <SDL3/SDL.h>

struct Item {
    std::string name;
    SDL_Texture* texture;
    SDL_FRect rect;
    int price = 0;
};