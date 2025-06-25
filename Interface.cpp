#include "Interface.h"

Interface::Interface(SDL_Renderer* renderer, TTF_Font* font, int currentHealth, int TotalHealth)
    : renderer(renderer), font(font)
{
    playerHealthHUD = new PlayerHealthHUD(renderer, font, currentHealth, TotalHealth);
}
Interface::~Interface() {
    TTF_CloseFont(font);
    delete playerHealthHUD;
}
void Interface::setHealth(int hp) {
    playerHealthHUD->setHealth(hp);
}
void Interface::obnovlenieHUD(float deltaTime) {
    playerHealthHUD->obnovlenieHUD(deltaTime);
}
void Interface::otrisovka() const {
    playerHealthHUD->otrisovka();
}