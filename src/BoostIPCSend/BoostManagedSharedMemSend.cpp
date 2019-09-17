#include "BoostManagedSharedMemSend.hpp"

#include <iostream>
#include <cstring>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>

using namespace std;
using namespace boost::interprocess;

BoostManagedSharedMemSend::BoostManagedSharedMemSend() : shm(nullptr)
{
}

BoostManagedSharedMemSend::~BoostManagedSharedMemSend()
{
	delete shm;
	shm = nullptr;
}

bool BoostManagedSharedMemSend::create()
{
	bool ret = false;

	try {
		shm = new managed_shared_memory(open_or_create, "MY_MANAGED_SHARED_MEMORY", 1024);

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
			cout << e.get_error_code() << "," << e.what() << endl;
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
		char array[1024] = "\0";
		std::memcpy(array, message.c_str(), message.length());

		// このように、construct_itを使うと配列そのものを渡すこともできる
		// constructなら配列でなくてもOK
		char* instance = shm->construct_it<char>
			("MyString")			// オブジェクトの固有名
			[message.length()]		// (配列の場合)要素数
			(&array[0]);			// 値

		ret = true;
	}
	catch (interprocess_exception& e) {
		cout << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostManagedSharedMemSend::receive()
{
	return string();
}

bool BoostManagedSharedMemSend::destroy()
{
	// managed_shared_memoryもshared_memory_object::remove()で消せる
	return shared_memory_object::remove("MY_MANAGED_SHARED_MEMORY");
}

bool BoostManagedSharedMemSend::hasNewMessage()
{
	return false;
}
