#pragma once

#include "IPC_IF.hpp"

namespace boost {
	namespace interprocess {
		class file_mapping;
		class file_lock;
	}
}

class BoostMmapFileSend : public IPC_IF
{
public:
	BoostMmapFileSend();
	~BoostMmapFileSend();

	bool create();
	bool send(string message);
	string receive();
	bool destroy();

	bool hasNewMessage();

	string getClassName();

private:
	boost::interprocess::file_mapping* file_map;
	boost::interprocess::file_lock* lock;
};

