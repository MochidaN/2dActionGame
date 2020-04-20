#pragma once
#include "character.hpp"
#include <vector>

using namespace std;

class Enemy;
enum class EVENT;

class Player : public Character {
private:
	int m_trunk;//ガード耐久値
	unsigned int m_cannotChangeAction;//実行中キャンセルできないアクションを示すビット列

	//各アクションで実行する処理
	void Parry(Enemy &enemy);
	void SideAttack(Enemy &enemy);
	void VerticalAttack(Enemy &enemy);

	//アニメーションが1周した際の行動遷移処理
	void ChangeAction(vector<unsigned int> hurtActive, vector<unsigned int> atkActive);

	//キー入力を処理
	void HandleEvent(bool endAnimation, EVENT event, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive);
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	void Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive, vector<vector<int>> mapData, EVENT event);

	//攻撃イベントの処理
	void HandleAttack(Enemy &enemy, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect);

	const short GetState(CHARA_STATE request) override;
	void SetState(CHARA_STATE request, short state) override;
};