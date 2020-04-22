#include "stdafx.h"

#include <iostream>
#include "font.hpp"

#include "sequence.hpp"

using namespace std;

Font::Font(SDL_Renderer *renderer, const char *text, int ptsize, const char *color) {
#ifdef _DEBUG
	const char* fontPath = "../font/ipaexg.ttf";
#endif

#ifdef NDEBUG
	const char* fontPath = "../../font/ipaexg.ttf";
#endif
	TTF_Font *font = TTF_OpenFont(fontPath, ptsize);
	if (!font) {
		cout << "TTF_OpenFont:" << TTF_GetError() << endl;
	}
	SDL_Color fontColor = ConvertToRGB(color);

	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, fontColor);
	if (surface == NULL) { cout << "failed to create font surface:" << SDL_GetError() << endl; }
	else {
		m_texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (m_texture == NULL) { cout << "failed to create texture:" << SDL_GetError() << endl; }
		SDL_FreeSurface(surface);
	}
	TTF_CloseFont(font);
}

Font::~Font() {
	SDL_DestroyTexture(m_texture);
}

SDL_Texture* Font::ReturnTexture() {
	return m_texture;
}

const void Font::QueryTexture(int &width, int &height) {
	SDL_QueryTexture(m_texture, NULL, NULL, &width, &height);
}