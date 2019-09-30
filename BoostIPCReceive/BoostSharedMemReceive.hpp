#pragma once
#include "IPC_IF.hpp"

namespace boost {
	namespace interprocess {
		class interprocess_mutex;
	}
}

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

private:
	boost::interprocess::interprocess_mutex* mutex;
};
