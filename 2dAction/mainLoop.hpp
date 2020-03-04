#pragma once
#include <vector>
#include <SDL.h>

//#include "joystick.hpp"
//#include "font.hpp"

//const int g_winPosX = 100;
//const int g_winPosY = 100;

class Sequence;
inline void OutputError(const char *msg);
SDL_Color ConvertToRGB(const char *color);
SDL_Texture *CreateText(SDL_Renderer *renderer, const char *text, int size, const char *color);

class MainLoop {
private:
	Sequence *m_sequence;//シーケンス遷移用
	SDL_Texture *m_loadingText;//ロード画面用文字
	//std::vector<std::vector<std::vector<int>>> m_maxFrame;//各アニメーションのフレーム数
	//SDL_Rect m_collisionRect[g_charaNum];//当たり判定用矩形の座標
	//std::vector<std::vector<std::vector<int>>> m_attackRect;//攻撃判定用矩形座標
	//std::vector<std::vector<std::vector<int>>> m_attackFrame;//攻撃判定のあるフレーム

public:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	//SDL_Texture*** m_characterTexture;
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
	bool Update();//ループ一回分の処理
	void MoveTo(SEQ_ID next); //遷移先を代入する
	void Loading();

private:
	SEQ_ID m_next;//次のシーケンス
};