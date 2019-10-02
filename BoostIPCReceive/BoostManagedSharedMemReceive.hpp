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

	bool create();				// €”õ
	bool send(string message);	// ‘—M
	string receive();			// óM
	bool destroy();				// ”jŠü

	bool hasNewMessage();

	string getClassName();

private:
	boost::interprocess::named_mutex* mutex;
};

