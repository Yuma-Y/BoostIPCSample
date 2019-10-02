#pragma once

#include "IPC_IF.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

// managed_shared_memory�͕ʃN���X��typedef�Ȃ̂őO���錾�ł��Ȃ�
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

	bool create();				// ����
	bool send(string message);	// ���M
	string receive();			// ��M
	bool destroy();				// �j��

	bool hasNewMessage();

	string getClassName();

private:
	boost::interprocess::managed_shared_memory* shm;
	boost::interprocess::named_mutex* mutex;
};

