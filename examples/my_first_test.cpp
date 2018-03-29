#include "../include/ptest.hpp"
#include <string>

P_TEST(MyFirstTest) {
    std::string testString;
    
    testString += "hello";
    testString += ' ';
    testString += "world!";
    
    P_ASSERT(testString == "hello world!");
    P_EXPECT(testString == "goodbye world!");
    P_ASSERT(0 == 1);
}

int main() {
    return PTest::PTestRegistry::Get().RunAllTests();
}