#include "BoostMmapFileSend.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace std;
using namespace boost::interprocess;

static const std::string argv_path(__argv[0]);
static const std::string mapped_file_path = argv_path.substr(0, argv_path.find_last_of("\\")) + "\\MMAP_FILE";

static const int file_size = 10000;

BoostMmapFileSend::BoostMmapFileSend() : file_map(nullptr)
{
}

BoostMmapFileSend::~BoostMmapFileSend()
{
	delete file_map;
	file_map = nullptr;
}

bool BoostMmapFileSend::create()
{
	// file_mapping���쐬����B
	bool ret = false;

	try {
		// �}�b�s���O�p�̃t�@�C�����쐬����B
		std::filebuf fbuf;
		fbuf.open(mapped_file_path, std::ios_base::in | std::ios_base::out
			| std::ios_base::trunc | std::ios_base::binary);
		// �t�@�C���T�C�Y�̐ݒ�B
		fbuf.pubseekoff(file_size - 1, std::ios_base::beg);
		fbuf.sputc(0);

		// �K����Ƀ}�b�s���O����t�@�C������� or ���݂��Ă��邱�Ƃ��m�F���Ă���}�b�s���O���邱�ƁB
		// �t�@�C�������݂��Ȃ��ꍇ�Ainterprocess_exception��not_found_error(=7)�ƂȂ�B
		file_map = new file_mapping(mapped_file_path.c_str(), read_write);

		ret = true;
	}
	catch (interprocess_exception& e) {
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

bool BoostMmapFileSend::send(string message)
{
	bool ret = false;

	try {
		// �t�@�C���}�b�s���O������A����������mapped_region�ɓn�����ƂŃA�h���X���擾���A�ǂݏ����ł���悤�ɂȂ�B
		// ���Ȃ݂�mapped_region���̂͋��L�������Ɠ��l�Ɏg����B
		// �܂��f�t�H���g���ƃt�@�C���̐擪����̈悷�ׂĂ��v���Z�X�Ƀ}�b�s���O���邪�A
		// �C���X�^���X�쐬���Ɉ�����n�����ƂŁA�t�@�C���̐擪�ȊO�̗̈悩��}�b�s���O���邱�Ƃ��A
		// �t�@�C���̗̈�̈ꕔ�����v���Z�X�Ƀ}�b�s���O���邱�Ƃ��\�B
		// (���ɋ���ȃt�@�C�����g���ꍇ�A�v���Z�X��Ԉȏ�̃T�C�Y�̃t�@�C���̓}�b�s���O�ł��Ȃ��̂ŕK�v�ɂȂ��Ă���B)

		// ����mapped_region���|�C���^�Ń����o�ϐ��ɂ���ƂȂ���interprocess_exception��system_error(=1)�ƂȂ�̂�
		// �s�x�}�b�s���O����悤�ɂ���B
		mapped_region region(*file_map, read_write, 0, message.length() + 1);

		void* addr = region.get_address();
		std::size_t size = region.get_size();

		if (size > message.length()) {
			std::memcpy(addr, message.c_str(), message.length());
		}

		// mapped_region�ɏ������񂾂������ƃv���Z�X�Ƀ}�b�s���O�����������ɏ����������Ȃ̂ŁA
		// �����flush()���ăt�@�C���ɏ������ޕK�v������B�i�����炭���L���������{���͂��ׂ��j
		region.flush();

		ret = true;
	}
	catch (interprocess_exception& e) {
		cout << __FILE__ << "(" << __LINE__ << "):" << e.get_error_code() << "," << e.what() << endl;
	}

	return ret;
}

string BoostMmapFileSend::receive()
{
	return string();
}

bool BoostMmapFileSend::destroy()
{
	// ���L���������l�A�t�@�C���}�b�s���O��remove()�ŏ���
	return file_mapping::remove(mapped_file_path.c_str());;
}

bool BoostMmapFileSend::hasNewMessage()
{
	return false;
}

string BoostMmapFileSend::getClassName()
{
	return string("BoostMmapFileSend");
}
