#include "BoostMessageQueueSend.hpp"

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

BoostMessageQueueSend::BoostMessageQueueSend()
{
}

BoostMessageQueueSend::~BoostMessageQueueSend()
{
}

bool BoostMessageQueueSend::create()
{
	bool ret = false;

	try {
		message_queue mq
		(open_or_create			  //open or create
			, "message_queue"     //name
			, 100                 //max message number
			, 100                 //max message size
		);

		ret = true;
	}
	catch (interprocess_exception & e) {
		// already_exists_error(=���L�����������łɂ���)�������炢������폜���čēxcreate()����
		// (open_or_create�w�肵�Ă邩�炷�łɂ����Ă��悳�����Ȃ̂ɂȂ���exception�ɂȂ�̂�)
		if (e.get_error_code() == already_exists_error) {
			destroy();
			create();
		}
		else {
			cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
		}
	}

	return ret;
}

bool BoostMessageQueueSend::send(string message)
{
	bool ret = false;

	try {
		message_queue mq(open_only, "message_queue");
		named_semaphore sem(open_or_create, "semaphore", 1);	// �����p�̖��O�t���Z�}�t�H�B
		sem.wait();

		// message_queue�ł�std::string�̂悤�ȁA�V���A���C�Y�i���o�C�g���j�ł��Ȃ��^�����̂܂ܑ��邱�Ƃ͂ł��Ȃ��B
		// ���̂��߁Achar�ɕϊ�����
		for (int i = 0; i < message.length(); i++) {
			// send����ۂɈ����ŗD��x���w��ł���B
			// �D��x���グ�Ă����ƁA�D��x�̍������̂�����receive�����B
			char ch = message[i];
			mq.send(&ch, sizeof(char), 0);

			ret = true;
		}
	}
	catch (interprocess_exception & e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostMessageQueueSend::receive()
{
	return string();
}

bool BoostMessageQueueSend::destroy()
{
	// read�O�Ƀ��b�Z�[�W�L���[�������Ă��܂�Ȃ��悤�A��xwait()����
	message_queue mq(open_only, "message_queue");
	named_semaphore sem(open_only, "semaphore");
	sem.wait();

	return message_queue::remove("message_queue");
}

bool BoostMessageQueueSend::hasNewMessage()
{
	return false;
}

string BoostMessageQueueSend::getClassName()
{
	return string("BoostMessageQueueSend");
}
