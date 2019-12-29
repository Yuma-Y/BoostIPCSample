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
		// already_exists_error(=共有メモリがすでにある)だったらいったん削除して再度create()する
		// (open_or_create指定してるからすでにあってもよさそうなのになぜかexceptionになるので)
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
		named_semaphore sem(open_or_create, "semaphore", 1);	// 同期用の名前付きセマフォ。
		sem.wait();

		// message_queueではstd::stringのような、シリアライズ（＝バイト化）できない型をそのまま送ることはできない。
		// そのため、charに変換する
		for (int i = 0; i < message.length(); i++) {
			// sendする際に引数で優先度を指定できる。
			// 優先度を上げておくと、優先度の高いものから先にreceiveされる。
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
	// read前にメッセージキューを消してしまわないよう、一度wait()する
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
