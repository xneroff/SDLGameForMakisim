#pragma once
#include <SDL3/SDL.h>
#include <map>
#include <string>
#include "Animation.h"
#include "Camera.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>

class NPC {
public:
    NPC(SDL_Renderer* renderer, float x, float y, const std::string& name, const std::vector<std::string>& phrases);
    ~NPC();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer, Camera* camera);
    SDL_FRect getRect() const;
    std::string getName() const;

    bool isNearPlayer(const SDL_FRect& playerRect) const;

    bool showDialog;
    std::string dialogText;
    std::vector<std::string> dialogPhrases;
    int currentPhrase;

private:
    void initAnimations();

    std::string name;
    SDL_Renderer* renderer;
    SDL_FRect dest;
    SDL_FRect src;
    SDL_FlipMode flip;
    TTF_Font* font;
    std::map<std::string, AnimationSet> animations;
    std::string currentAnim;
    int currentFrame;
    int frameTimer;
    Animation animationHandler;
    float walkTimer;
    float direction;
    float speed;
};
