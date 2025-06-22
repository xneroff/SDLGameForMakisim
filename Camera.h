#pragma once
#include <SDL3/SDL.h>

class Camera {
public:
    Camera(float screenWidth, float screenHeight);
    ~Camera();

    // Добавляем параметры размеров тайлов
    void update(const SDL_FRect& player, float mapWidth, float mapHeight, int tileWidth, int tileHeight);

    SDL_FRect getView() const { return camera; }
    SDL_FRect apply(const SDL_FRect& worldRect) const;

    float zoom = 2.0f;

private:
    SDL_FRect camera;
    float screenW, screenH;
};
