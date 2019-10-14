#include "BoostManagedSharedMemReceive.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

using namespace std;
using namespace boost::interprocess;

// 共有メモリに読み書きする内容
typedef struct _IPCData {
	boost::interprocess::interprocess_semaphore write_semaphore;
	boost::interprocess::interprocess_semaphore read_semaphore;
	string message;

public:
	// interprocess_semaphoreはコンストラクタ引数でinitial_countを指定する。
	// なおセマフォはwaitでカウンタをデクリメント、postでインクリメントし、
	// ・内部のカウンタが0以下の状態のときにwait()すると、そのプロセスの実行が止まる。
	// ・post()してカウンタが0以上になると、止まっていたプロセスの実行が再開される。

	// initial_countが0だと最初にwriteする時点で止まってしまうので、1にしておく。
	_IPCData() : write_semaphore(1), read_semaphore(1), message() {}
	~_IPCData() {}
} IPCData;

BoostManagedSharedMemReceive::BoostManagedSharedMemReceive()
{
}

BoostManagedSharedMemReceive::~BoostManagedSharedMemReceive()
{
}

bool BoostManagedSharedMemReceive::create()
{
	return false;
}

bool BoostManagedSharedMemReceive::send(string message)
{
	return false;
}

string BoostManagedSharedMemReceive::receive()
{
	string ret;

	try {
		managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
		/*
		typedef pair<char*, managed_shared_memory::size_type> MyType;
		MyType res = shm.find<char>("MyString");
		if (res.second != 0) {
			for (int i = 0; i < res.second; i++) {
				ret.push_back(res.first[i]);
			}

			// 読んだ後メモリの内容をクリアする
			shm.destroy<char>("MyString");
		}
		*/
		typedef pair<IPCData*, managed_shared_memory::size_type> MyType;
		MyType res = shm.find<IPCData>("MyData");

		IPCData* data = res.first;
		
		data->read_semaphore.post();
		ret = data->message;

		// 読んだ後メモリの内容をクリアする
		data->message.clear();
	}
	catch (interprocess_exception& e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

bool BoostManagedSharedMemReceive::destroy()
{
	return false;
}

bool BoostManagedSharedMemReceive::hasNewMessage()
{
	bool ret = false;

	// まず共有メモリファイルがあるかどうかを確かめる
	const boost::filesystem::path path(BOOST_INTERPROCESS_SHARED_DIR_PATH + "\\MY_MANAGED_SHARED_MEMORY");

	boost::system::error_code error;
	const bool result = boost::filesystem::exists(path, error);
	if (result && !error) {
		try {
			managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
			// managed_shared_memoryの場合、このように
			// find<取りたい型>(オブジェクト名)を呼ぶことで
			// std::pair<取りたい型のポインタ, managed_shared_memory::size_type>が返ってくる。
			// 2要素目のsize_typeには書き込まれた要素数が入る。
			// constructで書き込むオブジェクトには配列も渡せるので、要素数のチェックもできる。
			typedef pair<IPCData*, managed_shared_memory::size_type> MyType;
			MyType res = shm.find<IPCData>("MyData");

			if ((res.second != 0) && (!res.first->message.empty())) {
				ret = true;
			}
		}
		catch (interprocess_exception& e) {
			cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
		}
	}

	return ret;
}

string BoostManagedSharedMemReceive::getClassName()
{
	return string("BoostManagedSharedMemReceive");
}
