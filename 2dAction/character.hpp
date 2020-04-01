#pragma once

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
	Y_ADD
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
	unsigned int m_prevTime;//�O��̃A�j���[�V�����X�V����
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
	//�Ԓl�F�Ȃ�
	virtual void SetState(CHARA_STATE request, int state);

	//���݂̍��W��Ԃ�
	const SDL_Rect GetPos();

	//���W��ύX����
	//nextPos�F�ύX��̍��W
	void SetPos(SDL_Rect nextPos);
	
	const unsigned int GetTime();

	void SetTime(unsigned int time);

protected:
	//�p�����[�^�̏�����
	void InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time);
};

class Player : public Character {
private:
	short m_yAdd;//y�����ړ���
public:
	Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
	//~Player();

	const short GetState(CHARA_STATE request) override;
	void SetState(CHARA_STATE request, int state) override;
};

class Enemy : public Character {
private:

public:
	Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time);
	//~Enemy();

	//const short GetState(CHARA_STATE request);
	//void SetState(CHARA_STATE request, int state);
};