#pragma once
#include "character.hpp"
#include <vector>

using namespace std;

class Enemy;
enum class EVENT;

class Player : public Character {
private:
	//�e�A�N�V�����Ŏ��s���鏈��
	void Parry();
	void SideAttack();
	void VerticalAttack();

	//�A�j���[�V������1�������ۂ̍s���J�ڏ���
	void ChangeAction();

	//�L�[���͂�����
	void HandleEvent(bool endAnimation, EVENT event, vector<int> hurtRect, vector<unsigned int> attackActive);
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	void Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<vector<int>> mapData, EVENT event);

	//�U���C�x���g�̏���
	void HandleAttack(Enemy &enemy, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect);

	//const short GetState(CHARA_STATE request) override;
	//void SetState(CHARA_STATE request, int state) override;
};