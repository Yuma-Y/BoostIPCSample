#pragma once

#include "IPC_IF.hpp"

namespace boost {
	namespace interprocess {
		class shared_memory_object;
		class interprocess_mutex;
	}
}

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

private:
	boost::interprocess::shared_memory_object* shm;
	boost::interprocess::interprocess_mutex* mutex;
};

