#include "PlayerMoneyHUD.h"

PlayerMoneyHUD::PlayerMoneyHUD(SDL_Renderer* renderer, TTF_Font* font) : renderer(renderer), font(font)
{
	dest = { 850,10,0,0 };
	obnovlenieText();
}

PlayerMoneyHUD::~PlayerMoneyHUD()
{
}

void PlayerMoneyHUD::obnovlenieHUD()
{
	obnovlenieText();
}

void PlayerMoneyHUD::otrisovka() const
{
	SDL_RenderTexture(renderer, Texture, NULL, &dest);
	SDL_DestroyTexture(Texture);
}

void PlayerMoneyHUD::changeMoney()
{
}


void PlayerMoneyHUD::obnovlenieText()
{
	Text = std::to_string(money) + "$";
	SDL_Surface* moneySurface = TTF_RenderText_Solid(font, Text.c_str(), strlen(Text.c_str()), TextColor);
	Texture = SDL_CreateTextureFromSurface(renderer, moneySurface);
	SDL_GetTextureSize(Texture, &dest.w, &dest.h);
	SDL_DestroySurface(moneySurface);
}