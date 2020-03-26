#pragma once

enum class CHARA_STATE {
	DIR,
	HP,
	POW,
	DEF,
	AGI,
	FRAME_V,
	FRAME_H,
	ACTION,
	TIME
};

class Character {
private:
	bool m_dir;//向き　true:右 false:左
	short m_hp;
	short m_power;//攻撃力
	short m_defense;//防御力
	short m_agility;//速さ
	//int m_trunk;//体幹
	SDL_Rect m_pos;//位置
	short m_vertical;//現在のアニメーションコマ数(縦)
	short m_heng;//現在のアニメーションコマ数(横)
	short m_action;
	unsigned int m_prevTime;//前回のアニメーション更新時間
public:
	Character();
	virtual ~Character();	

	//指定されたパラメータを返す
	//request：指定されたパラメータ名
	//返値：パラメータ
	const short GetState(CHARA_STATE request);

	//指定されたパラメータを設定する
	//request：指定されたパラメータ名
	//state：変更後の値
	//返値：なし
	void SetState(CHARA_STATE request, int state);

	//座標を変更する
	//nextPos：変更先の座標
	void SetPos(SDL_Rect nextPos);

	//現在の座標を返す
	SDL_Rect GetPos();

protected:
	//パラメータの初期化
	void InitState(bool dir, short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time);
};

class Player : public Character {
private:
	//short m_jumpMovement;//ジャンプ時のx方向移動量
	//short m_yPrev;//前のy座標
public:
	Player(short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time);
	//~Player();

	const short GetState(CHARA_STATE request);
};

class Enemy : public Character {
private:

public:
	Enemy(short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time);
	//~Enemy();

	const short GetState(CHARA_STATE request);
};