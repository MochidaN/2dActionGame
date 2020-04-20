#pragma once
#include <SDL.h>
#include <vector>

using namespace std;

const short g_right = 1;
const short g_left = -1;
const short g_addX = 4;

enum class CHARA_ID {
	GUARD,
	WARP,
	KICK,
	BOSS,
	PLAYER,
	NUM
};

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
	ATTACK_ACTIVE,
	TRUNK
};

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

namespace PLAYER {
	enum class ACTION {
		STAND,
		WALK,
		STEP,
		HIT,
		HIT_AIR,
		JUMP,
		FALL,
		FALL_HIT,
		LANDING,
		FAINT,
		FAINTING,
		DOWN,
		DEAD,
		GUARD,
		PARRY,
		SIDE_ATTACK,
		VERTICAL_ATTACK,
		NUM
	};
}

class Character {
protected:
	//パラメータの初期化
	void InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
private:
	short m_dir;//向き
	short m_hp;
	short m_power;//攻撃力
	short m_defense;//防御力
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

	virtual void Update();

	//アニメーションフレーム更新
	bool UpdateAnimation(unsigned int nowTime, std::vector<int> maxFrame);

	//座標移動処理
	void MovePos(vector<int> myHurtRect, vector<vector<int>> mapData);

	//キャラ同士が衝突した際の処理
	void CollisionChara(vector<int> myHurtRect, vector<int> oppHurtRect, vector<vector<int>> mapData, Character &opponent);

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
};


//2点間の距離の2乗を返す
float CalculateSquaredDistance(SDL_Rect r0, SDL_Rect r1);

//次のアニメーションフレームを計算
//hengFrame：現在の横コマ数
//verticalFrame：現在の縦コマ数
//maxFrame：最大コマ数(0:横 1:縦)
//endAnimation：アニメーションが一周したかどうか
//返値：次のフレーム数
std::vector<short> ReturnNextFrame(short hengFrame, short verticalFrame, std::vector<int> maxFrame, bool &endAnimation);

//今のコマが通しで何コマ目か返す
//maxFrameHeng：横のコマが最大いくつか
//chara：計算対象のキャラクタ構造値
//返値：現在のコマ数
int ReturnFrameNum(int maxFrameHeng, Character chara);

//キャラ当たり判定の座標を反転
//rect：右向きの座標
//imgWidth：画像の幅
inline vector<int> FlipRect(vector<int> rect, int imgWidth);

//x座標の移動
//nowPos：現在の座標(画像全体)
//hurtRext：キャラの当たり判定
//distance：移動距離
//worldWidth：ワールドの端座標
//mapData：マップ番号データ
//返値：移動後の座標
SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData);

//y座標の移動
//nowPos：現在の座標(画像全体)
//hurtRext：キャラの当たり判定
//yAdd：y方向の移動量
//worldHeight：ワールドの端座標
//mapData：マップ番号データ
//返値：移動後の座標
SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData);

//キャラ当たり判定の座標を反転
//rect：右向きの座標
//imgWidth：画像の幅
//vector<int> FlipRect(vector<int> rect, int imgWidth);

//移動時のマップとの衝突判定
//nowPos：現在の座標(画像全体)
//prevX：移動前のx座標
//hurtRext：キャラの当たり判定
//mapData：マップ番号データ
void CollisionMapX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData);

//移動時のマップとの衝突判定
//nowPos：現在の座標(画像全体)
//prevY：移動前のy座標
//hurtRext：キャラの当たり判定
//mapData：マップ番号データ
//返値：地上にいるかどうか
bool CollisionMapY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData);

//キャラクタの判定範囲を計算
//pos：キャラ画像の左上座標
//rect：キャラ矩形の相対座標
//返値：計算後の座標
SDL_Rect ReturnCharaRect(SDL_Rect pos, vector<int> rect);

//矩形の当たり判定
//引数:判定する2矩形
//返値 true:衝突している false:していない
bool DetectCollisionRect(SDL_Rect r0, SDL_Rect r1);

//アクションとActiveBitの変更をする
void SetAction(Character &chara, short nextAction, unsigned int hurtActive, unsigned int atkActive);