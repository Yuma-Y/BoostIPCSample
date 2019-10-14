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

// ���L�������ɓǂݏ���������e
typedef struct _IPCData {
	boost::interprocess::interprocess_semaphore write_semaphore;
	boost::interprocess::interprocess_semaphore read_semaphore;
	string message;

public:
	// interprocess_semaphore�̓R���X�g���N�^������initial_count���w�肷��B
	// �Ȃ��Z�}�t�H��wait�ŃJ�E���^���f�N�������g�Apost�ŃC���N�������g���A
	// �E�����̃J�E���^��0�ȉ��̏�Ԃ̂Ƃ���wait()����ƁA���̃v���Z�X�̎��s���~�܂�B
	// �Epost()���ăJ�E���^��0�ȏ�ɂȂ�ƁA�~�܂��Ă����v���Z�X�̎��s���ĊJ�����B

	// initial_count��0���ƍŏ���write���鎞�_�Ŏ~�܂��Ă��܂��̂ŁA1�ɂ��Ă����B
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

			// �ǂ񂾌チ�����̓��e���N���A����
			shm.destroy<char>("MyString");
		}
		*/
		typedef pair<IPCData*, managed_shared_memory::size_type> MyType;
		MyType res = shm.find<IPCData>("MyData");

		IPCData* data = res.first;
		
		data->read_semaphore.post();
		ret = data->message;

		// �ǂ񂾌チ�����̓��e���N���A����
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

	// �܂����L�������t�@�C�������邩�ǂ������m���߂�
	const boost::filesystem::path path(BOOST_INTERPROCESS_SHARED_DIR_PATH + "\\MY_MANAGED_SHARED_MEMORY");

	boost::system::error_code error;
	const bool result = boost::filesystem::exists(path, error);
	if (result && !error) {
		try {
			managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
			// managed_shared_memory�̏ꍇ�A���̂悤��
			// find<��肽���^>(�I�u�W�F�N�g��)���ĂԂ��Ƃ�
			// std::pair<��肽���^�̃|�C���^, managed_shared_memory::size_type>���Ԃ��Ă���B
			// 2�v�f�ڂ�size_type�ɂ͏������܂ꂽ�v�f��������B
			// construct�ŏ������ރI�u�W�F�N�g�ɂ͔z����n����̂ŁA�v�f���̃`�F�b�N���ł���B
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
