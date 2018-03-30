#define USE_PTEST_MAIN
#include "../include/ptest.hpp"
#include <iostream>

enum class Status {
    Failure = 0,
    Success,
    Busy
};

enum class StatusNonVerbose {
    Failure = 0,
    Success,
    Busy
};

class MyTestFixture : public PTest::PTestFixture {
    
    protected:
        Status status_verbose;
        StatusNonVerbose status;
        
        class TestClass {
            private:
                int internal_;
            public:
                TestClass() {}
                TestClass(int value) : internal_(value) {};
                void SetValue(int value) { internal_ = value; }
                bool operator == (const TestClass &rhs) {
                    return this->internal_ == rhs.internal_;
                }
        };
        
        TestClass a, b;
        
        int myInt;
        float myFloat;
    
    public:
        MyTestFixture(){};
        void Create() {
            status_verbose = Status::Busy;
            status = StatusNonVerbose::Busy;
            a.SetValue(5);
            b.SetValue(7);
            
            myInt = 4;
            myFloat = 4.0f;
        }
        void Destroy() {
            
        }
};

// Allow ptest to be a bit more verbose in logs.
std::ostream& operator << (std::ostream& stream, const Status status) {
    static const char* values[] = {
      "Failure",
      "Success",
      "Busy"
    };
    auto index = static_cast<typename std::underlying_type<decltype(status)>::type>(status);
    
    if(index >= sizeof(values) / sizeof(const char*)) {
        stream << "Bad Status!";
        return stream;
    }
    
    stream << values[index];
    return stream;
}

P_TEST_F(MyTestFixture, TestEnumClassDisplay) {
    Status status = Status::Busy;
    
    // This should fail, but a nice message will be displayed, as PTest now knows
    // how to print Status enum class instances.
    P_EXPECT_EQ(status, Status::Success);
}

P_TEST_F(MyTestFixture, TestEnumClassDisplayNonVerbose) {
    StatusNonVerbose status = StatusNonVerbose::Busy;
    
    // Even if ostream is not overloaded for an enum class,
    // PTest will default to printing the raw value inside each instance.
    P_EXPECT_EQ(status, StatusNonVerbose::Success);
}

P_TEST_F(MyTestFixture, DoNotCrashOnClassWithNoOstream) {
    // If PTest has no known way to display value of a class instance,
    // it will simply display, "(unprintable)".
    P_EXPECT_EQ(a, b);
}

P_TEST_F(MyTestFixture, CompareDifferentTypes) {
    // PTest is capable of gracefully comparing
    // different types.
    P_EXPECT_EQ(myFloat, myInt);
    P_EXPECT_GE(myFloat, myInt);
}