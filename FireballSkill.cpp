#include "FireballSkill.h"

FireballSkill::FireballSkill()
    : active(false),
    fireballRect{ 0, 0, WIDTH, HEIGHT },
    fireballSpeed(600.0f),
    fireballDirection(1.0f),
    traveledDistance(0.0f),
    maxDistance(500.0f)
{
}

void FireballSkill::activate(Player* player) {
    if (active) return;

    SDL_FRect playerRect = player->getDest();

    float fireballX = playerRect.x + (playerRect.w / 2) - (WIDTH / 2);
    float fireballY = playerRect.y + (playerRect.h / 2) - (HEIGHT / 2);

    fireballRect = { fireballX, fireballY, WIDTH, HEIGHT };
    fireballDirection = player->isFlipped() ? -1.0f : 1.0f;

    traveledDistance = 0.0f;
    active = true;
}

void FireballSkill::update(Player* player, float deltaTime) {
    if (!active) return;

    float dx = fireballSpeed * fireballDirection * deltaTime;

    // Позиция после перемещения
    SDL_FRect nextPos = fireballRect;
    nextPos.x += dx;

    // Проверка коллизии через player
    if (!player->checkCollisionForRect(nextPos)) {
        fireballRect = nextPos;
        traveledDistance += std::abs(dx);

        if (traveledDistance >= maxDistance) {
            active = false;
        }
    }
    else {
        // Если столкнулся — отключаем
        active = false;
    }
}

void FireballSkill::render(SDL_Renderer* renderer, Camera* camera) {
    if (!active) return;

    SDL_FRect screenRect = camera->apply(fireballRect);

    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderFillRect(renderer, &screenRect);
}
