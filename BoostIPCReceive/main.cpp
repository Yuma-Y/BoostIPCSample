#include <iostream>
#include <string>
#include <Windows.h>

#include "BoostSharedMemReceive.hpp"

using namespace std;

int main()
{
	IPC_IF* reciever = nullptr;
	reciever = new BoostSharedMemReceive;
	cout << "=== Reciver Process ===" << endl;

	// Sendから送られてきたメッセージを表示する
	while (true) {
		string message;
		if (reciever->hasNewMessage()) {
			message = reciever->receive();
			cout << "<--- Message from Sender: " << message << endl;
		}

		Sleep(100);
	}

	reciever->destroy();
	delete reciever;
}

