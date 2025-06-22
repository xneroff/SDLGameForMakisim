#include "Camera.h"

Camera::Camera(float screenWidth, float screenHeight)
    : screenW(screenWidth), screenH(screenHeight)
{
    camera = { 0, 0, screenWidth , screenHeight };
}
Camera::~Camera() {}

void Camera::update(const SDL_FRect& player, float mapWidth, float mapHeight, int tileWidth, int tileHeight) {
    camera.w = screenW / zoom;
    camera.h = screenH / zoom;

    float smoothing = 0.08f;
    float targetX = player.x + player.w / 2 - camera.w / 2;
    float targetY = player.y + player.h / 2 - camera.h / 2;

    float maxX = mapWidth * tileWidth - camera.w;
    float maxY = mapHeight * tileHeight - camera.h;

    camera.x += (targetX - camera.x) * smoothing;
    camera.y += (targetY - camera.y) * smoothing;

    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x > maxX) camera.x = maxX;
    if (camera.y > maxY) camera.y = maxY;
}

SDL_FRect Camera::apply(const SDL_FRect& worldRect) const {
    return SDL_FRect{
        (worldRect.x - camera.x) * zoom,
        (worldRect.y - camera.y) * zoom,
        worldRect.w * zoom,
        worldRect.h * zoom
    };
}
