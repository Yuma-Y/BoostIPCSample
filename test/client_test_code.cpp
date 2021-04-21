/*
 * client_main.cpp
 *
 *  Created on: 2021/04/14
 *      Author: yuma
 */

#include <gtest/gtest.h>
#include <string>
#include "BoostIPC_IF.hpp"

class IPCTest1 : public ::testing::Test {
protected:
	static void SetUpTestSuite() {
		ipc_if = new BoostIPC_IF;
	}

	static void TearDownTestSuite() {
		ipc_if->Term();
		delete ipc_if;
	}

	static BoostIPC_IF *ipc_if;
};

BoostIPC_IF* IPCTest1::ipc_if = NULL;

TEST_F(IPCTest1, test_case_01){
	bool ret = false;
	ret = ipc_if->init(CLIENT);
	EXPECT_TRUE(ret);
}

TEST_F(IPCTest1, test_case_02){
	bool ret = false;
	ret = ipc_if->write<int>(1000);
	EXPECT_TRUE(ret);
}

TEST_F(IPCTest1, test_case_03){
	bool ret = false;
	int p;
	ret = ipc_if->read<int>(&p);
	EXPECT_TRUE(ret);
	printf("response from server  : %d\n", p);

	EXPECT_EQ(p, 2000);
}

TEST_F(IPCTest1, test_case_04){
	bool ret = false;
	ret = ipc_if->write<std::string>("send");
	EXPECT_TRUE(ret);
}

TEST_F(IPCTest1, test_case_05){
	bool ret = false;

	std::string str;
	ret = ipc_if->read<std::string>(&str);
	EXPECT_TRUE(ret);
	printf("response from server  : %s\n", str.c_str());

	EXPECT_EQ(str, "received");
}
