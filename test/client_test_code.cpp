/*
 * client_main.cpp
 *
 *  Created on: 2021/04/14
 *      Author: yuma
 */

#include <gtest/gtest.h>
#include <string>
#include "BoostIPC_IF.hpp"

class IPCTest : public ::testing::Test {
protected:
	static void SetUpTestSuite() {
		ipc_if = new BoostIPC_IF<int>;
	}

	static void TearDownTestSuite() {
		ipc_if->Term();
		delete ipc_if;
	}

	static BoostIPC_IF<int> *ipc_if;
};

BoostIPC_IF<int>* IPCTest::ipc_if = NULL;

TEST_F(IPCTest, test_case_01){
	bool ret = false;
	ret = ipc_if->init(CLIENT);
	EXPECT_TRUE(ret);
}

TEST_F(IPCTest, test_case_02){
	bool ret = false;
	ret = ipc_if->write(1000);
	EXPECT_TRUE(ret);
}

TEST_F(IPCTest, test_case_03){
	bool ret = false;
	int p;
	ret = ipc_if->read(&p);
	EXPECT_TRUE(ret);
	printf("response from server  : %d\n", p);

	EXPECT_EQ(p, 2000);
}

