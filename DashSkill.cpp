#include "DashSkill.h"
#include "Player.h"

DashSkill::DashSkill()
    : isDashing(false), dashTime(0.0f), dashDuration(200.0f),
    dashSpeed(1000.0f), direction(1), lastTime(0), lastUsedTime(0) {
}

void DashSkill::activate(Player* player) {
    Uint64 now = SDL_GetTicks();
    if (now - lastUsedTime < cooldown || isDashing)
        return;

    isDashing = true;
    dashTime = 0;
    lastTime = now;
    direction = player->getDirection();
    lastUsedTime = now;
}

void DashSkill::update(Player* player, float deltaTime) {
    if (!isDashing) return;

    dashTime += deltaTime;

    float dx = static_cast<float>(direction) * dashSpeed * deltaTime;

    SDL_FRect dest = player->getDest();
    SDL_FRect nextPos = dest;
    nextPos.x += dx;

    // ѕровер€ем коллизию с новым положением
    if (!player->checkCollisionForRect(nextPos)) {
        player->setDest(nextPos); // безопасное перемещение
    }
    else {
        // ѕри столкновении прерываем рывок
        isDashing = false;
    }

    if (dashTime >= (dashDuration / 1000.0f)) {
        isDashing = false;
    }
}


