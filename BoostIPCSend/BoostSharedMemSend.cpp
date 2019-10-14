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

// ���L�������ɓǂݏ���������e
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
		shm.truncate(1024);	// ����������0KB�̂܂܁B������1KB�ȏ�̃T�C�Y�ɂ��Ȃ��ƁA���mapped_region��exception����������

		ret = true;
	}
	catch (interprocess_exception& e) {
		// already_exists_error(=���L�����������łɂ���)�������炢������폜���čēxcreate()����
		// (open_or_create�w�肵�Ă邩�炷�łɂ����Ă��悳�����Ȃ̂ɂȂ���exception�ɂȂ�̂�)
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
		// mapped_region��shared_memory_object�����v���Z�X�Ƀ}�b�s���O���A
		// ���̂悤�ɒ��ړǂݏ����ł���悤�ɂȂ�
		shared_memory_object shm(open_only, "MY_SHARED_MEMORY", read_write);
		mapped_region region(shm, read_write);
		void* addr = region.get_address();
		
		// ���������ƁAaddr�����(=���L����������)�I�u�W�F�N�g���쐬�ł���悤���B
		IPCData* data = new (addr) IPCData;

		// mutex�̏ꍇ�Aunlock()���Y����lock()�ȍ~�̃Z�N�V�����ɃA�N�Z�X�ł��Ȃ��Ȃ�
		// ���Ȃ݂ɁAscoped_lock���g����mutex�������o�ϐ��ɂ��Ȃ��Ă�
		// ���[�J���ϐ��̃X�R�[�v�̊Ԃ������b�N���A�X�R�[�v�𔲂���Ǝ����ŃA�����b�N���Ă����B
		// (scoped_lock<mutex�̌^>�@�ϐ���錾�������_�Ń��b�N�����j
		// interprocess��mutex�ɂ͒P����lock�ȊO�ɂ�
		// lock�̌��ʂ��킩��try_lock,
		// �ݒ肵�����Ԍo�߂Ń��b�N���������timed_lock������B
		scoped_lock<interprocess_mutex> lock(data->mutex);
		data->message = message;

		// �����ϐ���wait����B
		// ����ŁA���̃v���Z�X����notify�ŉ��������܂ł��̃v���Z�X�̎��s�͎~�܂�B
		// interprocess_consition��scoped_lock<interprocess_mutex>�^��n����wait����B
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
	// ���M��p�Ȃ̂ŋ�ł���
	return "";
}

bool BoostSharedMemSend::destroy()
{
	return shared_memory_object::remove("MY_SHARED_MEMORY");
}

bool BoostSharedMemSend::hasNewMessage()
{
	// ���M��p�Ȃ̂ŋ�ł���
	return false;
}

string BoostSharedMemSend::getClassName()
{
	return string("BoostSharedMemSend");
}
