#pragma once
#include <SDL.h>
#include <vector>

using namespace std;

enum class EVENT;
namespace ENEMY {
	enum class ACTION;
}

//各シーケンスの値
enum class SEQ_ID {
	MENU,          //モード選択
	CONFIG,        //設定
	GAME,          //ゲーム画面
	GAME_CLEAR,    //ゲーム終了(遷移先選択画面)
	GAME_OVER,     //ゲーム終了(遷移先選択画面)
	NONE,          //無効値
	QUIT           //終了
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


//イベント取得
//SDL_Joystick *joystick：ジョイスティックの構造体
//返値：取得したイベント
EVENT GetEvent(SDL_Joystick *joystick);

//エラー出力
//const char* msg:出力するエラー文
//返値:なし
inline const void OutputError(const char *msg);

//色名に対応したRGB値を返す
//const char* color:描画色名
//返値:描画色のパラメータ
SDL_Color ConvertToRGB(const char *color);