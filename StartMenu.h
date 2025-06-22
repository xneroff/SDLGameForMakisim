#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class StartMenu {
public:
    StartMenu(SDL_Renderer* renderer, TTF_Font* font, SDL_Window* window);
    ~StartMenu();
    void render();
    void handleEvent(const SDL_Event& e, bool& startGame, bool& showSettings, bool& quit);
    bool isInSettings() const { return showSettings; }
private:
    TTF_Font* font;
    SDL_Renderer* renderer;
    SDL_Window* window;
    bool showSettings = false;
    void renderButton(const SDL_FRect& rect, const std::string& text);
};

