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
		// string�^�̂܂܏����������Ƃ���ƂȂ������܂������Ȃ��̂�
		// char�^�z��ɕϊ����ēǂݏ�������
		// �f�X�g���N�^�̒��ł��낢����悤�ȕ��G�ȃN���X�̃I�u�W�F�N�g��
		// ����Ȃ��ق����悳�����B
		/* char array[1024] = "\0";
		std::memcpy(array, message.c_str(), message.length()); */

		// ���̂悤�ɁAconstruct_it���g���Ɣz�񂻂̂��̂�n�����Ƃ��ł���
		// construct�Ȃ�z��łȂ��Ă�OK
		/* char* instance = shm->construct_it<char>
			("MyString")			// �I�u�W�F�N�g�̌ŗL��
			[message.length()]		// (�z��̏ꍇ)�v�f��
			(&array[0]);			// �l
		*/

		// managed_shared_memory.find_or_construct()�łP�̃I�u�W�F�N�g���g���܂킷���Ƃ��ł���B
		managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
		IPCData* data = shm.find_or_construct<IPCData>("MyData")();

		// �Z�}�t�H��mutex���l�A����Z�N�V�����ւ̃A�N�Z�X�𐧌䂷��r������Ɏg���B
		// wait()�ŃZ�}�t�H�����b�N����B
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

	// ���L���������폜����O�ɁAwait()����B
	// receive����post()�����Έȍ~�̏����֐i�߂�B
	// ����ŁAreceive����quit���b�Z�[�W��ǂޑO��send�������L���������폜���Ă��܂��̂�h����B
	managed_shared_memory shm(open_only, "MY_MANAGED_SHARED_MEMORY");
	typedef pair<IPCData*, managed_shared_memory::size_type> MyType;
	MyType res = shm.find<IPCData>("MyData");
	IPCData* data = res.first;
	data->read_semaphore.wait();

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
