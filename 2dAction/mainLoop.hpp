#pragma once

using namespace std;
//#include "joystick.hpp"
//#include "font.hpp"

class Sequence;
enum class ENEMY_ACTION;

class MainLoop {
private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	Sequence *m_sequence;//シーケンス遷移用
	SDL_Texture *m_loadingText;//ロード画面用文字
	SDL_Texture ***m_characterTexture;
	vector<vector<ENEMY_ACTION>> m_enemyAction;//各敵が持つアクション
	vector<vector<vector<int>>> m_maxFrame;//各アニメーションのフレーム数
	vector<vector<unsigned short>> m_attackActive;//攻撃判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_attackRect;//攻撃判定範囲
	vector<vector<unsigned short>> m_hurtActive;//くらい判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_hurtRect;//くらい判定範囲

public:
	//SDL_Texture* m_backgroundTexture;
	//Joystick m_joyEvent; //ジョイステックのイベント処理用

	//各シーケンスの値
	enum class SEQ_ID {
		//TITLE,           //タイトル
		//MODE_SELECT,     //モード選択
		GAME,            //ゲーム画面

		NONE, //無効値
		QUIT  //終了
	};

	MainLoop();
	~MainLoop();

	//ループ一回分の処理
	bool Update();

	//シーケンス遷移を行う
	//SEQ_ID next:次のシーケンス
	//返値:なし
	void MoveTo(SEQ_ID next);

	//ロード画面の表示
	//返値:なし
	void Loading();

private:
	SEQ_ID m_next;//次のシーケンス
};

enum class CHARA_ID {
	GUARD,
	WARP,
	KICK,
	//THROW,
	BOSS,
	PLAYER,
	NUM
};

enum class ENEMY_ACTION {
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
	//THROW,
	HARD_PUNCH,
	COMBO_PUNCH,
	NUM
};

enum class PLAYER_ACTION {
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

//エラー出力
//const char* msg:出力するエラー文
//返値:なし
inline void OutputError(const char *msg);

//色名に対応したRGB値を返す
//const char* color:描画色名
//返値:描画色のパラメータ
SDL_Color ConvertToRGB(const char *color);

//文字表示用のテキスチャを作成
//SDL_Renderer *renderer：表示先のレンダラ
//SDL_Texture *texture：作成したテクスチャ保存用
//const char *text：文字列
//int size：文字サイズ
//const char *color：文字色
//返値：なし
void CreateText(SDL_Renderer *renderer, SDL_Texture *texture, const char *text, int size, const char *color);