
#include "BoostSharedMemReceive.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

#include <iostream>
#include <cstring>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/filesystem.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

using namespace std;
using namespace boost::interprocess;

// 共有メモリに読み書きする内容
typedef struct {
	boost::interprocess::interprocess_mutex mutex;
	boost::interprocess::interprocess_condition condition;
	string message;
} IPCData;

BoostSharedMemReceive::BoostSharedMemReceive()
{
}

BoostSharedMemReceive::~BoostSharedMemReceive()
{
}

bool BoostSharedMemReceive::create()
{
	return false;
}

bool BoostSharedMemReceive::send(string message)
{
	return false;
}

string BoostSharedMemReceive::receive()
{
	string ret;

	try {
		shared_memory_object shm(open_only, "MY_SHARED_MEMORY", read_write);

		mapped_region region(shm, read_write);
		void* addr = region.get_address();
		IPCData* data = static_cast<IPCData*>(addr);

		scoped_lock<interprocess_mutex> lock(data->mutex);
		ret = data->message;
		// notifyで条件変数を解除する。
		// これで、wait()したプロセスが動き始める。
		// notify_one()以外にも、waitしている他のプロセス全てに通知するnotify_all()もある。
		data->condition.notify_one();

		// 読んだ後メモリの内容をクリアする
		// std::memset(region.get_address(), 0, region.get_size());

		// ちなみに、条件変数を使うと、メモリごとクリアすると正しく解除できなくなってしまう。
		// そのため、messageのみクリアする。
		data->message.clear();
	}
	catch (interprocess_exception& e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

bool BoostSharedMemReceive::destroy()
{
	return false;
}

bool BoostSharedMemReceive::hasNewMessage()
{
	bool ret = false;
	
	// まず共有メモリファイルがあるかどうかを確かめる
	const boost::filesystem::path path(BOOST_INTERPROCESS_SHARED_DIR_PATH + "\\MY_SHARED_MEMORY");

	boost::system::error_code error;
	const bool result = boost::filesystem::exists(path, error);
	if (result && !error) {
		try {
			shared_memory_object shm(open_only, "MY_SHARED_MEMORY", read_only);

			mapped_region region(shm, read_only);
			void* addr = region.get_address();

			IPCData* data = static_cast<IPCData*>(addr);
			if (!data->message.empty()) {
				ret = true;
			}
		}
		catch (interprocess_exception& e) {
			cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
		}
	}

	return ret;
}

string BoostSharedMemReceive::getClassName()
{
	return string("BoostSharedMemReceive");
}
