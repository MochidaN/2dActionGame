#pragma once

//�V�[�P���X�J�ڂ̊��N���X
class Sequence {
public:
	virtual void Update() = 0;
	virtual ~Sequence() {};
};

class Game : public Sequence {
public :
	Game();
	~Game();

	void Update();
};