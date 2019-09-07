
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

using namespace std;
using namespace boost::interprocess;

BoostSharedMemReceive::BoostSharedMemReceive() : shm(nullptr)
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
		char* mem = static_cast<char*>(region.get_address());
		for (std::size_t i = 0; i < region.get_size(); ++i, ++mem) {
			if (*mem != '\0') {
				ret.push_back(*mem);
			}
		}

		// 読んだ後メモリの内容をクリアする
		std::memset(region.get_address(), 0, region.get_size());
	}
	catch (interprocess_exception& e) {
		cout << e.get_error_code() << "," << e.what() << endl;
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
			char* mem = static_cast<char*>(region.get_address());
			if (*mem != '\0') {
				ret = true;
			}
		}
		catch (interprocess_exception& e) {
			cout << e.get_error_code() << "," << e.what() << endl;
		}
	}

	return ret;
}
