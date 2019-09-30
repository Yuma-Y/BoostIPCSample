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

	bool create();				// €”õ
	bool send(string message);	// ‘—M
	string receive();			// óM
	bool destroy();				// ”jŠü

	bool hasNewMessage();

	string getClassName();

private:
	boost::interprocess::interprocess_mutex* mutex;
};
