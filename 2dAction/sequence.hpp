#pragma once
//#include <SDL.h>

//シーケンス遷移の基底クラス
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