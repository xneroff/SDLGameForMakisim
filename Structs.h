#pragma once
#include <SDL3/SDL.h>
#include <string>

struct AnimationSet {
    SDL_Texture* texture;
    int frameCount;
    int frameDelay;
};
struct InventorySlot {
    SDL_Texture* icon = nullptr;
    int count = 0;
};