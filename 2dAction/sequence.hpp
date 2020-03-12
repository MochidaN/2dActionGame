#pragma once

using namespace std;

//各シーケンスの値
enum class SEQ_ID {
	//TITLE,           //タイトル
	//MODE_SELECT,     //モード選択
	GAME,            //ゲーム画面

	NONE, //無効値
	QUIT  //終了
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

//シーケンス遷移の基底クラス
class Sequence {
protected:
	SEQ_ID m_next;//次のシーケンス
public:
	Sequence();
	virtual ~Sequence() {};
	virtual bool Update(SDL_Renderer *renderer) = 0;

	//シーケンス遷移を行う
	//返値:次のシーケンス
	SEQ_ID MoveTo();
};

class Game : public Sequence {
private:
	SDL_Joystick *m_joystick;
	SDL_Texture *m_world;
	SDL_Surface *m_mapChip;
	vector<vector<int>> m_mapData;
	SDL_Texture ***m_characterTexture;

	vector<vector<ENEMY_ACTION>> m_enemyAction;//各敵が持つアクション
	vector<vector<vector<int>>> m_maxFrame;//各アニメーションのフレーム数
	vector<vector<unsigned short>> m_attackActive;//攻撃判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_attackRect;//攻撃判定範囲
	vector<vector<unsigned short>> m_hurtActive;//くらい判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_hurtRect;//くらい判定範囲
public :
	Game(SDL_Renderer *renderer, vector<vector<ENEMY_ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned short>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned short>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect);
	~Game();

	bool Update(SDL_Renderer *renderer);
	void Draw(SDL_Renderer *renderer);
};

enum class EVENT {
	//JOY_TRIANGLE,
	JOY_CIRCLE,
	JOY_CROSS,
	//JOY_SQUARE,
	JOY_R1,
	JOY_L1_UP,
	JOY_L1_DOWN,
	//JOY_HAT_UP,
	//JOY_HAT_DOWN,
	JOY_HAT_RIGHT,
	//JOY_HAT_RIGHTUP,
	//JOY_HAT_RIGHTDOWN,
	JOY_HAT_LEFT,
	//JOY_HAT_LEFTUP,
	//JOY_HAT_LEFTDOWN,
	JOY_HAT_CENTERED,
	QUIT,
	NONE
};

//イベント取得
//SDL_Joystick *joystick：ジョイスティックの構造体
//返値：取得したイベント
EVENT GetEvent(SDL_Joystick *joystick);

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

//ファイルの読み込み
//string fileName：読み込むファイル名
//返値：読み込んだデータ
vector<string> ReadFile(string fileName);

//strをmarkで分割し、数値に変換したものを返す
//string str：文字列
//const char mark：区切り文字
//返値：変換後の数字配列
vector<int> split(string str, const char mark);

//区切り文字のあるデータの読み込み
//string fileName：ファイル名
//const char mark：区切り文字
//返値：読み込んだマップデータ
vector<vector<int>>ReadFileSplit(string fileName, const char mark);