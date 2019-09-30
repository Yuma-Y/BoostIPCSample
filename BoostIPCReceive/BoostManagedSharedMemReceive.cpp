#include "BoostManagedSharedMemReceive.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

using namespace std;
using namespace boost::interprocess;

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

		typedef pair<char*, managed_shared_memory::size_type> MyType;
		MyType res = shm.find<char>("MyString");
		if (res.second != 0) {
			for (int i = 0; i < res.second; i++) {
				ret.push_back(res.first[i]);
			}

			// 読んだ後メモリの内容をクリアする
			shm.destroy<char>("MyString");
		}
	}
	catch (interprocess_exception& e) {
		cout << e.get_error_code() << "," << e.what() << endl;
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
			pair<char*, managed_shared_memory::size_type> res = shm.find<char>("MyString");

			if (res.second != 0) {
				ret = true;
			}
		}
		catch (interprocess_exception& e) {
			cout << e.get_error_code() << "," << e.what() << endl;
		}
	}

	return ret;
}

string BoostManagedSharedMemReceive::getClassName()
{
	return string("BoostManagedSharedMemReceive");
}
