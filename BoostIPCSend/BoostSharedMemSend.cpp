#include "BoostSharedMemSend.hpp"

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

BoostSharedMemSend::BoostSharedMemSend()
{
}

BoostSharedMemSend::~BoostSharedMemSend()
{
}

bool BoostSharedMemSend::create()
{
	bool ret = false;

	try {
		shared_memory_object shm(open_or_create, "MY_SHARED_MEMORY", read_write);
		shm.truncate(1024);	// 作った直後は0KBのまま。ここで1KB以上のサイズにしないと、後でmapped_regionでexceptionが発生する

		ret = true;
	}
	catch (interprocess_exception& e) {
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

bool BoostSharedMemSend::send(string message)
{
	bool ret = false;

	try {
		// mapped_regionでshared_memory_objectを自プロセスにマッピングし、
		// このように直接読み書きできるようになる
		shared_memory_object shm(open_only, "MY_SHARED_MEMORY", read_write);
		mapped_region region(shm, read_write);
		void* addr = region.get_address();
		
		// こう書くと、addrを基準に(=共有メモリ内に)オブジェクトを作成できるようだ。
		IPCData* data = new (addr) IPCData;

		// mutexの場合、unlock()し忘れるとlock()以降のセクションにアクセスできなくなる
		// ちなみに、scoped_lockを使えばmutexをメンバ変数にしなくても
		// ローカル変数のスコープの間だけロックし、スコープを抜けると自動でアンロックしてくれる。
		// (scoped_lock<mutexの型>　変数を宣言した時点でロックされる）
		// interprocessのmutexには単純なlock以外にも
		// lockの結果がわかるtry_lock,
		// 設定した時間経過でロック解除されるtimed_lockがある。
		scoped_lock<interprocess_mutex> lock(data->mutex);
		data->message = message;

		// 条件変数でwaitする。
		// これで、他のプロセスからnotifyで解除されるまでこのプロセスの実行は止まる。
		// interprocess_consitionはscoped_lock<interprocess_mutex>型を渡してwaitする。
		data->condition.wait(lock);

		ret = true;
	}
	catch (interprocess_exception& e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostSharedMemSend::receive()
{
	// 送信専用なので空でいい
	return "";
}

bool BoostSharedMemSend::destroy()
{
	return shared_memory_object::remove("MY_SHARED_MEMORY");
}

bool BoostSharedMemSend::hasNewMessage()
{
	// 送信専用なので空でいい
	return false;
}

string BoostSharedMemSend::getClassName()
{
	return string("BoostSharedMemSend");
}
