#include "PlayerHealthHUD.h"
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

PlayerHealthHUD::PlayerHealthHUD(SDL_Renderer* renderer, TTF_Font* font, int health, int totalHealth)
    : renderer(renderer), font(font), Health(health), TotalHealth(totalHealth), DisplayedHealth(static_cast<float>(health)), Texture(nullptr)
{
    destHealthBar = { 20, 20, 600, 60 };
    destBorderHealthBar = { 22, 22, 596, 56 };
    destTotalHealthBar = { 22, 22, 596, 56 };

    float percent = DisplayedHealth / TotalHealth;
    destHealthBar.w = static_cast<int>(600 * percent);

    updateText();
}

PlayerHealthHUD::~PlayerHealthHUD() {
    if (Texture) {
        SDL_DestroyTexture(Texture);
        Texture = nullptr;
    }
}

void PlayerHealthHUD::setHealth(int newHealth) {
    Health = newHealth;
    SDL_Log("setHealth called, newHealth = %d", newHealth);
}


void PlayerHealthHUD::update(float deltaTime) {
    SDL_Log("update called: DisplayedHealth = %.2f, Health = %d", DisplayedHealth, Health);

    if (DisplayedHealth != Health) {
        float diff = Health - DisplayedHealth;
        float step = animationSpeed * deltaTime;

        if (std::fabs(diff) <= step) {
            DisplayedHealth = static_cast<float>(Health);
        }
        else {
            DisplayedHealth += (diff > 0 ? 1 : -1) * step;
        }

        float percent = DisplayedHealth / TotalHealth;
        destHealthBar.w = static_cast<int>(600 * percent);

        updateText();

        SDL_Log("DisplayedHealth updated: %.2f", DisplayedHealth);
    }
}


void PlayerHealthHUD::otrisovka() const {
    // SDL3 использует SDL_FRect
    SDL_FRect border = { static_cast<float>(destBorderHealthBar.x), static_cast<float>(destBorderHealthBar.y),
                         static_cast<float>(destBorderHealthBar.w), static_cast<float>(destBorderHealthBar.h) };

    SDL_FRect total = { static_cast<float>(destTotalHealthBar.x), static_cast<float>(destTotalHealthBar.y),
                        static_cast<float>(destTotalHealthBar.w), static_cast<float>(destTotalHealthBar.h) };

    SDL_FRect health = { static_cast<float>(destHealthBar.x), static_cast<float>(destHealthBar.y),
                         static_cast<float>(destHealthBar.w), static_cast<float>(destHealthBar.h) };

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &border);

    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_RenderFillRect(renderer, &total);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &health);

    if (Texture)
        SDL_RenderTexture(renderer, Texture, nullptr, &destText);
}

void PlayerHealthHUD::obnovlenieHUD(float deltaTime) {
    update(deltaTime);
}


void PlayerHealthHUD::updateText() {
    int shownHealth = static_cast<int>(DisplayedHealth + 0.5f);
    Text = std::to_string(shownHealth) + "/" + std::to_string(TotalHealth);

    if (Texture) {
        SDL_DestroyTexture(Texture);
        Texture = nullptr;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, Text.c_str(), Text.length(), TextColor);

    Texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!Texture) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    destText.w = static_cast<float>(surface->w);
    destText.h = static_cast<float>(surface->h);
    destText.x = static_cast<float>(destBorderHealthBar.x + (destBorderHealthBar.w - surface->w) / 2);
    destText.y = static_cast<float>(destBorderHealthBar.y + (destBorderHealthBar.h - surface->h) / 2);


    SDL_DestroySurface(surface);
}
