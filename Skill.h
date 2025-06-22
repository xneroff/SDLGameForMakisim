#pragma once
#include <SDL3/SDL.h>
class Player;
class Camera;

class Skill {
public:
    virtual ~Skill() = default;
    virtual void activate(Player* player) = 0;
    virtual void update(Player* player, float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer, Camera* camera) = 0;
};
