#pragma once

#include "IPC_IF.hpp"

class BoostManagedSharedMemReceive : public IPC_IF
{
public:
	BoostManagedSharedMemReceive();
	~BoostManagedSharedMemReceive();

	bool create();				// €”õ
	bool send(string message);	// ‘—M
	string receive();			// óM
	bool destroy();				// ”jŠü

	bool hasNewMessage();

	string getClassName();
};

