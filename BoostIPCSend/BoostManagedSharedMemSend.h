#pragma once

#include "IPC_IF.hpp"

namespace boost {
	namespace interprocess {
		class managed_shared_memory;
	}
}

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

private:
	boost::interprocess::managed_shared_memory* shm;
};

