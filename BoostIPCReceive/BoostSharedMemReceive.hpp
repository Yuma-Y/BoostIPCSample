#pragma once

#include "IPC_IF.hpp"


class BoostSharedMemReceive : public IPC_IF
{
public:
	BoostSharedMemReceive();
	~BoostSharedMemReceive();

	bool create();				// ����
	bool send(string message);	// ���M
	string receive();			// ��M
	bool destroy();				// �j��

	bool hasNewMessage();

	string getClassName();
};
