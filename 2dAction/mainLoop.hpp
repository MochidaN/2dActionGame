#pragma once

using namespace std;
//#include "font.hpp"

class Sequence;
namespace ENEMY {
	enum class ACTION;
}

class MainLoop {
private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	Sequence *m_sequence;//シーケンス遷移用
	SDL_Texture *m_loadingText;//ロード画面用文字
	vector<vector<ENEMY::ACTION>> m_enemyAction;//各敵が持つアクション
	vector<vector<vector<int>>> m_maxFrame;//各アニメーションのフレーム数
	vector<vector<unsigned int>> m_attackActive;//攻撃判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_attackRect;//攻撃判定範囲
	vector<vector<unsigned int>> m_hurtActive;//くらい判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_hurtRect;//くらい判定範囲

public:
	MainLoop();
	~MainLoop();

	//ループ一回分の処理
	bool Update();

	//ロード画面の表示
	//返値:なし
	void Loading();
};