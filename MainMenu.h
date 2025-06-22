#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class MainMenu {
public:
    MainMenu(SDL_Renderer* renderer, TTF_Font* font, SDL_Window* window);
    ~MainMenu();
    bool isInSettings() const { return showSettings; }
    void render();
    void handleEvent(const SDL_Event& e, bool& resume, bool& quit);
private:
    /*SDL_Color* colorText = { 255, 255, 255, 255 };*/

    TTF_Font* font;
    SDL_Renderer* renderer;
    bool showSettings = false;
    SDL_Window* window;
    void renderButton(const SDL_FRect& rect, const std::string& text);
};