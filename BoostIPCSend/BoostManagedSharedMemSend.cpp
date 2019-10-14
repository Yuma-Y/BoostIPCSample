#include "BoostManagedSharedMemSend.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

#include <iostream>
#include <cstring>
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
	_IPCData() : write_semaphore(1), read_semaphore(1), message(){}
	~_IPCData(){}
} IPCData;

BoostManagedSharedMemSend::BoostManagedSharedMemSend()
{
}

BoostManagedSharedMemSend::~BoostManagedSharedMemSend()
{
}

bool BoostManagedSharedMemSend::create()
{
	bool ret = false;

	try {
		managed_shared_memory shm(open_or_create, "MY_MANAGED_SHARED_MEMORY", 1024);

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

bool BoostManagedSharedMemSend::send(string message)
{
	bool ret = false;

	// managed_shared_segmentはこのように型と名前を指定してメモリを読み書きできる
	// ちなみに、同じ名前のオブジェクトを複数書き込もうとすると
	// interprocess_exceptionの9(=already_exists_error)となるが
	// receive側で読まれると同時に消される想定なのでチェックはしない

	try {
		// string型のまま書き込もうとするとなぜかうまくいかないので
		// char型配列に変換して読み書きする
		// デストラクタの中でいろいろやるような複雑なクラスのオブジェクトは
		// 送らないほうがよさそう。
		/* char array[1024] = "\0";
		std::memcpy(array, message.c_str(), message.length()); */

		// このように、construct_itを使うと配列そのものを渡すこともできる
		// constructなら配列でなくてもOK
		/* char* instance = shm->construct_it<char>
			("MyString")			// オブジェクトの固有名
			[message.length()]		// (配列の場合)要素数
			(&array[0]);			// 値
		*/

		// managed_shared_memory.find_or_construct()で１つのオブジェクトを使いまわすこともできる。
		managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
		IPCData* data = shm.find_or_construct<IPCData>("MyData")();

		// セマフォもmutex同様、特定セクションへのアクセスを制御する排他制御に使う。
		// wait()でセマフォをロックする。
		data->write_semaphore.wait();
		data->read_semaphore.wait();
		data->message = message;

		data->write_semaphore.post();

		ret = true;
	}
	catch (interprocess_exception& e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostManagedSharedMemSend::receive()
{
	return string();
}

bool BoostManagedSharedMemSend::destroy()
{
	// named_mutex::remove("named_mutex");

	// 共有メモリを削除する前に、wait()する。
	// receive側でpost()されれば以降の処理へ進める。
	// これで、receive側がquitメッセージを読む前にsend側が共有メモリを削除してしまうのを防げる。
	managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
	typedef pair<IPCData*, managed_shared_memory::size_type> MyType;
	MyType res = shm.find<IPCData>("MyData");
	IPCData* data = res.first;
	data->read_semaphore.wait();

	// managed_shared_memoryもshared_memory_object::remove()で消せる
	return shared_memory_object::remove("MY_MANAGED_SHARED_MEMORY");
}

bool BoostManagedSharedMemSend::hasNewMessage()
{
	return false;
}

string BoostManagedSharedMemSend::getClassName()
{
	return string("BoostManagedSharedMemSend");
}
