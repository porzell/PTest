#include "../include/ptest.hpp"
#include <string>

P_TEST(MyFirstTest) {
    std::string testString;
    
    testString += "hello";
    testString += ' ';
    testString += "world!";
    
    P_ASSERT(testString == "hello world!");
    P_ASSERT(testString == "goodbye world!");
}

int main() {
    return PTest::PTestRegistry::Get().RunAllTests();
}