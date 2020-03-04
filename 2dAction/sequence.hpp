#pragma once

//シーケンス遷移の基底クラス
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