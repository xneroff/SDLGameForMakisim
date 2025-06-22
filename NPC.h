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
    NPC(SDL_Renderer* renderer, float x, float y);
    ~NPC();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer, Camera* camera);
    SDL_FRect getRect() const;

    bool isNearPlayer(const SDL_FRect& playerRect) const;
    bool showDialog = false;
    std::string dialogText = "Привет, путник!";
    std::vector<std::string> dialogPhrases = {
    "Privet, putnik!",
    "be safe in thats places.",
    "Talk, in forest can live monsters...",
    "Have a good luck!"
    };
    int currentPhrase = 0;

private:
    void initAnimations();



    SDL_Renderer* renderer;
    SDL_FRect dest;
    SDL_FRect src;
    SDL_FlipMode flip;
    TTF_Font* font = nullptr;

    std::map<std::string, AnimationSet> animations;
    std::string currentAnim = "idle";
    int currentFrame = 0;
    int frameTimer = 0;
    Animation animationHandler;

    float walkTimer = 0;
    float direction = 1;
    float speed = 1.0f;
};