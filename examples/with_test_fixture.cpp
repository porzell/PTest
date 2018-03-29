#define USE_PTEST_MAIN
#include "../include/ptest.hpp"
#include <cstdio>
#include <string.h>

class MyTestFixture : public PTest::PTestFixture {
    protected:
        int myVar = 0;
    public:
        void Create() {
            myVar = 5;
            printf("[MyTestFixture.Create]: I'm in setup, where myVar=%i...\n", myVar);
        }
        void Destroy() {
            P_EXPECT(myVar == 5);
            printf("[MyTestFixture.Destroy]: I'm in cleanup, where myVar=%i...\n", myVar);
        }
};


P_TEST_F(MyTestFixture, SucceedTest) {
    P_ASSERT(myVar == 5);
    // This change will be seen in MyTestFixture::Destroy().
    myVar = 7;
}

// Regular tests can be used alongside test fixture tests.
P_TEST(RegularTest) {
    static const char* howdy = "Howdy!  I'm a regular test!\n";
    P_ASSERT(printf("%s", howdy) == strlen(howdy));
}

P_TEST_F(MyTestFixture, FailTest) {
    P_ASSERT(myVar == 3);
    
    // Since the assert above fails, this value will not be set.
    myVar = 13;
}