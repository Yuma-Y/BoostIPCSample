#pragma once

#include "IPC_IF.hpp"

class BoostMessageQueueReceive : public IPC_IF
{
public:
	BoostMessageQueueReceive();
	~BoostMessageQueueReceive();

	bool create();
	bool send(string message);
	string receive();
	bool destroy();

	bool hasNewMessage();

	string getClassName();
};

