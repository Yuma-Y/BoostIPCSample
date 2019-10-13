#include "BoostMmapFileReceive.hpp"

#include <iostream>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace std;
using namespace boost::interprocess;

static const std::string argv_path(__argv[0]);
static const std::string mapped_file_path = argv_path.substr(0, argv_path.find_last_of("\\")) + "\\MMAP_FILE";

static const int file_size = 10000;

BoostMmapFileReceive::BoostMmapFileReceive() : file_map(nullptr)
{
}

BoostMmapFileReceive::~BoostMmapFileReceive()
{
	delete file_map;
	file_map = nullptr;
}

bool BoostMmapFileReceive::create()
{
	return false;
}

bool BoostMmapFileReceive::send(string message)
{
	return false;
}

string BoostMmapFileReceive::receive()
{
	string ret = "";

	try {
		mapped_region region(*file_map, read_write);
		void* addr = region.get_address();

		char* ch = static_cast<char*>(addr);
		do {
			ret.push_back(*ch);
			ch++;
		} while (*ch != '\0');

		// 読み終わったら中身をクリアする
		std::size_t size = region.get_size();
		std::memset(addr, '\0', size);
	}
	catch (interprocess_exception & e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

bool BoostMmapFileReceive::destroy()
{
	return false;
}

bool BoostMmapFileReceive::hasNewMessage()
{
	bool ret = false;

	try {
		if (file_map == nullptr) {
			file_map = new file_mapping(mapped_file_path.c_str(), read_write);
		}

		mapped_region region(*file_map, read_only);
		void* addr = region.get_address();
		char* ch = static_cast<char*>(addr);
		
		if (*ch != '\0') {
			ret = true;
		}
	}
	catch (interprocess_exception & e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostMmapFileReceive::getClassName()
{
	return string("BoostMmapFileReceive");
}
