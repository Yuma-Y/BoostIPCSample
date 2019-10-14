#pragma once

#include "IPC_IF.hpp"

class BoostManagedSharedMemSend : public IPC_IF
{
public:
	BoostManagedSharedMemSend();
	~BoostManagedSharedMemSend();

	bool create();				// ����
	bool send(string message);	// ���M
	string receive();			// ��M
	bool destroy();				// �j��

	bool hasNewMessage();

	string getClassName();
};

