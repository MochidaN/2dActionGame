#pragma once
#include "character.hpp"
#include <vector>

using namespace std;

namespace ENEMY {
	enum class ACTION {
		STAND,
		WALK,
		HIT,
		DOWN,
		DEAD,
		GUARD,
		PUNCH,
		DIVE,
		JUMP_OUT,
		RAMMING,
		KICK_FRONT,
		KICK_BACK,
		HARD_PUNCH,
		COMBO_PUNCH,
		NUM
	};
};

class Enemy : public Character {
private:
	//bool m_inWindow;
public:
	Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Enemy();

	//const short GetState(CHARA_STATE request) override;
	//void SetState(CHARA_STATE request, int state) override;

	//このキャラクターがウィンドウ内にいるか返す
	//const bool GetInWindow();

	//ウィンドウ内にいるかの状態更新
	//void SetInWindow(bool flag);

	void Stand();
	void Walk();
	void Hit();
	void Down();
	void Dead();
};

//次のアニメーションフレームを計算
//hengFrame：現在の横コマ数
//verticalFrame：現在の縦コマ数
//maxFrame：最大コマ数(0:横 1:縦)
//endAnimation：アニメーションが一周したかどうか
//返値：次のフレーム数
vector<short> ReturnNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation);