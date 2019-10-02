#include <iostream>
#include <string>
#include <Windows.h>

#include "BoostSharedMemReceive.hpp"
#include "BoostManagedSharedMemReceive.hpp"

using namespace std;

int main()
{
	IPC_IF* receiver = nullptr;
	// receiver = new BoostSharedMemReceive;
	receiver = new BoostManagedSharedMemReceive;

	cout << "=== Receiver Process ===" << endl;
	cout << "=== IPC class : " << receiver->getClassName() << " ===" << endl;

	// Sendから送られてきたメッセージを表示する
	while (true) {
		string message;
		if (receiver->hasNewMessage()) {
			message = receiver->receive();
			cout << "<--- Message from Sender: " << message << endl;
		}

		Sleep(100);
	}

	receiver->destroy();
	delete receiver;
}

