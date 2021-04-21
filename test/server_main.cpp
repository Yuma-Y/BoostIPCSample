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
	BoostIPC_IF ipc_if;
	if (ipc_if.init(SERVER)) {
		std::cout << "=== IPC test server ===" << std::endl;

		// サーバーは読んで書くだけ
		int p;
		if(ipc_if.read<int>(&p)){
			ipc_if.write<int>(2000);
		}

		std::string str;
		if(ipc_if.read<std::string>(&str)){
			ipc_if.write<std::string>("received");
		}

		std::cout << "=== IPC test server END ===" << std::endl;
	}

	return 0;
}
