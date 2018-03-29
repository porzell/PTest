#define USE_PTEST_MAIN
#include "../include/ptest.hpp"
#include <string>

P_TEST(MyFirstTest) {
	std::string testString;

	testString += "hello";
	testString += ' ';
	testString += "world!";

	P_ASSERT(testString == "hello world!");
	P_EXPECT(testString == "goodbye world!");
	
	P_ASSERT(2 + 2 != 4);
}