#include "stdafx.h"
#include <algorithm>

#include "gameClass.hpp"

const short ANIME_INTERVAL = 1000 / 10;//�A�j���[�V�����Ԋu

const short IMG_SIZE[static_cast<short>(CHARA_ID::NUM)] = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, PLAYER_SIZE };
const short CHARA_NUM[static_cast<short>(CHARA_ID::NUM)] = { 0, 0, 0, 1, 1 };
const short X = 0, Y = 1, W = 2, H = 3;
const short g_playerMoveX = 4;
const short g_playerStepX = 20;
const short g_yAdd = -28;
const short g_yAdd_ground = -1;

Sequence::Sequence() {
	m_next = SEQ_ID::NONE;
}

SEQ_ID Sequence::MoveTo() {
	return m_next;
}

Game::Game(SDL_Renderer *renderer, vector<vector<ENEMY_ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned int>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned int>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect) {
	m_joystick = SDL_JoystickOpen(0);//�W���C�X�e�B�b�N���\���̂Ɋ��蓖�ĂėL����
	if (!SDL_JoystickGetAttached(m_joystick)) {
		cout << "failed to open joystick" << endl;
	}

#ifdef NDEBUG
	const char *mapImgPath = "../../image/map/mapchip.png";
	const char *mapDataPath = "../../image/map/map.csv";
	vector<string> filePath{ "../../txt/guard/", "../../txt/warp/", "../../txt/kick/", "../../txt/boss/", "../../txt/player/" };
	string imgPathFile("imgPath.txt");
#else
	const char *mapImgPath = "../image/map/mapchip.png";
	const char *mapDataPath = "../image/map/map.csv";
	vector<string> filePath{ "../txt/guard/", "../txt/warp/", "../txt/kick/", "../txt/boss/", "../txt/player/" };
	string imgPathFile("imgPath_debug.txt");
#endif 

	SDL_Surface *worldSurface = SDL_CreateRGBSurface(0, WORLD_WIDTH * MAP_CHIPSIZE, WORLD_HEIGHT * MAP_CHIPSIZE, 32, 0, 0, 0, 0);
	SDL_Surface *mapChip = IMG_Load(mapImgPath);
	if (worldSurface == NULL || mapChip == NULL) {
		OutputError("failed to create world surface");
	}
	m_mapData = ReadFileSplit(mapDataPath, ',');
	DrawWorld(renderer, worldSurface, mapChip, m_mapData);
	if ((m_world = SDL_CreateTextureFromSurface(renderer, worldSurface)) == NULL) {
		OutputError("failed to create world texture");
	}
	SDL_FreeSurface(mapChip);

	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	m_characterTexture = new SDL_Texture**[charaNum];
	for (int i = 0; i < charaNum; i++) {
		vector<string> imagePath = ReadFile(filePath[i] + imgPathFile);
		m_characterTexture[i] = new SDL_Texture*[imagePath.size()];
		for (int j = 0, end = imagePath.size(); j < end; j++) {
			SDL_Surface *img;
			if ((img = IMG_Load(imagePath[j].c_str())) == NULL) {
				OutputError("failed to open character image");
			}
			if ((m_characterTexture[i][j] = SDL_CreateTextureFromSurface(renderer, img)) == NULL) {
				OutputError("failed to create character texture");
			}
			SDL_FreeSurface(img);
		}
	}

	m_enemyAction = enemyAction;
	m_maxFrame = maxFrame;
	m_attackActive = attackActive;
	m_attackRect = attackRect;
	m_hurtActive = hurtActive;
	m_hurtRect = hurtRect;

	const short enemyNum = static_cast<short>(CHARA_ID::NUM) - 1;
	const short action[static_cast<short>(CHARA_ID::NUM)-1] = { static_cast<short>(ENEMY_ACTION::GUARD), static_cast<short>(ENEMY_ACTION::STAND),  static_cast<short>(ENEMY_ACTION::STAND),  static_cast<short>(ENEMY_ACTION::STAND)};
	unsigned int time = SDL_GetTicks();
	for (int id = 0; id < enemyNum; id++) {
		m_enemy[id] = new Enemy*[CHARA_NUM[id]];
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			SDL_Rect pos = { 400, 0, IMG_SIZE[id], IMG_SIZE[id]};
			m_enemy[id][cn] = new Enemy(100, 10, 10, pos, action[id], time, g_yAdd_ground);
		}
	}
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	const short stand = static_cast<short>(PLAYER_ACTION::STAND);
	SDL_Rect pos = { 0, WINDOW_HEIGHT * MAP_CHIPSIZE - (MAP_CHIPSIZE + m_hurtRect[1][stand][0][Y] + m_hurtRect[1][stand][0][H]), IMG_SIZE[playerID], IMG_SIZE[playerID] };
	m_player = new Player(100, 10, 10, pos, static_cast<short>(PLAYER_ACTION::STAND), time, g_yAdd_ground);
}

