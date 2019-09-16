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

using namespace std;
using namespace boost::interprocess;

BoostSharedMemSend::BoostSharedMemSend() : shm(nullptr)
{
}

BoostSharedMemSend::~BoostSharedMemSend()
{
	delete shm;
	shm = nullptr;
}

bool BoostSharedMemSend::create()
{
	bool ret = false;

	try {
		shm = new shared_memory_object(open_or_create, "MY_SHARED_MEMORY", read_write);
		shm->truncate(1024);	// 作った直後は0KBのまま。ここで1KB以上のサイズにしないと、後でmapped_regionでexceptionが発生する

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

bool BoostSharedMemSend::send(string message)
{
	bool ret = false;

	try {
		// mapped_regionでshared_memory_objectを自プロセスにマッピングし、
		// このように直接読み書きできるようになる
		mapped_region region(*shm, read_write);
		char* mem = static_cast<char*>(region.get_address());
		for (std::size_t i = 0; i < message.length(); ++i, ++mem) {
			*mem = message[i];
		}
		
		ret = true;
	}
	catch (interprocess_exception& e) {
		cout << e.get_error_code() << "," << e.what() << endl;
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
