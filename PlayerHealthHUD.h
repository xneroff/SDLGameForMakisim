#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class PlayerHealthHUD {
public:
    PlayerHealthHUD(SDL_Renderer* renderer, TTF_Font* font, int health, int totalHealth);
    ~PlayerHealthHUD();

    void setHealth(int newHealth);
    void update(float deltaTime);
    void otrisovka() const;
    void obnovlenieHUD(float deltaTime);


private:
    SDL_Renderer* renderer;
    TTF_Font* font;

    int Health;
    int TotalHealth;
    float DisplayedHealth;

    SDL_Texture* Texture = nullptr;
    SDL_Rect destHealthBar;
    SDL_Rect destBorderHealthBar;
    SDL_Rect destTotalHealthBar;
    SDL_FRect destText; // Áûכמ SDL_Rect
    SDL_Color TextColor{ 255, 255, 255, 255 };

    float animationSpeed = 200.0f;
    std::string Text;

    void updateText();
};