Game::~Game() {
	if (SDL_JoystickGetAttached(m_joystick)) {
		SDL_JoystickClose(m_joystick);
	}

	SDL_DestroyTexture(m_world);
	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	vector<short> actionNum;
	for (int id = 0, end = m_enemyAction.size(); id < end; id++) {
		actionNum.push_back(m_enemyAction[id].size());
	}
	actionNum.push_back(static_cast<short>(ENEMY_ACTION::NUM));
	actionNum.push_back(static_cast<short>(PLAYER_ACTION::NUM));

	for (int id = 0; id < charaNum - 1; id++) {
		for (int cn = 0; cn < actionNum[id]; cn++) {
			SDL_DestroyTexture(m_characterTexture[id][cn]);
		}
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			delete m_enemy[id][cn];
		}
		delete[] m_characterTexture[id];
		delete[] m_enemy[id];
	}
	delete m_player;
	delete m_characterTexture[static_cast<int>(CHARA_ID::PLAYER)];
	delete[] m_characterTexture;
}

bool Game::Update(SDL_Renderer *renderer) {
	Draw(renderer);

	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	const short act = m_player->GetState(CHARA_STATE::ACTION);
	const short pNowFrame = ReturnFrameNum(m_maxFrame[playerID][act][0], *m_player);
	SDL_Rect playerPos = ReturnCharaRect(m_player->GetPos(), m_hurtRect[1][act][pNowFrame], m_player->GetState(CHARA_STATE::DIR));
	SDL_Rect pAtkRect = { -1, -1, -1, -1 };
	if (m_player->GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, pNowFrame) != 0) {
		pAtkRect = ReturnCharaRect(m_player->GetPos(), m_attackRect[1][act][pNowFrame], m_player->GetState(CHARA_STATE::DIR));
	}

	const int winWidth = WINDOW_WIDTH * MAP_CHIPSIZE;
	int windowPosX = (m_player->GetPos().x + m_hurtRect[1][static_cast<short>(PLAYER_ACTION::STAND)][0][X]) - (WINDOW_WIDTH * MAP_CHIPSIZE / 2);//�`��̈�̍��[
	if (windowPosX < 0) { windowPosX = 0; }
	else if (windowPosX + winWidth > WORLD_WIDTH * MAP_CHIPSIZE) { windowPosX = (WORLD_WIDTH - WINDOW_WIDTH) * MAP_CHIPSIZE; }
	const short charaNum = static_cast<short>(CHARA_ID::NUM) - 1;

	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			const short action = m_enemy[id][cn]->GetState(CHARA_STATE::ACTION);
			const short eNowFrame = ReturnFrameNum(m_maxFrame[id][action][0], *m_enemy[id][cn]);
			SDL_Rect enemyPos = ReturnCharaRect(m_enemy[id][cn]->GetPos(), m_hurtRect[0][action][eNowFrame], m_enemy[id][cn]->GetState(CHARA_STATE::DIR));
			if ((windowPosX <= enemyPos.x + enemyPos.w) && (windowPosX + winWidth >= enemyPos.x)) {//�E�B���h�E���ɂ���
				if (DetectCollisionRect(playerPos, enemyPos) == true) {//�v���C���ƓG���Ԃ����Ă���
					SDL_Rect p = m_player->GetPos();
					SDL_Rect e = m_enemy[id][cn]->GetPos();
					const short dst = g_playerMoveX;
					int pDst = (playerPos.x <= enemyPos.x) ? -dst : dst;
					int eDst = (playerPos.x <= enemyPos.x) ? dst : -dst;
					m_player->SetPos(MovePositionX(p, m_hurtRect[1][act][pNowFrame], pDst, WORLD_WIDTH, m_mapData));
					m_enemy[id][cn]->SetPos(MovePositionX(e, m_hurtRect[0][action][eNowFrame], eDst, WORLD_WIDTH, m_mapData));
				}

				if (pAtkRect.x != -1) {
					if (DetectCollisionRect(pAtkRect, enemyPos) == true) {
						m_enemy[id][cn]->SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY_ACTION::HIT));
						m_player->SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, 0);//�U��������Ȃ���
					}
				}
				/*
				if (m_enemy[id][cn]->GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, eNowFrame) != 0) {//�G�̍s���ɍU�����肪����
					SDL_Rect eAtkRect = ReturnCharaRect(m_enemy[id][cn]->GetPos(), m_attackRect[0][action][eNowFrame], m_enemy[id][cn]->GetState(CHARA_STATE::DIR));
					if (DetectCollisionRect(playerPos, eAtkRect) == true) {

					}
				}*/
			}
		}
	}

	EVENT event = GetEvent(m_joystick);
	switch (event) {
	case EVENT::QUIT: {
		m_next = SEQ_ID::QUIT;
		return false;
		break;
	}
	case EVENT::JOY_CIRCLE: {
		if (m_player->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::STEP));
			m_player->SetState(CHARA_STATE::X_ADD, g_playerStepX);
		}
		break;
	}
	case EVENT::JOY_CROSS: {
		if (m_player->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::JUMP));
			m_player->SetState(CHARA_STATE::Y_ADD, g_yAdd);
		}
		break;
	}
	case EVENT::JOY_L1_DOWN: {
		if (m_player->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::GUARD));
			m_player->SetState(CHARA_STATE::X_ADD, 0);
		}
		break;
	}
	case EVENT::JOY_L1_UP: {
		if (m_player->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::STAND));
		}
		break;
	}
	case EVENT::JOY_R1: {
		if (m_player->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			if (m_player->GetState(CHARA_STATE::ACTION) == static_cast<short>(PLAYER_ACTION::VERTICAL_ATTACK)) {
				m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::SIDE_ATTACK));
				m_player->SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, m_attackActive[1][static_cast<short>(PLAYER_ACTION::SIDE_ATTACK)]);
				m_player->SetState(CHARA_STATE::X_ADD, 0);
			}
			else if (m_player->GetState(CHARA_STATE::ACTION) != static_cast<short>(PLAYER_ACTION::SIDE_ATTACK)) {
				m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::VERTICAL_ATTACK));
				m_player->SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, m_attackActive[1][static_cast<int>(PLAYER_ACTION::VERTICAL_ATTACK)]);
				m_player->SetState(CHARA_STATE::X_ADD, 0);
			}
		}
		break;
	}
	case EVENT::JOY_HAT_LEFT: {
		m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::WALK));
		m_player->SetState(CHARA_STATE::X_ADD, g_playerMoveX);
		
		if (m_player->GetState(CHARA_STATE::DIR) == g_right) {//�E�������獶�����ɂȂ�Ƃ�
			SDL_Rect pos = m_player->GetPos();
			const short action = m_player->GetState(CHARA_STATE::ACTION);
			vector<int> fRect = FlipRect(m_hurtRect[1][action][0], IMG_SIZE[playerID]);
			pos.x -= (fRect[X] - m_hurtRect[1][action][0][X]);
			m_player->SetPos(pos);
			m_player->SetState(CHARA_STATE::DIR, g_left);
		}
		break;
	}
	case EVENT::JOY_HAT_RIGHT: {
		m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::WALK));
		m_player->SetState(CHARA_STATE::X_ADD, g_playerMoveX);

		if (m_player->GetState(CHARA_STATE::DIR) == g_left) {//����������E�����ɂȂ�Ƃ�
			SDL_Rect pos = m_player->GetPos();
			const short action = m_player->GetState(CHARA_STATE::ACTION);
			vector<int> fRect = FlipRect(m_hurtRect[1][action][0], IMG_SIZE[playerID]);
			pos.x -= (m_hurtRect[1][action][0][X] - fRect[X]);
			m_player->SetPos(pos);
			m_player->SetState(CHARA_STATE::DIR, g_right);
		}
		break;
	}
	case EVENT::JOY_HAT_CENTERED: {
		m_player->SetState(CHARA_STATE::X_ADD, 0);
		m_player->SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER_ACTION::STAND));
		break;
	}
	}
	return true;
}

