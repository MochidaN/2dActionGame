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

	//���̃L�����N�^�[���E�B���h�E���ɂ��邩�Ԃ�
	//const bool GetInWindow();

	//�E�B���h�E���ɂ��邩�̏�ԍX�V
	//void SetInWindow(bool flag);

	void Stand();
	void Walk();
	void Hit();
	void Down();
	void Dead();
};

//���̃A�j���[�V�����t���[�����v�Z
//hengFrame�F���݂̉��R�}��
//verticalFrame�F���݂̏c�R�}��
//maxFrame�F�ő�R�}��(0:�� 1:�c)
//endAnimation�F�A�j���[�V����������������ǂ���
//�Ԓl�F���̃t���[����
vector<short> ReturnNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation);