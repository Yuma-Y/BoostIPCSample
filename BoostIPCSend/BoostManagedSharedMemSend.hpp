#pragma once

#include "IPC_IF.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

// managed_shared_memoryは別クラスのtypedefなので前方宣言できない
#include <boost/interprocess/managed_shared_memory.hpp>

namespace boost {
	namespace interprocess {
		class named_mutex;
	}
}

class BoostManagedSharedMemSend : public IPC_IF
{
public:
	BoostManagedSharedMemSend();
	~BoostManagedSharedMemSend();

	bool create();				// 準備
	bool send(string message);	// 送信
	string receive();			// 受信
	bool destroy();				// 破棄

	bool hasNewMessage();

	string getClassName();

private:
	boost::interprocess::managed_shared_memory* shm;
	boost::interprocess::named_mutex* mutex;
};

