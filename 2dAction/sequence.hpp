#pragma once
#include <SDL.h>
#include <vector>

using namespace std;

enum class EVENT;
namespace ENEMY {
	enum class ACTION;
}

//�e�V�[�P���X�̒l
enum class SEQ_ID {
	MENU,          //���[�h�I��
	CONFIG,        //�ݒ�
	GAME,          //�Q�[�����
	GAME_CLEAR,    //�Q�[���I��(�J�ڐ�I�����)
	GAME_OVER,     //�Q�[���I��(�J�ڐ�I�����)
	NONE,          //�����l
	QUIT           //�I��
};

//�V�[�P���X�J�ڂ̊��N���X
class Sequence {
protected:
	SEQ_ID m_next;//���̃V�[�P���X
public:
	Sequence();
	virtual ~Sequence() {};
	virtual bool Update(SDL_Renderer *renderer) = 0;

	//�V�[�P���X�J�ڂ��s��
	//�Ԓl:���̃V�[�P���X
	SEQ_ID MoveTo();
};


//�C�x���g�擾
//SDL_Joystick *joystick�F�W���C�X�e�B�b�N�̍\����
//�Ԓl�F�擾�����C�x���g
EVENT GetEvent(SDL_Joystick *joystick);

//�G���[�o��
//const char* msg:�o�͂���G���[��
//�Ԓl:�Ȃ�
inline const void OutputError(const char *msg);

//�F���ɑΉ�����RGB�l��Ԃ�
//const char* color:�`��F��
//�Ԓl:�`��F�̃p�����[�^
SDL_Color ConvertToRGB(const char *color);