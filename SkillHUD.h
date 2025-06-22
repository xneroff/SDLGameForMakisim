// SkillHUD.h
#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>

struct SkillIcon {
    SDL_Texture* icon;
    SDL_FRect rect;
    std::string name;
};

class SkillHUD {
public:
    SkillHUD(SDL_Renderer* renderer);
    ~SkillHUD();
    void addSkillIcon(SDL_Texture* tex, const std::string& name);
    void render() const;

private:
    SDL_Renderer* renderer;
    std::vector<SkillIcon> icons;
};
