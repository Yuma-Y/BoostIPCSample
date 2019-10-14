#pragma once

#include "IPC_IF.hpp"

class BoostSharedMemSend : public IPC_IF
{
public:
	BoostSharedMemSend();
	~BoostSharedMemSend();

	bool create();				// ����
	bool send(string message);	// ���M
	string receive();			// ��M
	bool destroy();				// �j��

	bool hasNewMessage();

	string getClassName();
};

