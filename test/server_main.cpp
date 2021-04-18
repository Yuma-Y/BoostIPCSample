/*
 * server_main.cpp
 *
 *  Created on: 2021/04/14
 *      Author: yuma
 */

#include "BoostIPC_IF.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
	BoostIPC_IF<int> ipc_if;
	if (ipc_if.init(SERVER)) {
		std::cout << "=== IPC test server ===" << std::endl;

		// サーバーは読んで書くだけ
		int p;
		if(ipc_if.read(&p)){
			ipc_if.write(2000);
		}

		std::cout << "=== IPC test server END ===" << std::endl;
	}

	return 0;
}
