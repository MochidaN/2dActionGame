#pragma once
#include <SDL.h>

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
	Y_ADD,
	HURT_ACTIVE,
	ATTACK_ACTIVE
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
	short m_yAdd;//y方向移動量
	unsigned int m_prevTime;//前回のアニメーション更新時間
	unsigned int m_hurtActive;//くらい判定のあるフレーム
	unsigned int m_attackActive;//攻撃判定のあるフレーム
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
	virtual void SetState(CHARA_STATE request, short state);

	//現在の座標を返す
	const SDL_Rect GetPos();

	//座標を変更する
	//nextPos：変更先の座標
	void SetPos(SDL_Rect nextPos);
	
	//前回のアニメーション更新タイミング取得
	const unsigned int GetTime();

	//最後のアニメーション更新タイミング保存
	void SetTime(unsigned int time);

	//frameコマ目のくらい判定が有効か返す
	//request：HURT_ACTIVE or ATTACK_ACTIVE
	//frame：確認したいコマ数
	//返値 0：有効でない場合 1：それ以外
	const unsigned int GetActiveBit(CHARA_STATE request, int frame);

	//引数のビット列をセットする
	//request：HURT_ACTIVE or ATTACK_ACTIVE
	//bit：セットするビット数
	void SetActiveBit(CHARA_STATE request, unsigned int bit);
protected:
	//パラメータの初期化
	void InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
};



class Player : public Character {
private:
	
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	//const short GetState(CHARA_STATE request) override;
	//void SetState(CHARA_STATE request, int state) override;
};