#include "MainMenu.h"
#include <SDL3_ttf/SDL_ttf.h>
MainMenu::MainMenu(SDL_Renderer* renderer, TTF_Font* font, SDL_Window* window)
    : renderer(renderer), font(font), window(window)
{

}

MainMenu::~MainMenu() {}

void MainMenu::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderClear(renderer);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float centerX = w / 2.0f - 200.f / 2.0f;
    if (!showSettings) {
        renderButton({ centerX, 300.f, 200.f, 60.f }, "Continue");
        renderButton({ centerX, 380.f, 200.f, 60.f }, "Settings");
        renderButton({ centerX, 460.f, 200.f, 60.f }, "Exit");
    }
    else {
        renderButton({ centerX, 300.f, 200.f, 60.f }, "Windowed");
        renderButton({ centerX, 380.f, 200.f, 60.f }, "Fullscreen");
    }

    SDL_RenderPresent(renderer);
}

// ✅ исправлено: SDL_FRect
void MainMenu::renderButton(const SDL_FRect& rect, const std::string& text) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderFillRect(renderer, &rect); // ✅ теперь типы совпадают

    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), strlen(text.c_str()), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_FRect dst;
            dst.w = surface->w;
            dst.h = surface->h;
            dst.x = static_cast<int>(rect.x + (rect.w - dst.w) / 2);
            dst.y = static_cast<int>(rect.y + (rect.h - dst.h) / 2);
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}

void MainMenu::handleEvent(const SDL_Event& e, bool& resume, bool& exitToStartMenu) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float centerX = w / 2.0f - 200.f / 2.0f;
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float x = e.button.x;
        float y = e.button.y;
        if (!showSettings) {
            if (x >= centerX && x <= centerX + 200.f && y >= 300.f && y <= 360.f) {
                resume = true;
            }
            else if (x >= centerX && x <= centerX + 200.f && y >= 380.f && y <= 440.f) {
                showSettings = true;
            }
            else if (x >= centerX && x <= centerX + 200.f && y >= 460.f && y <= 520.f) {
                exitToStartMenu = true; // вместо quit = true
            }
        }
        else {
            if (x >= centerX && x <= centerX + 200.f && y >= 300.f && y <= 360.f) {
                SDL_SetWindowFullscreen(window, 0);
                SDL_SetWindowSize(window, 1280, 720);
                SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                SDL_SetWindowBordered(window, true);
                showSettings = false;
            }
            else if (x >= centerX && x <= centerX + 200.f && y >= 380.f && y <= 440.f) {
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                showSettings = false;
            }
        }
    }
}