void Game::Draw(SDL_Renderer *renderer) {
	int windowPosX = (m_player->GetPos().x + m_hurtRect[1][static_cast<short>(PLAYER_ACTION::STAND)][0][X]) - (WINDOW_WIDTH * MAP_CHIPSIZE / 2);//�`��̈�̍��[
	if (windowPosX < 0) { windowPosX = 0; }
	else if (windowPosX + WINDOW_WIDTH * MAP_CHIPSIZE > WORLD_WIDTH * MAP_CHIPSIZE) { windowPosX = (WORLD_WIDTH - WINDOW_WIDTH) * MAP_CHIPSIZE; }
	SDL_Rect src = { windowPosX, 0, WINDOW_WIDTH * MAP_CHIPSIZE, WINDOW_HEIGHT * MAP_CHIPSIZE };
	SDL_RenderCopy(renderer, m_world, &src, NULL);

	unsigned int nowTime = SDL_GetTicks();
	const short charaNum = static_cast<short>(CHARA_ID::NUM);
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {

			auto UpdateAnimation = [renderer, id, nowTime, windowPosX](SDL_Texture **charaTexture, Character &chara, vector<vector<int>> maxFrame, vector<vector<ENEMY_ACTION>> m_enemyAction, vector<vector<vector<vector<int>>>> hurtRect, vector<vector<int>> m_mapData) {
				const short charaType = id / (static_cast<int>(CHARA_ID::BOSS) + 1);
				short action = chara.GetState(CHARA_STATE::ACTION);
				short actionIndex = action;
				if (id < static_cast<short>(CHARA_ID::BOSS)) {
					actionIndex = ActionToIndex(m_enemyAction[id], action);
				}

				vector<short> frame = { chara.GetState(CHARA_STATE::FRAME_H), chara.GetState(CHARA_STATE::FRAME_V) };
				if (nowTime - chara.GetTime() >= ANIME_INTERVAL) {
					//�A�j���[�V�����X�V
					bool endAnimation = false;
					frame = RetrunNextFrame(frame[0], frame[1], maxFrame[actionIndex], endAnimation);
					const short walk[2] = { static_cast<short>(ENEMY_ACTION::WALK), static_cast<short>(PLAYER_ACTION::WALK) };
					const short guard[2] = { static_cast<short>(ENEMY_ACTION::GUARD), static_cast<short>(PLAYER_ACTION::GUARD) };
					const short dead[2] = { static_cast<short>(ENEMY_ACTION::DEAD), static_cast<short>(PLAYER_ACTION::DEAD) };

					if ((endAnimation == true) && (action != walk[charaType]) && (action != guard[charaType]) && (action != dead[charaType])) {
						if (id == 0) {
							chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY_ACTION::GUARD));
						}
						else {
							chara.SetState(CHARA_STATE::ACTION, 0);
							chara.SetState(CHARA_STATE::X_ADD, 0);
						}
					}
					chara.SetState(CHARA_STATE::FRAME_H, frame[0]);
					chara.SetState(CHARA_STATE::FRAME_V, frame[1]);
					chara.SetTime(nowTime);
				}

				SDL_Rect srcChara = { frame[0] * IMG_SIZE[id], frame[1] * IMG_SIZE[id], IMG_SIZE[id], IMG_SIZE[id] };
				SDL_Rect dstRect = chara.GetPos();
				dstRect.x -= windowPosX;

				vector<int> hRect = hurtRect[charaType][action][ReturnFrameNum(maxFrame[action][0], chara)];
				if (chara.GetState(CHARA_STATE::DIR) == g_right) {//�E����
					SDL_RenderCopy(renderer, charaTexture[action], &srcChara, &dstRect);
				}
				else {//��
					SDL_RenderCopyEx(renderer, charaTexture[action], &srcChara, &dstRect, 180, NULL, SDL_FLIP_VERTICAL);

					hRect = FlipRect(hRect, IMG_SIZE[id]);//�����蔻�蔽�]
				}
				SDL_Rect pos = chara.GetPos();
				if (chara.GetState(CHARA_STATE::X_ADD) > 0) {//x�����̈ړ�������Ƃ�
					pos = MovePositionX(pos, hRect, chara.GetState(CHARA_STATE::X_ADD) * chara.GetState(CHARA_STATE::DIR), WORLD_WIDTH, m_mapData);
				}
				short yAdd = chara.GetState(CHARA_STATE::Y_ADD);
				chara.SetPos(MovePositionY(pos, hRect, yAdd, WORLD_HEIGHT, m_mapData));
				chara.SetState(CHARA_STATE::Y_ADD, yAdd);
			};

			if (id == playerID) {
				UpdateAnimation(m_characterTexture[id], *m_player, m_maxFrame[id], m_enemyAction, m_hurtRect, m_mapData);
			}
			else {
				UpdateAnimation(m_characterTexture[id], *m_enemy[id][cn], m_maxFrame[id], m_enemyAction, m_hurtRect, m_mapData);
			}
			
			/*
			const short charaType = id / (static_cast<int>(CHARA_ID::BOSS) + 1);
			short action = m_enemy[id][cn]->GetState(CHARA_STATE::ACTION);
			short actionIndex = action;
			if (id < static_cast<short>(CHARA_ID::BOSS)) {
				actionIndex = ActionToIndex(m_enemyAction[id], action);
			}
			
			vector<short> frame = { m_enemy[id][cn]->GetState(CHARA_STATE::FRAME_H), m_enemy[id][cn]->GetState(CHARA_STATE::FRAME_V) };
			if (nowTime - m_enemy[id][cn]->GetTime() >= ANIME_INTERVAL) {
				//�A�j���[�V�����X�V
				bool endAnimation = false;
				frame = RetrunNextFrame(frame[0], frame[1], m_maxFrame[id][actionIndex], endAnimation);
				const short walk[2] = { static_cast<short>(ENEMY_ACTION::WALK), static_cast<short>(PLAYER_ACTION::WALK) };
				const short guard[2] = { static_cast<short>(ENEMY_ACTION::GUARD), static_cast<short>(PLAYER_ACTION::GUARD) };
				const short dead[2] = { static_cast<short>(ENEMY_ACTION::DEAD), static_cast<short>(PLAYER_ACTION::DEAD) };
				if ((endAnimation == true) && (action != walk[charaType]) && (action != guard[charaType]) && (action != dead[charaType])) {
					if (id == 0) {
						m_enemy[id][cn]->SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY_ACTION::GUARD));
					}
					else {
						m_enemy[id][cn]->SetState(CHARA_STATE::ACTION, 0);
						m_enemy[id][cn]->SetState(CHARA_STATE::X_ADD, 0);
					}
				}

				m_enemy[id][cn]->SetState(CHARA_STATE::FRAME_H, frame[0]);
				m_enemy[id][cn]->SetState(CHARA_STATE::FRAME_V, frame[1]);
				m_enemy[id][cn]->SetTime(nowTime);
			}

			SDL_Rect srcChara = { frame[0] * IMG_SIZE[id], frame[1] * IMG_SIZE[id], IMG_SIZE[id], IMG_SIZE[id] };
			SDL_Rect dstRect = m_enemy[id][cn]->GetPos();
			dstRect.x -= windowPosX;
			
			vector<int> hurtRect = m_hurtRect[charaType][action][ReturnFrameNum(m_maxFrame[id][action][0], *m_enemy[id][cn])];
			if (m_enemy[id][cn]->GetState(CHARA_STATE::DIR) == g_right) {//�E����
				SDL_RenderCopy(renderer, m_characterTexture[id][action], &srcChara, &dstRect);
			}
			else {//��
				SDL_RenderCopyEx(renderer, m_characterTexture[id][action], &srcChara, &dstRect, 180, NULL, SDL_FLIP_VERTICAL);

				hurtRect = FlipRect(hurtRect, IMG_SIZE[id]);//�����蔻�蔽�]
			}
			SDL_Rect pos = m_enemy[id][cn]->GetPos();
			if (m_enemy[id][cn]->GetState(CHARA_STATE::X_ADD) > 0) {//x�����̈ړ�������Ƃ�
				pos = MovePositionX(pos, hurtRect, m_enemy[id][cn]->GetState(CHARA_STATE::X_ADD) * m_enemy[id][cn]->GetState(CHARA_STATE::DIR), WORLD_WIDTH, m_mapData);
			}
			short yAdd = m_enemy[id][cn]->GetState(CHARA_STATE::Y_ADD);
			m_enemy[id][cn]->SetPos(MovePositionY(pos, hurtRect, yAdd, WORLD_HEIGHT, m_mapData));
			m_enemy[id][cn]->SetState(CHARA_STATE::Y_ADD, yAdd);
			*/
		}
	}

}

