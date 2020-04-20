#pragma once
#include "character.hpp"
#include <vector>

using namespace std;

class Enemy;
enum class EVENT;

class Player : public Character {
private:
	int m_trunk;//�K�[�h�ϋv�l
	unsigned int m_cannotChangeAction;//���s���L�����Z���ł��Ȃ��A�N�V�����������r�b�g��

	//�e�A�N�V�����Ŏ��s���鏈��
	void Parry(Enemy &enemy);
	void SideAttack(Enemy &enemy);
	void VerticalAttack(Enemy &enemy);

	//�A�j���[�V������1�������ۂ̍s���J�ڏ���
	void ChangeAction(vector<unsigned int> hurtActive, vector<unsigned int> atkActive);

	//�L�[���͂�����
	void HandleEvent(bool endAnimation, EVENT event, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive);
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	void Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive, vector<vector<int>> mapData, EVENT event);

	//�U���C�x���g�̏���
	void HandleAttack(Enemy &enemy, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect);

	const short GetState(CHARA_STATE request) override;
	void SetState(CHARA_STATE request, short state) override;
};