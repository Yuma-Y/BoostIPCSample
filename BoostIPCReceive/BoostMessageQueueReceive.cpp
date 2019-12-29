#include "BoostMessageQueueReceive.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

#include <iostream>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/exceptions.hpp>

using namespace std;
using namespace boost::interprocess;

BoostMessageQueueReceive::BoostMessageQueueReceive()
{
}

BoostMessageQueueReceive::~BoostMessageQueueReceive()
{
}

bool BoostMessageQueueReceive::create()
{
	return false;
}

bool BoostMessageQueueReceive::send(string message)
{
	return false;
}

string BoostMessageQueueReceive::receive()
{
	string ret;

	try {
		message_queue mq(open_only, "message_queue");
		named_semaphore sem(open_only, "semaphore");

		unsigned int priority;
		message_queue::size_type recvd_size;
		
		// メッセージキューでは、送受信側双方がメッセージのサイズと型を知っておく必要がある。
		// またシリアライズ可能な型しか送れないため、比較的単純なメッセージの送受信に向いている。
		char buf[100] = { '\0' };
		int num = mq.get_num_msg();		// get_num_msg()でとれるメッセージ数はreceive()するたび減っていくようなので退避しておく
		for (int i = 1; i <= num; i++) {
			// 第2引数のバッファサイズが足らないと
			mq.receive(&buf[i - 1], sizeof(buf), recvd_size, priority);
			ret.push_back(buf[i - 1]);
		}
		
		sem.post();
	}
	catch (interprocess_exception & e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

bool BoostMessageQueueReceive::destroy()
{
	return false;
}

bool BoostMessageQueueReceive::hasNewMessage()
{
	bool ret = false;

	try {
		message_queue mq(open_only, "message_queue");

		if (mq.get_num_msg() != 0) {
			ret = true;
		}
	}
	catch (interprocess_exception & e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostMessageQueueReceive::getClassName()
{
	return string("BoostMessageQueueReceive");
}
