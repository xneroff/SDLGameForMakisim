#include "Animation.h"

Animation::Animation() {}
Animation::~Animation() {}

void Animation::update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, bool loop) {
    frameCount = anim.frameCount;  // ����������� ��������� frameCount ��� ���������� ������

    Uint32 now = SDL_GetTicks();

    if (lastUpdate == 0) {
        lastUpdate = now;
        return; // ������ ����� � �� ������� �����
    }

    Uint64 delta = now - lastUpdate;
    elapsedTime += static_cast<int>(delta);
    lastUpdate = now;

    if (elapsedTime >= anim.frameDelay) {
        elapsedTime = 0;
        currentFrame++;
        if (currentFrame >= anim.frameCount) {
            if (loop) {
                currentFrame = 0; // ����� ��� �����
            }
            else {
                currentFrame = anim.frameCount - 1; // ��������� �� ��������� �����
            }
        }
    }

    src.x = static_cast<float>(currentFrame * frameWidth);
    src.y = 0; // ���� � ��� ���� ������ ������, ��������� Y �� 0
}


void Animation::reset() {
    currentFrame = 0;
    elapsedTime = 0;
    lastUpdate = 0;
    // ����� ����� src ������ � ���������� ���� (Player)
}

bool Animation::isFinished() const {
    return currentFrame >= frameCount - 1;
}
