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
    std::string getName() const { return name; }
    bool isNearPlayer(const SDL_FRect& playerRect) const;
    bool showDialog = false;
    std::string dialogText = "Привет, путник!";
    std::vector<std::string> dialogPhrases;
    int currentPhrase = 0;

private:
    void initAnimations();
    std::string name;
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