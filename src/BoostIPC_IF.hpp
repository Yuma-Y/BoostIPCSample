/*
 * BoostIPC_IF.hpp
 *
 *  Created on: 2021/04/14
 *      Author: yuma
 */

#ifndef BOOSTIPC_IF_HPP_
#define BOOSTIPC_IF_HPP_

#include "IPC_IF.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#define BOOST_SHARED_MEMORY_NAME "BoostIPC_shm"

enum IPCType {
	CLIENT,
	SERVER
};

using namespace boost::interprocess;

class BoostIPC_IF {
	friend class IPCTest1;
	friend class IPCTest2;

	class IPCFlag {
		friend class BoostIPC_IF;
	public:
		IPCFlag() : top(0xFF), new_data(false){}
		~IPCFlag(){}

	private:
		unsigned char top;
		interprocess_mutex mutex;
		interprocess_condition write_cond;
		interprocess_condition read_cond;
		bool new_data;
	};

	template <class T>
	class IPCData {
		friend class BoostIPC_IF;
		public:
			IPCData() : data_length(0), data(0){}
			~IPCData(){}

		private:
			size_t data_length;
			T data;
	};

public:
	BoostIPC_IF() : shm(nullptr) {}

	~BoostIPC_IF(){}

	bool init(IPCType type){
		bool ret = false;

		try {
			shm = new shared_memory_object(open_or_create, BOOST_SHARED_MEMORY_NAME, read_write);
			shm->truncate(1024);
			mapped_region region(*shm, read_write);
			if (type == SERVER) {
				memset(region.get_address(), 0, 1024);
				IPCFlag *flag = new (region.get_address()) IPCFlag;
			}
			ret = true;
		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

	void Term(){
		shared_memory_object::remove(BOOST_SHARED_MEMORY_NAME);
	}

	template <class T>
	bool read(T* arg) {
		bool ret = false;

		try {
			mapped_region region(*shm, read_write);
			IPCFlag *ipc_flag = static_cast<IPCFlag *>(region.get_address());
			scoped_lock<interprocess_mutex> lock(ipc_flag->mutex);
			if (!ipc_flag->new_data) {
				// もし書き込まれていなければ待つ
				ipc_flag->write_cond.wait(lock);
			}

			void *base_addr = region.get_address() + sizeof(IPCFlag);
			IPCData<T> *ipc_data = static_cast<IPCData<T> *>(base_addr);
			memcpy(arg, &ipc_data->data, ipc_data->data_length);

			ipc_flag->new_data = false;
			ipc_flag->read_cond.notify_all();

			ret = true;
		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

	// std::string型だけ別処理にする
	template <typename T>
	bool read(std::string* arg){
		bool ret = false;

		try {
			mapped_region region(*shm, read_write);
			IPCFlag *ipc_flag = static_cast<IPCFlag *>(region.get_address());
			scoped_lock<interprocess_mutex> lock(ipc_flag->mutex);
			if (!ipc_flag->new_data) {
				// もし書き込まれていなければ待つ
				ipc_flag->write_cond.wait(lock);
			}

			char buf[800] = "\0";
			void *base_addr = region.get_address() + sizeof(IPCFlag);
			IPCData<char> *ipc_data = static_cast<IPCData<char> *>(base_addr);
			strncpy(buf, &ipc_data->data, ipc_data->data_length);
			arg->clear();
			arg->append(buf);

			ipc_flag->new_data = false;
			ipc_flag->read_cond.notify_all();

			ret = true;
		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

	template <class T>
	bool write(T arg){
		bool ret = false;

		try {
			mapped_region region(*shm, read_write);
			IPCFlag *ipc_flag = static_cast<IPCFlag *>(region.get_address());
			scoped_lock<interprocess_mutex> lock(ipc_flag->mutex);
			if (ipc_flag->new_data) {
				// もしまだ読まれていなければ待つ
				ipc_flag->read_cond.wait(lock);
			}

			IPCFlag *base_addr = ipc_flag + 1;
			IPCData<T> *ipc_data = new (base_addr) IPCData<T>;
			ipc_data->data = arg;
			ipc_data->data_length = sizeof(arg);

			ipc_flag->new_data = true;

			ipc_flag->write_cond.notify_all();
			ipc_flag->read_cond.wait(lock);

			ret = true;
		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

	// std::string型だけ別処理にする
	template <typename T>
	bool write(std::string arg){
		bool ret = false;

		// さすがに文字数制限をかける
		if (arg.length() > 800) {
			std::cerr << "error : string length too long" << std::endl;
		} else {
			try {
				mapped_region region(*shm, read_write);
				IPCFlag *ipc_flag = static_cast<IPCFlag *>(region.get_address());
				scoped_lock<interprocess_mutex> lock(ipc_flag->mutex);

				if (ipc_flag->new_data) {
					// もしまだ読まれていなければ待つ
					ipc_flag->read_cond.wait(lock);
				}

				IPCFlag *base_addr = ipc_flag + 1;
				IPCData<char> *ipc_data = new (base_addr) IPCData<char>;
				strncpy(&ipc_data->data, arg.c_str(), arg.length());
				ipc_data->data_length = arg.length();

				ipc_flag->new_data = true;

				ipc_flag->write_cond.notify_all();
				ipc_flag->read_cond.wait(lock);

				ret = true;
			} catch(interprocess_exception& e) {
				std::cerr << "line:" << __LINE__ << " : error_code = " <<
						e.get_error_code() << " reason : " << e.what() << std::endl;
			}
		}

		return ret;
	}

private:
	shared_memory_object *shm;
};

#endif /* BOOSTIPC_IF_HPP_ */