EVENT GetEvent(SDL_Joystick *joystick) {
	if (!SDL_JoystickGetAttached(joystick)) {//�܂��J���ĂȂ��Ƃ�
		joystick = SDL_JoystickOpen(0);
	}

	SDL_Event event;
	if (SDL_PollEvent(&event)) {//�L���[�ɃC�x���g���������Ƃ�
		switch (event.type) {
		case SDL_JOYBUTTONDOWN:
			if (SDL_JoystickGetButton(joystick, 1)) {//�Z�{�^��
				return EVENT::JOY_CIRCLE;
			}
			else if (SDL_JoystickGetButton(joystick, 0)) {//�~�{�^��
				return EVENT::JOY_CROSS;
			}
			else if (SDL_JoystickGetButton(joystick, 9)) {//L1
				return EVENT::JOY_L1_DOWN;
			}
			else if (SDL_JoystickGetButton(joystick, 10)) {//R1
				return EVENT::JOY_R1;
			}
			else if (SDL_JoystickGetButton(joystick, 13)) {//��
				return EVENT::JOY_HAT_LEFT;
			}
			else if (SDL_JoystickGetButton(joystick, 14)) {//�E
				return EVENT::JOY_HAT_RIGHT;
			}
			break;
		case SDL_JOYBUTTONUP:
			if ((int)event.jbutton.button == 9) {//L1
				return EVENT::JOY_L1_UP;
			}
			else if ((int)event.jbutton.button == 13 || (int)event.jbutton.button == 14) {
				return EVENT::JOY_HAT_CENTERED;
			}
			break;
		case SDL_JOYHATMOTION: {//�\���L�[�������ꂽ�Ƃ�
			switch (SDL_JoystickGetHat(joystick, 0)) {
			case SDL_HAT_RIGHT:
				return EVENT::JOY_HAT_RIGHT;
				break;
			case SDL_HAT_LEFT:
				return EVENT::JOY_HAT_LEFT;
				break;
			default:
				return EVENT::JOY_HAT_CENTERED;
				break;
			}
			break;
		}
		case SDL_QUIT: {//�E�B���h�E�́~�{�^��
			return EVENT::QUIT;
			break;
		}
		}
	}
	return EVENT::NONE;
}

