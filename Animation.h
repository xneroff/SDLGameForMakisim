#pragma once
#include <SDL3/SDL.h>
#include "Structs.h"

class Animation {
public:
    Animation();
    ~Animation();

    void update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, bool loop = true);

    void reset();
    bool isFinished() const;

    int getCurrentFrame() const {
        return currentFrame;
    }

private:
    int currentFrame = 0;
    int elapsedTime = 0;
    int frameCount = 1;          // ���������� ������ � ������� ��������
    Uint64 lastUpdate = 0;
    Uint32 lastFrameTime = 0;
    Uint32 frameDuration = 100; // ������������ ������ ����� � ��
    int totalFrames = 0;
};
