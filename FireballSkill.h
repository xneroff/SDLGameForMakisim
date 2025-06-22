#pragma once
#include <SDL3/SDL.h>
#include "Player.h"
#include "Skill.h"
#include "Camera.h"

class FireballSkill : public Skill {
public:
    FireballSkill();

    void activate(Player* player) override;
    void update(Player* player, float deltaTime) override;
    void render(SDL_Renderer* renderer, Camera* camera) override;

    SDL_FRect getRect() const { return fireballRect; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }

private:
    bool active;
    SDL_FRect fireballRect;
    float fireballSpeed;
    float fireballDirection;

    static constexpr float WIDTH = 20.0f;
    static constexpr float HEIGHT = 20.0f;

    float traveledDistance;
    float maxDistance;
};
