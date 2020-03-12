#pragma once

using namespace std;
//#include "font.hpp"

class Sequence;
enum class ENEMY_ACTION;

class MainLoop {
private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	Sequence *m_sequence;//�V�[�P���X�J�ڗp
	SDL_Texture *m_loadingText;//���[�h��ʗp����
	vector<vector<ENEMY_ACTION>> m_enemyAction;//�e�G�����A�N�V����
	vector<vector<vector<int>>> m_maxFrame;//�e�A�j���[�V�����̃t���[����
	vector<vector<unsigned short>> m_attackActive;//�U������̂���t���[��
	vector<vector<vector<vector<int>>>> m_attackRect;//�U������͈�
	vector<vector<unsigned short>> m_hurtActive;//���炢����̂���t���[��
	vector<vector<vector<vector<int>>>> m_hurtRect;//���炢����͈�

public:
	//SDL_Texture* m_backgroundTexture;

	MainLoop();
	~MainLoop();

	//���[�v��񕪂̏���
	bool Update();

	//���[�h��ʂ̕\��
	//�Ԓl:�Ȃ�
	void Loading();
};