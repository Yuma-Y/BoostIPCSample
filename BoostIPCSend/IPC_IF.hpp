#pragma once

#include <string>
using namespace std;
// boost�̊e�ʐM���J�j�Y���������N���X�͈ȉ��̃N���X���p�����A
// ����ɍ��킹�Ď����A���C������Ăяo�����ƂƂ���B
class IPC_IF {
public:
	IPC_IF() {}
	virtual ~IPC_IF() {}

	virtual bool create() = 0;	// ����
	virtual bool send(string message) = 0;	// ���M
	virtual string receive() = 0;	// ��M
	virtual bool destroy() = 0;	// �j��

	virtual bool hasNewMessage() = 0; // ���b�Z�[�W����H
};