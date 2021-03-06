#include "stdafx.h"
#include "mainLoop.hpp"

int main(int argc, char* argv[]) {
	MainLoop *mainLoop = new MainLoop();

	while (mainLoop->Update()) {}
	delete mainLoop;

	return 0;
}


/*
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_surface.h>

using namespace std;

void Init() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "unable to initialize SDL:" << SDL_GetError() << endl;
	}
	//jpgとpng画像を読み込めるようにする
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((flags & initted) != flags) {
		cout << "could not init IMG:" << IMG_GetError << endl;
	}

	//if (!InitAudio()) { exit(1); }
}

int main(int argc, char* argv[]) {
	Init();
	SDL_Window* window = SDL_CreateWindow("window", 100, 100, 256, 256, SDL_WINDOW_OPENGL);;
	if (window == NULL) {
		cout << "could not create window:" << SDL_GetError() << endl;
		//return;
	}
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer == NULL) {
		cout << "could not create renderer:" << SDL_GetError() << endl;
	}

	SDL_Surface* img = IMG_Load("dead.png");
	SDL_Surface* world = SDL_CreateRGBSurface(0, 512, 512, 32, 0, 0, 0, 0);
	if (img == NULL || world == NULL) {
		cout << SDL_GetError() << endl;
		cout << "failed to open background image:" <<img << endl;
	}

	const int Size = 256;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			SDL_Rect src = {}, dst = {Size*j, Size*i, Size, Size};

			SDL_BlitSurface(img, NULL, world, &dst);
		}
	}

	if ((texture = SDL_CreateTextureFromSurface(renderer, world)) == NULL) {
		cout << "failed to create texture:" << SDL_GetError() << endl;
	}
	SDL_FreeSurface(img);

	SDL_Rect src = {256,256,256,256};
	//SDL_Rect dst = {100,100,256,256};

	//SDL_RenderCopyEx(renderer, texture, &src, &dst, 180, NULL, SDL_FLIP_VERTICAL);//左右反転（左向き）
	//描画を反映
	//SDL_RenderPresent(renderer);
	//SDL_Delay(1000);

	SDL_RenderCopy(renderer, texture, &src, NULL);
	//描画を反映
	SDL_RenderPresent(renderer);
	SDL_Delay(3000);

    return 0;
}
*/
