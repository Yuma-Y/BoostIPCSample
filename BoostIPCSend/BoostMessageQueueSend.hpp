#pragma once

#include "IPC_IF.hpp"

class BoostMessageQueueSend : public IPC_IF
{
public:
	BoostMessageQueueSend();
	~BoostMessageQueueSend();

	bool create();
	bool send(string message);
	string receive();
	bool destroy();

	bool hasNewMessage();

	string getClassName();
};

