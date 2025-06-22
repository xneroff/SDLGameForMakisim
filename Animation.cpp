#include "Animation.h"

Animation::Animation() {}
Animation::~Animation() {}

void Animation::update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, bool loop) {
    frameCount = anim.frameCount;  // ќЅя«ј“≈Ћ№Ќќ обновл€ть frameCount дл€ корректной работы

    Uint32 now = SDL_GetTicks();

    if (lastUpdate == 0) {
        lastUpdate = now;
        return; // первый вызов Ч не двигаем кадры
    }

    Uint64 delta = now - lastUpdate;
    elapsedTime += static_cast<int>(delta);
    lastUpdate = now;

    if (elapsedTime >= anim.frameDelay) {
        elapsedTime = 0;
        currentFrame++;
        if (currentFrame >= anim.frameCount) {
            if (loop) {
                currentFrame = 0; // сброс дл€ цикла
            }
            else {
                currentFrame = anim.frameCount - 1; // остановка на последнем кадре
            }
        }
    }

    src.x = static_cast<float>(currentFrame * frameWidth);
    src.y = 0; // если у вас одна строка кадров, фиксируем Y на 0
}


void Animation::reset() {
    currentFrame = 0;
    elapsedTime = 0;
    lastUpdate = 0;
    // Ћучше сброс src делать в вызывающем коде (Player)
}

bool Animation::isFinished() const {
    return currentFrame >= frameCount - 1;
}
