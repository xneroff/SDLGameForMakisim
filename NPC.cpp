#include "NPC.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include "Game.h"
#include <iostream>



NPC::NPC(SDL_Renderer * renderer, float x, float y) : renderer(renderer) {
    dest = { x, y, 64, 64 };
    src = { 0, 0, 48, 48 };
    initAnimations();
}

NPC::~NPC() {}

void NPC::initAnimations() {
    SDL_Texture* idleTex = IMG_LoadTexture(renderer, "assets/NPC/2 GraveRobber/GraveRobber_idle.png");
    if (idleTex) {
        SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);
        animations["idle"] = { idleTex, 4, 250 };
    }

    SDL_Texture* walkTex = IMG_LoadTexture(renderer, "assets/NPC/2 GraveRobber/GraveRobber_walk.png");
    if (walkTex) {
        SDL_SetTextureScaleMode(walkTex, SDL_SCALEMODE_NEAREST);
        animations["walk"] = { walkTex, 6, 180 };
    }
}

bool NPC::isNearPlayer(const SDL_FRect& playerRect) const {
    SDL_FRect area = dest;
    area.x -= 50;
    area.w += 100;
    return SDL_HasRectIntersectionFloat(&area, &playerRect);
}


void NPC::update(float deltaTime) {
    if (showDialog) {
        currentAnim = "idle";
        currentFrame = 0;
        src.x = 0;
        return;  // ⛔️ не двигаемся, не обновляем анимацию
    }

    walkTimer += deltaTime;

    // Меняет направление каждые 3 секунды
    if (walkTimer > 3.0f) {
        walkTimer = 0;
        direction *= -1;
    }

    dest.x += direction * speed;
    flip = direction < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    currentAnim = "walk";

    animationHandler.update(animations[currentAnim], src, (int)src.w);
}


void NPC::render(SDL_Renderer* renderer, Camera* camera) {
    SDL_FRect screenDest = camera->apply(dest);
    SDL_RenderTextureRotated(renderer, animations[currentAnim].texture, &src, &screenDest, 0, nullptr, flip);
}



SDL_FRect NPC::getRect() const {
    return dest;
}