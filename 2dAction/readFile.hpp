#pragma once
#include <vector>
#include <string>

using namespace std;

//�t�@�C���̓ǂݍ���
//string fileName�F�ǂݍ��ރt�@�C����
//�Ԓl�F�ǂݍ��񂾃f�[�^
vector<string> ReadFile(string fileName);

//str��mark�ŕ������A���l�ɕϊ��������̂�Ԃ�
//string str�F������
//const char mark�F��؂蕶��
//�Ԓl�F�ϊ���̐����z��
vector<int> split(string str, const char mark);

//��؂蕶���̂���f�[�^�̓ǂݍ���
//string fileName�F�t�@�C����
//const char mark�F��؂蕶��
//�Ԓl�F�ǂݍ��񂾃}�b�v�f�[�^
vector<vector<int>>ReadFileSplit(string fileName, const char mark);