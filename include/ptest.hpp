#pragma once
#ifndef _P_TEST_H_
#define _P_TEST_H_

#include <string>
#include <vector>
#include <unordered_map>

#define P_DEBUG_PRINT(STRING, ...) printf("%s (%u):" "STRING",__FILE__,__LINE__,__VA_ARGS__)

#define P_ASSERT(...) \
  if (!(__VA_ARGS__)) { \
    PTest::PTestRegistry::Get().GetCurrentTest().AddAssertion(PTest::PAssertion(false, __FILE__, __LINE__, #__VA_ARGS__)); \
    return; \
  } else { \
    PTest::PTestRegistry::Get().GetCurrentTest().AddAssertion(PTest::PAssertion(true, __FILE__, __LINE__, #__VA_ARGS__)); \
  }

#define STRINGIFY(x) #x

#define CAT_HELPER(x, y) x##y
#define CAT(val1, val2) CAT_HELPER(val1, val2)

#define MAKE_UNIQUE_TESTNAME(name) CAT(name, __LINE__)

#define P_TEST(TEST_NAME) \
    static void MAKE_UNIQUE_TESTNAME(TEST_NAME)(); \
    PTest::PTestRegistrar MAKE_UNIQUE_TESTNAME(TEST_NAME##reg)(STRINGIFY(TEST_NAME), MAKE_UNIQUE_TESTNAME(TEST_NAME)); \
    static void MAKE_UNIQUE_TESTNAME(TEST_NAME)()

namespace PTest {

  const char P_COLOR_NORMAL[] = "\x1B[0m";
  const char P_COLOR_GREEN[] = "\x1B[32m";
  const char P_COLOR_BLUE[] = "\x1b[36m";
  const char P_COLOR_RED[] = "\x1b[31m";

  /*class PTestRunner {
    private:

    public:
      static RunTest(std::string testName) {
        PTestRegistry::Get().FindTest(testName);
      }
  };*/

  typedef void (*P_TEST_FUNC)(void);

  void PrintBars(uint32_t num = 2) {
      for(int i = 0; i < num; ++i) {
          printf("======================");
      }
      putchar('\n');
  }

  class PAssertion {
    private:
      bool success_;
      std::string condition_;
      std::string file_;
      uint32_t line_;
    public:
      PAssertion(bool success, const char* file, uint32_t line, const char* condition) :
        success_(success), condition_(condition), file_(file), line_(line) {};

      ~PAssertion() = default;

      bool GetSuccess() const { return success_; }
      const std::string& GetCondition() const { return condition_; }
      const std::string& GetFile() const { return file_; }
      uint32_t GetLine() const { return line_; }

      explicit operator bool() const {
        return success_;
      }
  };

  class PTest {
    private:
      std::string name_;
      P_TEST_FUNC func_;
      bool run_, succeeded_;
      std::vector<PAssertion> assertions_;

    public:
      PTest(const char* testName, P_TEST_FUNC testFunc) :
        name_(testName), func_(testFunc),
        run_(false), succeeded_(false)
        {};

      ~PTest() = default;

      void AddAssertion(PAssertion assertion) {
        assertions_.push_back(assertion);
        if (!assertion) {
          printf("%s", P_COLOR_RED);
          PrintBars();
          printf("Assertion Failure in %s (Line %u)\n", assertion.GetFile().c_str(), assertion.GetLine());
          printf("     Condition not met:\n\t\x1B[0m(%s)\n\n", assertion.GetCondition().c_str());
        }
        printf("%s", P_COLOR_NORMAL);
      }

      bool HasSucceeded() const { return succeeded_; }
      bool HasRun() const { return run_; }

      bool Run() {
        printf("%s", P_COLOR_GREEN);
        printf("[Running] ");
        printf("%s", P_COLOR_NORMAL);
        printf("%s:\n", name_.c_str());

        run_ = false;
        succeeded_ = true;
        assertions_.clear();

        func_();

        size_t failedAssertionCount = 0;

        for (PAssertion& assertion : assertions_) {
          if (!assertion.GetSuccess()) {
            succeeded_ = false;
            failedAssertionCount++;
          }
        }

        printf("%s", P_COLOR_BLUE);
        printf(" Test \"%s\" finished.\n", name_.c_str());
        succeeded_ ? printf("%s", P_COLOR_GREEN) : printf("%s", P_COLOR_RED) ;
        printf("   %u assertions failed.\n", failedAssertionCount);
        printf("%s", P_COLOR_GREEN);
        printf("   %u assertions run.\n", assertions_.size());

        if (!succeeded_) {
          printf("%s", P_COLOR_RED);
          PrintBars();
        }
        printf("%s", P_COLOR_NORMAL);
        return succeeded_;
      }
  };

  class PTestRegistry {
    std::unordered_map<std::string, PTest> testMap_;

    PTest *currentTest_ = nullptr;

    public:
      static PTestRegistry& Get() {
        static PTestRegistry registry;
        return registry;
      }

      void AddTest(const char* testName, P_TEST_FUNC testFunc) {
        if (testMap_.find(testName) != testMap_.end()) {
          printf("%s", P_COLOR_RED);
          printf("[DUPLICATE TESTS FOUND for \"%s!\" ABORTING...]\n", testName);
          printf("%s", P_COLOR_NORMAL);
          return;
        }

        testMap_.insert({testName, PTest(testName, testFunc)});
      }

      bool RunTest(std::string testName) {
        auto it = testMap_.find(testName);
        if (it == testMap_.end()) {
          printf("%s", P_COLOR_RED);
          printf("[TEST \"%s\" NOT FOUND!]\n", testName.c_str());
          printf("%s", P_COLOR_NORMAL);
          return false;
        }

        currentTest_ = &it->second;

        return it->second.Run();
      }

      int RunAllTests() {
        size_t testsRunCount = 0;
        size_t testsFailedCount = 0;

        printf("%s", P_COLOR_GREEN);
        printf("\n[Running all tests]\n\n");
        printf("%s", P_COLOR_NORMAL);

        for (auto it = testMap_.begin(); it != testMap_.end(); it++) {
          auto& theTest = it->second;

          currentTest_ = &theTest;

          if (!theTest.Run()) {
            testsFailedCount++;
          }

          testsRunCount++;
        }

        putchar('\n');
        printf("%s", P_COLOR_NORMAL);
        PrintBars(3);
        printf("[Test Summary]:\n");
        printf("%s", P_COLOR_GREEN);
        printf("\tTests Passed:\t%s%u\n", P_COLOR_NORMAL, testsRunCount - testsFailedCount);
        testsFailedCount ? printf("%s", P_COLOR_RED) : printf("%s", P_COLOR_GREEN);
        printf("\tTests Failed:\t%s%u\n", P_COLOR_NORMAL, testsFailedCount);
        printf("%s", P_COLOR_BLUE);
        printf("\tTotal Tests:\t%s%u\n", P_COLOR_NORMAL, testsRunCount);
        printf("%s", P_COLOR_NORMAL);
        PrintBars(3);
        
        return testsFailedCount;
      }

      PTest& GetCurrentTest() { return *currentTest_;}

      inline size_t GetNumTests() { return testMap_.size(); }
  };

  struct PTestRegistrar {
    PTestRegistrar(const char* testName, P_TEST_FUNC testFunc) {
        printf("%s", P_COLOR_BLUE);
        printf("Found test %s...\n", testName);
        printf("%s", P_COLOR_NORMAL);
      PTestRegistry::Get().AddTest(testName, testFunc);
    };
  };
  //#define P_TEST(TEST_NAME, ...) PTestRegistry::Get().AddTest(TEST_NAME,MAKE_UNIQUE_TESTNAME)
}

#endif// _P_TEST_H_
