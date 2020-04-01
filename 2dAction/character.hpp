#pragma once

const short g_right = 1;
const short g_left = -1;

enum class CHARA_STATE {
	DIR,
	HP,
	POW,
	DEF,
	FRAME_V,
	FRAME_H,
	ACTION,
	X_ADD,
	Y_ADD
};

class Character {
private:
	short m_dir;//向き
	short m_hp;
	short m_power;//攻撃力
	short m_defense;//防御力
	//int m_trunk;//体幹
	SDL_Rect m_pos;//位置(キャラの判定ではなく画像全体) 
	short m_vertical;//現在のアニメーションコマ数(縦)
	short m_heng;//現在のアニメーションコマ数(横)
	short m_action;
	short m_xAdd;//x方向移動量
	unsigned int m_prevTime;//前回のアニメーション更新時間
public:
	Character();
	virtual ~Character();	

	//指定されたパラメータを返す
	//request：指定されたパラメータ名
	//返値：パラメータ
	virtual const short GetState(CHARA_STATE request);

	//指定されたパラメータを設定する
	//request：指定されたパラメータ名
	//state：変更後の値
	//返値：なし
	virtual void SetState(CHARA_STATE request, int state);

	//現在の座標を返す
	const SDL_Rect GetPos();

	//座標を変更する
	//nextPos：変更先の座標
	void SetPos(SDL_Rect nextPos);
	
	const unsigned int GetTime();

	void SetTime(unsigned int time);

protected:
	//パラメータの初期化
	void InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time);
};

class Player : public Character {
private:
	short m_yAdd;//y方向移動量
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	const short GetState(CHARA_STATE request) override;
	void SetState(CHARA_STATE request, int state) override;
};

class Enemy : public Character {
private:

public:
	Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time);
	//~Enemy();

	//const short GetState(CHARA_STATE request);
	//void SetState(CHARA_STATE request, int state);
};