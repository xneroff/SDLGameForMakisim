#include "Interface.h"

Interface::Interface(SDL_Renderer* renderer, TTF_Font* font, int currentHealth, int TotalHealth)
    : renderer(renderer), font(font)
{
    playerMoneyHUD = new PlayerMoneyHUD(renderer, font);
    playerHealthHUD = new PlayerHealthHUD(renderer, font, currentHealth, TotalHealth);
}

Interface::~Interface() {
    TTF_CloseFont(font);
    delete playerMoneyHUD;
    delete playerHealthHUD;
}

void Interface::setHealth(int hp) {
    playerHealthHUD->setHealth(hp);
}

void Interface::setMoney(int money) {
    playerMoneyHUD->setMoney(money);
}

void Interface::obnovlenieHUD(float deltaTime) {
    playerMoneyHUD->obnovlenieHUD();
    playerHealthHUD->obnovlenieHUD(deltaTime);
}


void Interface::otrisovka() const {
    playerMoneyHUD->otrisovka();
    playerHealthHUD->otrisovka();
}