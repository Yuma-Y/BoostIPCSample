#include "BoostManagedSharedMemSend.hpp"

#include <iostream>
#include <cstring>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

using namespace std;
using namespace boost::interprocess;

BoostManagedSharedMemSend::BoostManagedSharedMemSend() : shm(nullptr), mutex(nullptr)
{
	mutex = new named_mutex(open_or_create, "named_mutex");
}

BoostManagedSharedMemSend::~BoostManagedSharedMemSend()
{
	delete shm;
	shm = nullptr;
	delete mutex;
	mutex = nullptr;
}

bool BoostManagedSharedMemSend::create()
{
	bool ret = false;

	try {
		shm = new managed_shared_memory(open_or_create, "MY_MANAGED_SHARED_MEMORY", 1024);

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

bool BoostManagedSharedMemSend::send(string message)
{
	bool ret = false;

	// managed_shared_segment�͂��̂悤�Ɍ^�Ɩ��O���w�肵�ă�������ǂݏ����ł���
	// ���Ȃ݂ɁA�������O�̃I�u�W�F�N�g�𕡐������������Ƃ����
	// interprocess_exception��9(=already_exists_error)�ƂȂ邪
	// receive���œǂ܂��Ɠ����ɏ������z��Ȃ̂Ń`�F�b�N�͂��Ȃ�

	try {
		mutex->lock();

		// string�^�̂܂܏����������Ƃ���ƂȂ������܂������Ȃ��̂�
		// char�^�z��ɕϊ����ēǂݏ�������
		// �f�X�g���N�^�̒��ł��낢����悤�ȕ��G�ȃN���X�̃I�u�W�F�N�g��
		// ����Ȃ��ق����悳�����B
		char array[1024] = "\0";
		std::memcpy(array, message.c_str(), message.length());

		// ���̂悤�ɁAconstruct_it���g���Ɣz�񂻂̂��̂�n�����Ƃ��ł���
		// construct�Ȃ�z��łȂ��Ă�OK
		char* instance = shm->construct_it<char>
			("MyString")			// �I�u�W�F�N�g�̌ŗL��
			[message.length()]		// (�z��̏ꍇ)�v�f��
			(&array[0]);			// �l

			mutex->unlock();

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
	named_mutex::remove("named_mutex");

	// managed_shared_memory��shared_memory_object::remove()�ŏ�����
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
