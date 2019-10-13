#include <iostream>
#include <string>

#include "BoostSharedMemSend.hpp"
#include "BoostManagedSharedMemSend.hpp"
#include "BoostMmapFileSend.hpp"

using namespace std;

int main()
{
	IPC_IF* sender = nullptr;
	// sender = new BoostSharedMemSend;
	// sender = new BoostManagedSharedMemSend;
	sender = new BoostMmapFileSend;

	// 共有メモリなど、送信機構の作成は送信側が行うこととする
	if (sender->create()) {
		cout << "=== Sender Process ===" << endl;
		cout << "=== IPC class : " << sender->getClassName() << " ===" << endl;

		// 標準入力から入った文言を送る。q/Q/quitで終了。
		string message;
		while(true) {
			cout << "Enter message to send :";
			// cinはスペースを挟まれるとそこで区切ってしまう。
			// スペースが来ても区切らず、Enterされるまでの１行を取りたい場合getline(cin, 入力先)を使う。
			getline(cin, message);

			if (sender->send(message)) {
				cout << "---> send success" << endl;
			}
			else {
				cout << "!!! send error" << endl;
			}

			if ((message == "q") || (message == "Q") || (message == "quit")) {
				break;
			}
		}
	}

	sender->destroy();
	delete sender;
}

