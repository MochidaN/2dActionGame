#include "stdafx.h"

#include "sequence.hpp"
#include "mainLoop.hpp"
#include "parameter.hpp"

using namespace std;

void Init();

MainLoop::MainLoop() {
	Init();

	m_window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH * MAP_CHIPSIZE, WINDOW_HEIGHT * MAP_CHIPSIZE, SDL_WINDOW_OPENGL);
	if (m_window == NULL) {
		OutputError("failed to create window");
	}
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
	if (m_renderer == NULL) {
		OutputError("failed to create renderer");
	}

	//GUARD,WARP,KICK
	vector<vector<ENEMY_ACTION>> action{ {ENEMY_ACTION::WALK, ENEMY_ACTION::HIT, ENEMY_ACTION::DOWN, ENEMY_ACTION::DEAD, ENEMY_ACTION::GUARD, ENEMY_ACTION::PUNCH}, {ENEMY_ACTION::STAND, ENEMY_ACTION::HIT, ENEMY_ACTION::DOWN, ENEMY_ACTION::DEAD, ENEMY_ACTION::DIVE, ENEMY_ACTION::JUMP_OUT}, { ENEMY_ACTION::STAND, ENEMY_ACTION::HIT, ENEMY_ACTION::DOWN, ENEMY_ACTION::DEAD, ENEMY_ACTION::RAMMING, ENEMY_ACTION::KICK_FRONT, ENEMY_ACTION::KICK_BACK} };
	const short actionSize = action.size();
	m_enemyAction.resize(actionSize);
	for (int i = 0; i < actionSize; i++) {
		m_enemyAction[i] = action[i];
	}

#ifdef _DEBUG
	vector<string> filePath{ "../txt/guard/", "../txt/warp/", "../txt/kick/", "../txt/boss/", "../txt/player/" };
	string enemyHurtRect("../txt/enemy_hurtRect/");
	string enemyAttackRect("../txt/enemy_attackRect/");
	//string imgPathFile("imgPath_debug.txt");
	string hurtRectFile("hurtRectPath_debug.txt");
	string attackRectFile("attackRectPath_debug.txt");
#endif // _DEBUG
#ifdef NDEBUG
	vector<string> filePath{ "../../txt/guard/", "../../txt/warp/", "../../txt/kick/", "../../txt/boss/", "../../txt/player/" };
	string enemyHurtRect("../../txt/enemy_hurtRect/");
	string enemyAttackRect("../../txt/enemy_attackRect/");
	//string imgPathFile("imgPath.txt");
	string hurtRectFile("hurtRectPath.txt");
	string attackRectFile("attackRectPath.txt");
#endif // NDEBUG

	string animFrameFile("animationFrame.txt");
	string attackActiveFile("attackActive.txt");
	string hurtActiveFile("hurtActive.txt");

	for (auto fp : filePath) {
		m_maxFrame.push_back(ReadFileSplit(fp + animFrameFile, ' '));
	}

	auto paramToBit = [](vector<vector<int>> num) {
		vector<unsigned int> flag;
		for (auto i : num) {
			if (i[0] < 0) {
				flag.push_back(0);
				break;
			}
			else {
				unsigned short bit = 0;
				for (auto j : i) {
					bit |= (1 << j);
				}
				flag.push_back(bit);
			}
		}
		return flag;
	};
	m_attackActive.push_back(paramToBit(ReadFileSplit(enemyAttackRect + attackActiveFile, ' ')));
	m_attackActive.push_back(paramToBit(ReadFileSplit(filePath[static_cast<int>(CHARA_ID::PLAYER)] + attackActiveFile, ' ')));
	m_hurtActive.push_back(paramToBit(ReadFileSplit(enemyHurtRect + hurtActiveFile, ' ')));
	m_hurtActive.push_back(paramToBit(ReadFileSplit(filePath[static_cast<int>(CHARA_ID::PLAYER)] + hurtActiveFile, ' ')));

	auto setRect = [](vector<string> rectPath) {
		vector<vector<vector<int>>> rect;
		const short loopNum = rectPath.size();
		for (int i = 0; i < loopNum; i++) {
			rect.push_back(ReadFileSplit(rectPath[i], ' '));
		}
		return rect;
	};
	vector<string> hurtPathHead{ enemyHurtRect, filePath[static_cast<int>(CHARA_ID::PLAYER)] + "hurtRect/" };
	vector<string> attackPathHead{ enemyAttackRect, filePath[static_cast<int>(CHARA_ID::PLAYER)] + "attackRect/" };
	for (int i = 0; i < hurtPathHead.size(); i++) {
		vector<string> hurtRectPath(ReadFile(hurtPathHead[i] + hurtRectFile));
		m_hurtRect.push_back(setRect(hurtRectPath));
		vector<string> attackRectPath(ReadFile(attackPathHead[i] + attackRectFile));
		m_attackRect.push_back(setRect(attackRectPath));
	}
	
	CreateText(m_renderer, m_loadingText, u8"Loading...", 100, "white");
	m_sequence = new Game(m_renderer, m_enemyAction, m_maxFrame, m_attackActive, m_attackRect, m_hurtActive, m_hurtRect);
}

MainLoop::~MainLoop() {
	delete m_sequence;
	SDL_DestroyTexture(m_loadingText);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	TTF_Quit();
	SDL_Quit();
}

bool MainLoop::Update() {
	SEQ_ID next = SEQ_ID::NONE;
	if (!m_sequence->Update(m_renderer)) {		
		next = m_sequence->MoveTo();
	}

	SDL_RenderPresent(m_renderer);//描画を反映

	//遷移判定
	switch (next) {
	case SEQ_ID::GAME:
		delete m_sequence;
		Loading();
		m_sequence = new Game(m_renderer, m_enemyAction, m_maxFrame, m_attackActive, m_attackRect, m_hurtActive, m_hurtRect);
		break;
	case SEQ_ID::QUIT: //終了
		return false;
		break;
	default:
		break;
	}
	return true;
}

void MainLoop::Loading() {
	SDL_Color c = ConvertToRGB("black");
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(m_renderer, NULL);

	int w, h;
	SDL_QueryTexture(m_loadingText, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { WINDOW_WIDTH - w, WINDOW_HEIGHT - h, w, h };
	SDL_RenderCopy(m_renderer, m_loadingText, NULL, &dst_rect);

	SDL_RenderPresent(m_renderer);
	SDL_Delay(1000);
}

void Init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
		OutputError("unable to initialize SDL");
	}

	if (TTF_Init() < 0) {
		cout << TTF_GetError() << endl;
		exit(1);
	}

	//jpgとpng画像を読み込めるようにする
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((flags & initted) != flags) {
		cout << "could not init IMG:" << IMG_GetError() << endl;
		exit(1);
	}

	SDL_JoystickEventState(SDL_ENABLE);//イベントを取得可能にする
}