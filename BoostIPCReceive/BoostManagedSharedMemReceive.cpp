#include "BoostManagedSharedMemReceive.hpp"

#ifndef BOOST_INTERPROCESS_SHARED_DIR_PATH
static const std::string argv_path(__argv[0]);
static const std::string shared_dir_path = argv_path.substr(0, argv_path.find_last_of("\\"));

#define BOOST_INTERPROCESS_SHARED_DIR_PATH shared_dir_path
#endif

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

using namespace std;
using namespace boost::interprocess;

BoostManagedSharedMemReceive::BoostManagedSharedMemReceive() : mutex(nullptr)
{
	mutex = new named_mutex(open_or_create, "named_mutex");
}

BoostManagedSharedMemReceive::~BoostManagedSharedMemReceive()
{
	delete mutex;
	mutex = nullptr;
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
		// interprocess��mutex�ɂ͒P����lock�ȊO�ɂ�
		// lock�̌��ʂ��킩��try_lock,
		// �ݒ肵�����Ԍo�߂Ń��b�N���������timed_lock������B
		mutex->lock();

		managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");

		typedef pair<char*, managed_shared_memory::size_type> MyType;
		MyType res = shm.find<char>("MyString");
		if (res.second != 0) {
			for (int i = 0; i < res.second; i++) {
				ret.push_back(res.first[i]);
			}

			// �ǂ񂾌チ�����̓��e���N���A����
			shm.destroy<char>("MyString");
		}

		mutex->unlock();
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

	// �܂����L�������t�@�C�������邩�ǂ������m���߂�
	const boost::filesystem::path path(BOOST_INTERPROCESS_SHARED_DIR_PATH + "\\MY_MANAGED_SHARED_MEMORY");

	boost::system::error_code error;
	const bool result = boost::filesystem::exists(path, error);
	if (result && !error) {
		try {
			mutex->lock();

			managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
			// managed_shared_memory�̏ꍇ�A���̂悤��
			// find<��肽���^>(�I�u�W�F�N�g��)���ĂԂ��Ƃ�
			// std::pair<��肽���^�̃|�C���^, managed_shared_memory::size_type>���Ԃ��Ă���B
			// 2�v�f�ڂ�size_type�ɂ͏������܂ꂽ�v�f��������B
			// construct�ŏ������ރI�u�W�F�N�g�ɂ͔z����n����̂ŁA�v�f���̃`�F�b�N���ł���B
			pair<char*, managed_shared_memory::size_type> res = shm.find<char>("MyString");

			if (res.second != 0) {
				ret = true;
			}

			mutex->unlock();
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
