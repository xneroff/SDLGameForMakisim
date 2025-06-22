#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "PlayerMoneyHUD.h"
#include "PlayerHealthHUD.h"

class Interface {
public:
    Interface(SDL_Renderer* renderer, TTF_Font* font, int currentHealth, int TotalHealth);
    ~Interface();

    void obnovlenieHUD(float deltaTime);
    void otrisovka() const;

    void setMoney(int newMoney);
    void setHealth(int hp);

private:
    PlayerMoneyHUD* playerMoneyHUD;
    PlayerHealthHUD* playerHealthHUD;

    SDL_Renderer* renderer;
    TTF_Font* font;
};