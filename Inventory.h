#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>
#include "Animation.h"

struct Item {
    std::string name;
    SDL_Texture* texture;
    SDL_FRect rect;
};

class Inventory {
public:
    Inventory(SDL_Renderer* renderer);
    ~Inventory();

    void addItem(const std::string& name, const std::string& path);
    void render();
    void handleEvent(SDL_Event* event);

private:
    SDL_Renderer* renderer;
    SDL_Texture* background;
    SDL_FRect inventoryRect;
    SDL_FRect draggingItemOriginalRect;
    SDL_Texture* slotHighlight = nullptr;

    AnimationSet previewAnim;
    Animation previewHandler;
    SDL_FRect previewRect;

    std::vector<SDL_FRect> slots;
    std::vector<Item> items;

    Item* draggingItem = nullptr;
    SDL_FPoint dragOffset;

    SDL_Texture* loadTexture(const std::string& path);
};
