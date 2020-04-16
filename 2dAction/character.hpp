#pragma once
#include <SDL.h>

const short g_right = 1;
const short g_left = -1;

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
	ATTACK_ACTIVE
};

class Character {
private:
	short m_dir;//����
	short m_hp;
	short m_power;//�U����
	short m_defense;//�h���
	//int m_trunk;//�̊�
	SDL_Rect m_pos;//�ʒu(�L�����̔���ł͂Ȃ��摜�S��) 
	short m_vertical;//���݂̃A�j���[�V�����R�}��(�c)
	short m_heng;//���݂̃A�j���[�V�����R�}��(��)
	short m_action;
	short m_xAdd;//x�����ړ���
	short m_yAdd;//y�����ړ���
	unsigned int m_prevTime;//�O��̃A�j���[�V�����X�V����
	unsigned int m_hurtActive;//���炢����̂���t���[��
	unsigned int m_attackActive;//�U������̂���t���[��
public:
	Character();
	virtual ~Character();	

	//�w�肳�ꂽ�p�����[�^��Ԃ�
	//request�F�w�肳�ꂽ�p�����[�^��
	//�Ԓl�F�p�����[�^
	virtual const short GetState(CHARA_STATE request);

	//�w�肳�ꂽ�p�����[�^��ݒ肷��
	//request�F�w�肳�ꂽ�p�����[�^��
	//state�F�ύX��̒l
	virtual void SetState(CHARA_STATE request, short state);

	//���݂̍��W��Ԃ�
	const SDL_Rect GetPos();

	//���W��ύX����
	//nextPos�F�ύX��̍��W
	void SetPos(SDL_Rect nextPos);
	
	//�O��̃A�j���[�V�����X�V�^�C�~���O�擾
	const unsigned int GetTime();

	//�Ō�̃A�j���[�V�����X�V�^�C�~���O�ۑ�
	void SetTime(unsigned int time);

	//frame�R�}�ڂ̂��炢���肪�L�����Ԃ�
	//request�FHURT_ACTIVE or ATTACK_ACTIVE
	//frame�F�m�F�������R�}��
	//�Ԓl 0�F�L���łȂ��ꍇ 1�F����ȊO
	const unsigned int GetActiveBit(CHARA_STATE request, int frame);

	//�����̃r�b�g����Z�b�g����
	//request�FHURT_ACTIVE or ATTACK_ACTIVE
	//bit�F�Z�b�g����r�b�g��
	void SetActiveBit(CHARA_STATE request, unsigned int bit);
protected:
	//�p�����[�^�̏�����
	void InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
};



class Player : public Character {
private:
	
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	//const short GetState(CHARA_STATE request) override;
	//void SetState(CHARA_STATE request, int state) override;
};