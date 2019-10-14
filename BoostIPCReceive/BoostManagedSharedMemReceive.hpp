#pragma once

#include "IPC_IF.hpp"

class BoostManagedSharedMemReceive : public IPC_IF
{
public:
	BoostManagedSharedMemReceive();
	~BoostManagedSharedMemReceive();

	bool create();				// ����
	bool send(string message);	// ���M
	string receive();			// ��M
	bool destroy();				// �j��

	bool hasNewMessage();

	string getClassName();
};

