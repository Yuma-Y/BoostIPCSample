#include <iostream>
#include <string>
#include <Windows.h>

#include "BoostSharedMemReceive.hpp"
#include "BoostManagedSharedMemReceive.hpp"
#include "BoostMmapFileReceive.hpp"

using namespace std;

int main()
{
	IPC_IF* receiver = nullptr;
	// receiver = new BoostSharedMemReceive;
	receiver = new BoostManagedSharedMemReceive;
	// receiver = new BoostMmapFileReceive;

	cout << "=== Receiver Process ===" << endl;
	cout << "=== IPC class : " << receiver->getClassName() << " ===" << endl;

	// Sendから送られてきたメッセージを表示する
	while (true) {
		string message;
		if (receiver->hasNewMessage()) {
			message = receiver->receive();
			cout << "<--- Message from Sender: " << message << endl;

			if ((message == "q") || (message == "Q") || (message == "quit")) {
				break;
			}
		}

		Sleep(100);
	}

	receiver->destroy();
	delete receiver;
}

