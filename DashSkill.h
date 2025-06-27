#pragma once
#include "Skill.h"
#include "Camera.h"

class DashSkill : public Skill {
public:
    DashSkill();

    void activate(Player* player) override;
    void update(Player* player, float deltaTime) override;
    void render(SDL_Renderer*, Camera*) override {}

private:
    bool isDashing = false;
    float dashTime = 0.0f;
    float dashSpeed = 1500.0f;
    float dashDuration = 200.0f;
    int direction = 1;

    Uint64 lastTime = 0;
    Uint64 lastUsedTime = 0;
    Uint64 cooldown = 1000;
};