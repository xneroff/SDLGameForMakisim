#include "FireballSkill.h"

FireballSkill::FireballSkill()
    : active(false),
    fireballRect{ 0, 0, WIDTH, HEIGHT },
    fireballSpeed(600.0f),
    fireballDirection(1.0f),
    traveledDistance(0.0f),
    maxDistance(500.0f),
    cooldownTime(4.0f),      // 4 секунды КД
    timeSinceLastUse(4.0f)   // можно сразу разрешить использовать
{
}


void FireballSkill::activate(Player* player) {
    // Если скилл активен или КД не прошёл — не активируем
    if (active || timeSinceLastUse < cooldownTime)
        return;

    SDL_FRect playerRect = player->getDest();

    float fireballX = playerRect.x + (playerRect.w / 2) - (WIDTH / 2);
    float fireballY = playerRect.y + (playerRect.h / 2) - (HEIGHT / 2);

    fireballRect = { fireballX, fireballY, WIDTH, HEIGHT };
    fireballDirection = player->isFlipped() ? -1.0f : 1.0f;

    traveledDistance = 0.0f;
    active = true;

    timeSinceLastUse = 0.0f; // сбрасываем таймер КД при активации
}


void FireballSkill::update(Player* player, float deltaTime) {
    // Обновляем таймер КД
    if (timeSinceLastUse < cooldownTime)
        timeSinceLastUse += deltaTime;

    if (!active) return;

    float dx = fireballSpeed * fireballDirection * deltaTime;

    SDL_FRect nextPos = fireballRect;
    nextPos.x += dx;

    if (!player->checkCollisionForRect(nextPos)) {
        fireballRect = nextPos;
        traveledDistance += std::abs(dx);

        if (traveledDistance >= maxDistance) {
            active = false;
        }
    }
    else {
        active = false;
    }
}


void FireballSkill::render(SDL_Renderer* renderer, Camera* camera) {
    if (!active) return;

    SDL_FRect screenRect = camera->apply(fireballRect);

    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderFillRect(renderer, &screenRect);
}