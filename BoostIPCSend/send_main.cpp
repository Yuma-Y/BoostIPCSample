#include <iostream>
#include <string>

#include "BoostSharedMemSend.hpp"
#include "BoostManagedSharedMemSend.hpp"

using namespace std;

int main()
{
	IPC_IF* sender = nullptr;
	sender = new BoostSharedMemSend;
	// sender = new BoostManagedSharedMemSend;

	const type_info& id = typeid(sender);

	// 共有メモリなど、送信機構の作成は送信側が行うこととする
	if (sender->create()) {
		cout << "=== Sender Process ===" << endl;

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

			// @TODO : send側が終了したらreceive側も終了するようにしたいが、
			// 現状send側がquitを書き込むとその時点で終了して共有メモリを削除してしまうためreceive側がそれを読めない。
			// send側が書き込む→receive側が読む→receive側終了→send側終了となるようにする。
		}
	}

	sender->destroy();
	delete sender;
}

