#pragma once

#include <string>
using namespace std;
// boostの各通信メカニズムを試すクラスは以下のクラスを継承し、
// それに合わせて実装、メインから呼び出すこととする。
class IPC_IF {
public:
	IPC_IF() {}
	virtual ~IPC_IF() {}

	virtual bool create() = 0;	// 準備
	virtual bool send(string message) = 0;	// 送信
	virtual string receive() = 0;	// 受信
	virtual bool destroy() = 0;	// 破棄

	virtual bool hasNewMessage() = 0; // メッセージあり？
};