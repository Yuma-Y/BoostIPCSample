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
		
		// ���b�Z�[�W�L���[�ł́A����M���o�������b�Z�[�W�̃T�C�Y�ƌ^��m���Ă����K�v������B
		// �܂��V���A���C�Y�\�Ȍ^��������Ȃ����߁A��r�I�P���ȃ��b�Z�[�W�̑���M�Ɍ����Ă���B
		char buf[100] = { '\0' };
		int num = mq.get_num_msg();		// get_num_msg()�łƂ�郁�b�Z�[�W����receive()���邽�ь����Ă����悤�Ȃ̂őޔ����Ă���
		for (int i = 1; i <= num; i++) {
			// ��2�����̃o�b�t�@�T�C�Y������Ȃ���
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
