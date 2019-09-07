#pragma once

#include "IPC_IF.hpp"

namespace boost {
	namespace interprocess {
		class shared_memory_object;
	}
}

class BoostSharedMemSend : public IPC_IF
{
public:
	BoostSharedMemSend();
	~BoostSharedMemSend();

	bool create();				// €”õ
	bool send(string message);	// ‘—M
	string receive();			// óM
	bool destroy();				// ”jŠü

	bool hasNewMessage();

private:
	boost::interprocess::shared_memory_object* shm;
};

