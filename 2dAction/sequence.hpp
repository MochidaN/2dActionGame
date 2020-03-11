#pragma once
//#include <SDL.h>

//�V�[�P���X�J�ڂ̊��N���X
class Sequence {
public:
	virtual void Update(SDL_Renderer *renderer) = 0;
	virtual ~Sequence() {};
};

class Game : public Sequence {
public :
	Game();
	~Game();

	void Update(SDL_Renderer *renderer);
};