inline void OutputError(const char* msg) {
	cout << msg << ":" << SDL_GetError() << endl;  exit(1);
}

SDL_Color ConvertToRGB(const char* color) {
	if (color == "red") {
		SDL_Color c = { 255, 0, 0, 255 };
		return c;
	}
	else if (color == "green") {
		SDL_Color c = { 0, 255, 0, 255 };
		return c;
	}
	else if (color == "blue") {
		SDL_Color c = { 0, 0, 255, 255 };
		return c;
	}
	else if (color == "yellow") {
		SDL_Color c = { 255, 255, 0, 255 };
		return c;
	}
	else if (color == "black") {
		SDL_Color c = { 0, 0, 0, 255 };
		return c;
	}
	else if (color == "white") {
		SDL_Color c = { 255, 255, 255, 255 };
		return c;
	}
	else {
		cout << color << "is not ready or not exist." << endl;
		exit(1);
	}
}

void CreateText(SDL_Renderer *renderer, SDL_Texture *texture, const char *text, int size, const char *color) {
#ifdef _DEBUG
	const char* fontPath = "../font/ipaexg.ttf";
#endif

#ifdef NDEBUG
	const char* fontPath = "../../font/ipaexg.ttf";
#endif

	TTF_Font *font = TTF_OpenFont(fontPath, size);
	if (!font) {
		cout << "TTF_OpenFont:" << TTF_GetError() << endl;
		exit(1);
	}

	SDL_Color fontColor = ConvertToRGB(color);
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, fontColor);
	if (surface == NULL) {
		OutputError("failed to create font surface");
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		OutputError("failed to create texture");
	}
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
}

