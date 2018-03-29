#pragma once
#ifndef _P_TEST_H_
#define _P_TEST_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#endif

#define P_ASSERT(assertion) \
	{ \
		bool CAT(passertion, __LINE__) = (assertion); \
	    PTest::PTestRegistry::Get().AddAssertion(PTest::PAssertion(CAT(passertion, __LINE__), __FILE__, __LINE__, #assertion)); \
	    if(!(CAT(passertion, __LINE__))) \
	    	return; \
	}

#define P_EXPECT(assertion) \
    PTest::PTestRegistry::Get().AddAssertion(PTest::PAssertion((#assertion), __FILE__, __LINE__, #assertion), true);

#define STRINGIFY(x) #x

#define CAT_HELPER(x, y) x##y
#define CAT(val1, val2) CAT_HELPER(val1, val2)

#define MAKE_UNIQUE_TESTNAME(name) CAT(name, __LINE__)

#define P_TEST(TEST_NAME) \
    static void MAKE_UNIQUE_TESTNAME(TEST_NAME)(); \
    PTest::PTestRegistrar MAKE_UNIQUE_TESTNAME(TEST_NAME##reg)(STRINGIFY(TEST_NAME), MAKE_UNIQUE_TESTNAME(TEST_NAME)); \
    static void MAKE_UNIQUE_TESTNAME(TEST_NAME)()

#define P_TEST_F(TEST_FIXTURE, TEST_NAME) \
	class MAKE_UNIQUE_TESTNAME(TEST_NAME##clas) : public TEST_FIXTURE { \
		virtual void Test(); \
	}; \
	PTest::PTestRegistrar MAKE_UNIQUE_TESTNAME(TEST_NAME##reg)(STRINGIFY(TEST_NAME), STRINGIFY(TEST_FIXTURE), new MAKE_UNIQUE_TESTNAME(TEST_NAME##clas)()); \
	void MAKE_UNIQUE_TESTNAME(TEST_NAME##clas)::Test()

namespace PTest {

	class PTestFixture {
	public:
		virtual void Create() {};
		virtual void Destroy() {};
		virtual void Test() = 0;
	};

	typedef void(*P_TEST_FUNC)(void);

	void PrintBars(uint32_t num = 2) {
		for (uint32_t i = 0; i < num; ++i) {
			printf("======================");
		}
		putchar('\n');
	}

	enum class PColor {
		Normal,
		Green,
		Blue,
		Red
	};

	void SetConsoleColor(PColor color) {
#ifdef __linux__
		switch (color) {
		case PColor::Red:		printf("\x1b[31m"); break;
		case PColor::Green:	printf("\x1b[32m"); break;
		case PColor::Blue:	printf("\x1b[36m"); break;
		default:						printf("\x1B[0m");
		}
#endif
#ifdef _WIN32
		static HANDLE handle = 0;
		if (!handle) {
			handle = GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(handle, &csbi);
		}
		WORD attributes;
		switch (color) {
		case PColor::Red: attributes = FOREGROUND_RED; break;
		case PColor::Blue: attributes = FOREGROUND_BLUE | FOREGROUND_GREEN; break;
		case PColor::Green: attributes = FOREGROUND_GREEN; break;
		default: attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
		}
		SetConsoleTextAttribute(handle, attributes);
#endif
	}

	class PAssertion {
	private:
		bool success_;
		std::string condition_;
		std::string file_;
		uint32_t line_;
		std::string text_;
	public:
		PAssertion(bool success, const char* file, uint32_t line, const char* condition, const char* text = nullptr) :
			success_(success), condition_(condition), file_(file), line_(line) {
			if (text) {
				text_ = text;
			}
		};

		~PAssertion() = default;

		bool GetSuccess() const { return success_; }
		const std::string& GetCondition() const { return condition_; }
		const std::string& GetFile() const { return file_; }
		uint32_t GetLine() const { return line_; }
		const std::string& GetText() const { return text_; }

		explicit operator bool() const {
			return success_;
		}
	};

	class PTest {
	private:
		std::string name_;
		std::string fixtureName_;
		P_TEST_FUNC func_;
		PTestFixture *fixture_;
		bool run_, succeeded_;
		std::vector<PAssertion> assertions_;
		std::chrono::duration<double, std::milli> duration_;
	public:
		PTest(const char* testName, P_TEST_FUNC testFunc) :
			name_(testName), func_(testFunc),
			fixture_(nullptr), run_(false),
			succeeded_(false), duration_(0)
		{};

		PTest(const char* testName, const char* testFixture, PTestFixture *fixture) :
			name_(testName), fixtureName_(testFixture), func_(nullptr),
			fixture_(fixture), run_(false),
			succeeded_(false), duration_(0)
		{};

		~PTest() = default;

		void AddAssertion(PAssertion assertion, bool is_expect = false) {
			assertions_.push_back(assertion);
			if (!assertion) {
				SetConsoleColor(PColor::Red); putchar('\n'); PrintBars();
				if (is_expect) {
					printf("Expectation Failure in %s (Line %u)\n", assertion.GetFile().c_str(), assertion.GetLine());
				}
				else {
					printf("Assertion Failure in %s (Line %u)\n", assertion.GetFile().c_str(), assertion.GetLine());
				}
				printf("     Condition not met:\n\t");
				SetConsoleColor(PColor::Normal); printf("(%s)\n\n", assertion.GetCondition().c_str());
			}
		}

		bool HasSucceeded() const { return succeeded_; }
		bool HasRun() const { return run_; }
		bool HasFixture() const { return fixture_; }

		const std::string& GetName() const { return name_; }
		const std::string& GetFixtureName() const { return fixtureName_; }
		std::chrono::duration<double, std::milli> GetDuration() const { return duration_; }
		
		PTestFixture* GetFixture() const { return fixture_; }

		bool Run() {
			SetConsoleColor(PColor::Green);
			printf("[Running] ");
			SetConsoleColor(PColor::Normal);

			if (fixture_) {
				printf("%s.", fixtureName_.c_str());
			}
			printf("%s:\n", name_.c_str());

			run_ = false;
			succeeded_ = true;
			assertions_.clear();

			auto start = std::chrono::high_resolution_clock::now();

			if (!fixture_) {
				func_();
			}
			else {
				fixture_->Create();
				fixture_->Test();
				fixture_->Destroy();
			}
			auto end = std::chrono::high_resolution_clock::now();

			duration_ = end - start;

			size_t failedAssertionCount = 0;

			for (PAssertion& assertion : assertions_) {
				if (!assertion.GetSuccess()) {
					succeeded_ = false;
					failedAssertionCount++;
				}
			}

			SetConsoleColor(PColor::Blue); printf(" Test '%s' finished. (%f ms)\n", name_.c_str(), duration_.count());
			succeeded_ ? SetConsoleColor(PColor::Green) : SetConsoleColor(PColor::Red);
			printf("   %zu assertions failed.\n", failedAssertionCount);
			SetConsoleColor(PColor::Green); printf("   %zu assertions run.\n", assertions_.size());

			if (!succeeded_) {
				SetConsoleColor(PColor::Red);
				PrintBars();
			}
			SetConsoleColor(PColor::Normal);
			return succeeded_;
		}
	};

	class PTestRegistry {
		std::unordered_map<std::string, PTest> testMap_;
		std::vector<std::unordered_map<std::string, PTest>::iterator> index_;

		PTest *currentTest_ = nullptr;

	public:
		static PTestRegistry& Get() {
			static PTestRegistry registry;
			return registry;
		}
		
		~PTestRegistry() {
			// Free all test fixtures
			for (auto& it : index_) {
				auto& ptest = it->second;
				if(ptest.HasFixture()) {
					delete(ptest.GetFixture());
				}
			}
		}

		void AddAssertion(PAssertion assertion, bool is_expect = false) {
			if (currentTest_) {
				currentTest_->AddAssertion(assertion, is_expect);
			}
			else {
				SetConsoleColor(PColor::Red);
				printf("ASSERTION \"%s\" on line %u of %s:\n\tINVALID OUTSIDE OF TEST CASE\n",
					assertion.GetCondition().c_str(), assertion.GetLine(), assertion.GetFile().c_str());
				SetConsoleColor(PColor::Normal);
			}
		}

		void AddTest(PTest&& ptest) {
			auto &name = ptest.GetName();

			if (testMap_.find(name) != testMap_.end()) {
				SetConsoleColor(PColor::Red);
				printf("[DUPLICATE TESTS FOUND for \"%s!\" ABORTING...]\n", name.c_str());
				SetConsoleColor(PColor::Normal);
				return;
			}

			index_.push_back(testMap_.insert(testMap_.cbegin(), { name, ptest }));
		}

		bool RunTest(std::string testName) {
			auto it = testMap_.find(testName);
			if (it == testMap_.end()) {
				SetConsoleColor(PColor::Red);
				printf("[TEST \"%s\" NOT FOUND!]\n", testName.c_str());
				SetConsoleColor(PColor::Normal);
				return false;
			}

			currentTest_ = &it->second;

			return it->second.Run();
		}

		int RunAllTests() {
			size_t testsRunCount = 0;
			size_t testsFailedCount = 0;

			SetConsoleColor(PColor::Green);
			printf("\n[Running all tests]\n\n");
			SetConsoleColor(PColor::Normal);

			std::chrono::duration<double, std::milli> totalDuration = std::chrono::milliseconds(0);

			for (auto& it : index_) {
				auto& theTest = it->second;
				currentTest_ = &theTest;
				if (!theTest.Run()) {
					testsFailedCount++;
				}
				currentTest_ = nullptr;
				totalDuration += theTest.GetDuration();
				testsRunCount++;
			}

			putchar('\n');
			SetConsoleColor(PColor::Normal); PrintBars(3); printf("[Test Summary]:");
			SetConsoleColor(PColor::Blue); printf("\t\t\t(Total Time: %f ms)\n", totalDuration.count());
			SetConsoleColor(PColor::Green); printf("\tTests Passed:\t");
			SetConsoleColor(PColor::Normal); printf("%zu\n", testsRunCount - testsFailedCount);
			testsFailedCount ? SetConsoleColor(PColor::Red) : SetConsoleColor(PColor::Green);
			printf("\tTests Failed:\t");
			SetConsoleColor(PColor::Normal); printf("%zu\n", testsFailedCount);
			SetConsoleColor(PColor::Blue); printf("\tTotal Tests:\t");
			SetConsoleColor(PColor::Normal); printf("%zu\n", testsRunCount); PrintBars(3);

			return testsFailedCount;
		}

		PTest* GetCurrentTest() { return currentTest_; }

		inline size_t GetNumTests() { return testMap_.size(); }
	};

	struct PTestRegistrar {
		PTestRegistrar(const char* testName, P_TEST_FUNC testFunc) {
			SetConsoleColor(PColor::Blue);
			printf("Found test %s...\n", testName);
			SetConsoleColor(PColor::Normal);
			PTestRegistry::Get().AddTest(PTest(testName, testFunc));
		};
		PTestRegistrar(const char* testName, const char* fixtureName, PTestFixture* fixture) {
			SetConsoleColor(PColor::Blue);
			printf("Found test %s.%s...\n", fixtureName, testName);
			SetConsoleColor(PColor::Normal);
			PTestRegistry::Get().AddTest(std::move(PTest(testName, fixtureName, fixture)));
		}
	};
}

#ifdef USE_PTEST_MAIN
int main() {
	return PTest::PTestRegistry::Get().RunAllTests();
}
#endif

#endif // _P_TEST_H_