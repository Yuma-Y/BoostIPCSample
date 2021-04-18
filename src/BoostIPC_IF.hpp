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
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include <boost/interprocess/ipc/message_queue.hpp>

#define BOOST_SHARED_MEMORY_NAME "BoostIPC_shm"

enum IPCType {
	CLIENT,
	SERVER
};

using namespace boost::interprocess;

template <class T>
class BoostIPC_IF : public IPC_IF<T>{
	friend class IPCTest;

	class IPCData {
	public:
		interprocess_mutex mutex;
		interprocess_condition write_cond;
		interprocess_condition read_cond;
		bool new_data;

		T data;
	};

public:
	BoostIPC_IF(){}

	~BoostIPC_IF(){}

	bool init(IPCType type){
		bool ret = false;

		try {
			if (type == SERVER) {
				shm = new shared_memory_object(open_or_create, BOOST_SHARED_MEMORY_NAME, read_write);
				shm->truncate(1024);

				mapped_region region(*shm, read_write);
				void *addr = region.get_address();
				IPCData *ipc_data = new (addr) IPCData;
				ipc_data->new_data = false;
			} else {
				shm = new shared_memory_object(open_only, BOOST_SHARED_MEMORY_NAME, read_write);
			}

			ret = true;
		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

	void Term(){
		delete shm;
		shared_memory_object::remove(BOOST_SHARED_MEMORY_NAME);
	}

	bool read(T* arg) {
		bool ret = false;

		try {
			mapped_region region(*shm, read_write);
			void *addr = region.get_address();
			IPCData *ipc_data = static_cast<IPCData*>(addr);

			scoped_lock<interprocess_mutex> lock(ipc_data->mutex);
			if (!ipc_data->new_data) {
				// もし書き込み前にデータが読み込まれていなければ待つ
				ipc_data->write_cond.wait(lock);
			}

			*arg = ipc_data->data;

			ipc_data->new_data = false;
			ipc_data->read_cond.notify_all();

			ret = true;
		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

	bool write(T arg){
		bool ret = false;

		try {
			mapped_region region(*shm, read_write);
			void *addr = region.get_address();
			IPCData *ipc_data = static_cast<IPCData*>(addr);
			// 書き込みの際は書いたら通知
			scoped_lock<interprocess_mutex> lock(ipc_data->mutex);
			if (ipc_data->new_data) {
				// もし書き込み前にデータが読み込まれていなければ待つ
				ipc_data->read_cond.wait(lock);
			}

			ipc_data->data = arg;
			ipc_data->new_data = true;

			ipc_data->write_cond.notify_all();

			ret = true;

		} catch(interprocess_exception& e) {
			std::cerr << "line:" << __LINE__ << " : error_code = " <<
					e.get_error_code() << " reason : " << e.what() << std::endl;
		}

		return ret;
	}

private:
	shared_memory_object *shm;
};

#endif /* BOOSTIPC_IF_HPP_ */
