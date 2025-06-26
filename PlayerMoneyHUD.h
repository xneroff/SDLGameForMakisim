#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
class PlayerMoneyHUD
{
public:
    PlayerMoneyHUD(SDL_Renderer* renderer, TTF_Font* font);
    ~PlayerMoneyHUD();
    void obnovlenieHUD();
    void otrisovka() const;
    void setMoney(int newMoney) { money = newMoney; };
    void changeMoney();

private:
    void obnovlenieText();
    int money;
    std::string Text;
    SDL_Color TextColor = { 255, 255, 255, 255 };
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_FRect dest;
    SDL_Texture* Texture;
};