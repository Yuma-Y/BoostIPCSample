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
	// file_mappingを作成する。
	bool ret = false;

	try {
		// マッピング用のファイルを作成する。
		std::filebuf fbuf;
		fbuf.open(mapped_file_path, std::ios_base::in | std::ios_base::out
			| std::ios_base::trunc | std::ios_base::binary);
		// ファイルサイズの設定。
		fbuf.pubseekoff(file_size - 1, std::ios_base::beg);
		fbuf.sputc(0);

		// 必ず先にマッピングするファイルを作る or 存在していることを確認してからマッピングすること。
		// ファイルが存在しない場合、interprocess_exceptionのnot_found_error(=7)となる。
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
		// ファイルマッピングしたら、それをさらにmapped_regionに渡すことでアドレスを取得し、読み書きできるようになる。
		// ちなみにmapped_region自体は共有メモリと同様に使える。
		// またデフォルトだとファイルの先頭から領域すべてをプロセスにマッピングするが、
		// インスタンス作成時に引数を渡すことで、ファイルの先頭以外の領域からマッピングすることも、
		// ファイルの領域の一部だけプロセスにマッピングすることも可能。
		// (特に巨大なファイルを使う場合、プロセス空間以上のサイズのファイルはマッピングできないので必要になってくる。)

		// ただmapped_regionをポインタでメンバ変数にするとなぜかinterprocess_exceptionのsystem_error(=1)となるので
		// 都度マッピングするようにする。
		mapped_region region(*file_map, read_write, 0, message.length() + 1);

		void* addr = region.get_address();
		std::size_t size = region.get_size();

		if (size > message.length()) {
			std::memcpy(addr, message.c_str(), message.length());
		}

		// mapped_regionに書き込んだだけだとプロセスにマッピングしたメモリに書いただけなので、
		// これをflush()してファイルに書き込む必要がある。（おそらく共有メモリも本当はやるべき）
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
	// 共有メモリ同様、ファイルマッピングもremove()で消す
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
