#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

class Font {
private:
	SDL_Texture * m_texture;
public:
	Font(SDL_Renderer *renderer, const char *text, int ptsize, const char *color);
	~Font();

	//�e�N�X�`����Ԃ�
	SDL_Texture* ReturnTexture();
	//�e�N�X�`���̑傫����Ԃ�
	const void QueryTexture(int &width, int &height);
};