vector<string> ReadFile(string fileName) {
	std::ifstream ifs(fileName);
	if (!ifs) {
		cout << "failed to open file:" << fileName << endl;
		exit(1);
	}

	vector<string> data;
	std::string str;
	while (getline(ifs, str)) {
		data.push_back(str);
	}

	return data;
}

vector<int> split(string str, const char mark) {
	stringstream ss{ str };
	string buf;
	vector<int> data;
	while (getline(ss, buf, mark)) {
		data.push_back(atoi(buf.c_str()));
	}
	return data;
}

vector<vector<int>> ReadFileSplit(string fileName, const char mark) {
	ifstream ifs(fileName);
	if (!ifs) {
		cout << "failed to open file:" << fileName << endl;
		exit(1);
	}

	string str;
	vector<vector<int>> data;
	while (getline(ifs, str)) {
		data.push_back(split(str, mark));
	}
	return data;
}

void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData) {
	for (int w = 0; w < WORLD_WIDTH; w++) {
		for (int h = 0; h < WORLD_HEIGHT; h++) {
			SDL_Rect src = { MAP_CHIPSIZE * mapData[w][h], 0 , MAP_CHIPSIZE, MAP_CHIPSIZE }, dst = { MAP_CHIPSIZE * w, MAP_CHIPSIZE * h, MAP_CHIPSIZE, MAP_CHIPSIZE };
			SDL_BlitSurface(mapChip, &src, world, &dst);
		}
	}
}

vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation) {
	endAnimation = false;
	if (++hengFrame >= maxFrame[0]) {
		hengFrame = 0;
		if (++verticalFrame >= maxFrame[1]) {
			verticalFrame = 0;
			endAnimation = true;
		}
	}

	vector<short> nextFrame = { hengFrame, verticalFrame };
	return nextFrame;
}

template <typename T>
const int ActionToIndex( vector<T> actionList, int nowAction) {
	T action = static_cast<T>(nowAction);
	int min = 0;
	int max = actionList.size();

	while (min < max) {
		int mid = (max + min) / 2;
		if (actionList[mid] == action) {
			return mid;
		}
		else if (action < actionList[mid]) {
			max = mid;
		}
		else {
			min = mid;
		}
	}

	cout << "that action is not exist." << endl;
	return -1;
}

SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData) {
	const int prevX = nowPos.x;
	nowPos.x += distance;
	const int charaPos = nowPos.x + hurtRect[X];
	if (charaPos < 0) {
		nowPos.x = -hurtRect[X];
	}
	else if(charaPos + hurtRect[W] > worldWidth * MAP_CHIPSIZE) {
		nowPos.x = worldWidth * MAP_CHIPSIZE - (hurtRect[X] + hurtRect[W]);
	}
	
	CollisionMapX(nowPos, prevX, hurtRect, mapData);
	return nowPos;
}

SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData) {
	yAdd++;
	const int prevY = nowPos.y;
	nowPos.y += yAdd;
	
	const int charaPos = nowPos.y + hurtRect[Y];
	if (charaPos < 0) {
		nowPos.y = -hurtRect[Y];
	}
	else if (charaPos + hurtRect[H] > worldHeight * MAP_CHIPSIZE) {
		nowPos.y = worldHeight * MAP_CHIPSIZE - (hurtRect[Y] + hurtRect[H]);
	}

	if (CollisionMapY(nowPos, nowPos.y, hurtRect, mapData)) {
		yAdd = g_yAdd_ground;
	}
	
	return nowPos;
}

inline vector<int> FlipRect(vector<int> rect, int imgWidth) {
	rect[X] = imgWidth - (rect[X] + rect[W]);
	return rect;
}


void CollisionMapX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData) {
	const short left = (nowPos.x + hurtRect[X]) / MAP_CHIPSIZE;
	short right = (nowPos.x + hurtRect[X] + hurtRect[W] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (right > WORLD_WIDTH) { right = WORLD_WIDTH; }

	const short top = (nowPos.y + hurtRect[Y]) / MAP_CHIPSIZE;
	short bottom = (nowPos.y + hurtRect[Y] + hurtRect[H]) / MAP_CHIPSIZE;
	if (bottom > WORLD_HEIGHT) { bottom = WORLD_HEIGHT; }

	for (int w = left; w < right; w++) {
		for (int h = top; h < bottom; h++) {
			if (mapData[w][h] >= 1) {
				int x = max(nowPos.x, prevX);
				x /= MAP_CHIPSIZE;
				if (nowPos.x - prevX < 0) { x++; }
				x *= MAP_CHIPSIZE;
				nowPos.x = x;
			}
		}
	}
}

bool CollisionMapY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData) {
	bool landing = false;//���n�������ۂ�
	const short left = (nowPos.x + hurtRect[X]) / MAP_CHIPSIZE;
	short right = (nowPos.x + hurtRect[X] + hurtRect[W] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (right > WORLD_WIDTH) { right = WORLD_WIDTH; }
	const short top = (nowPos.y + hurtRect[Y]) / MAP_CHIPSIZE;
	short bottom = (nowPos.y + hurtRect[Y] + hurtRect[H] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (bottom > WORLD_HEIGHT) {
		bottom = WORLD_HEIGHT;
	}
	
	for (int w = left; w < right; w++) {
		for (int h = top; h < bottom; h++) {
			if (mapData[w][h] >= 1) {
				int y = max(nowPos.y, prevY) + hurtRect[Y] + hurtRect[H];
				y /= MAP_CHIPSIZE;
				y *= MAP_CHIPSIZE;
				nowPos.y = y - hurtRect[Y] - hurtRect[H];

				landing = true;
			}
		}
	}

	return landing;
}

SDL_Rect ReturnCharaRect(SDL_Rect pos, vector<int> rect, short dir) {
	if (dir == g_left) { rect = FlipRect(rect, pos.w); }
	pos.x += rect[X];
	pos.y += rect[Y];
	pos.w = rect[W];
	pos.h = rect[H];

	return pos;
}

bool DetectCollisionRect(SDL_Rect r0, SDL_Rect r1) {
	int distanceX = abs((r0.x + (r0.w / 2)) - (r1.x + (r1.w / 2)));
	int distanceY = abs((r0.y + (r0.h / 2)) - (r1.y + (r1.h / 2)));

	return (distanceX < ((r0.w + r1.w) / 2) && distanceY < ((r0.h + r1.h) / 2)) ? true : false;
}

inline int ReturnFrameNum(int maxFrameHeng, Character chara) {
	return chara.GetState(CHARA_STATE::FRAME_V) * maxFrameHeng + chara.GetState(CHARA_STATE::FRAME_H);
}