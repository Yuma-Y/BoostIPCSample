#pragma once

#include "IPC_IF.hpp"

namespace boost {
	namespace interprocess {
		class named_mutex;
	}
}

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

private:
	boost::interprocess::named_mutex* mutex;
};

