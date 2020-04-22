#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

class Font {
private:
	SDL_Texture * m_texture;
public:
	Font(SDL_Renderer *renderer, const char *text, int ptsize, const char *color);
	~Font();

	//テクスチャを返す
	SDL_Texture* ReturnTexture();
	//テクスチャの大きさを返す
	const void QueryTexture(int &width, int &height);
};