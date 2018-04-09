#include <gtest/gtest.h>
#include "common.h"

#ifdef _MSC_VER
#pragma comment(lib, "gtest.lib")
#endif

vector<int>* g_data;

int main(int argc, char** argv)
{
	testing::AddGlobalTestEnvironment (new bench_Environment);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS ();
}
