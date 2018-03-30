#pragma once
#ifndef _P_TEST_H_
#define _P_TEST_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#include <type_traits>

#ifdef _WIN32
#include <Windows.h>
#endif

template<typename S, typename T>
class is_streamable
{
    template<typename SS, typename TT>
    static auto test(int)
    -> decltype( std::declval<SS&>() << std::declval<TT>(), std::true_type() );

    template<typename, typename>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<S,T>(0))::value;
};

template<typename E>
constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

template<typename T>
void print_value(std::enable_if_t<std::is_enum<T>::value
										&& !is_streamable<std::ostream, T>::value, T> t) {
	std::cout << to_integral(t);
}

template<typename T>
void print_value(std::enable_if_t<is_streamable<std::ostream , T>::value
																	&& !(std::is_enum<T>::value || std::is_fundamental<T>::value), T> t) {
	std::cout << t;
}

template<typename T>
void print_value(std::enable_if_t<is_streamable<std::ostream, T>::value
																&& (std::is_enum<T>::value || std::is_fundamental<T>::value), T> t) {
	std::cout << t;
}

template<typename T>
void print_value(T t) {
  std::cout << "(unprintable)";
}

#define P_ASSERT(assertion) \
	{ \
		bool CAT(passertion, __LINE__) = (assertion); \
	    PTest::PTestRegistry::Get().AddAssertion(PTest::PAssertion(CAT(passertion, __LINE__), __FILE__, __LINE__, #assertion)); \
	    if(!(CAT(passertion, __LINE__))) { \
			  PTest::PrintAssertionText(false, __FILE__, __LINE__, #assertion);\
	    	return; \
			}\
	}

#ifdef __linux__
#define GET_TYPES(val1, val2) \
	const char* type1 = abi::__cxa_demangle(typeid(val1).name(), 0, 0, NULL);\
	const char* type2 = abi::__cxa_demangle(typeid(val2).name(), 0, 0, NULL);
#endif

#ifdef _WIN32
#define GET_TYPES(val1, val2) \
	const char* type1 = typeid(val1).name();\
	const char* type2 = typeid(val2).name();
#endif

#define P_ASSERT_COMP(val1, val2, OPERATOR, OP_STR) \
	{\
		bool CAT(passertion, __LINE__) = (val1 OPERATOR val2); \
		PTest::PTestRegistry::Get().AddAssertion(PTest::PAssertion(CAT(passertion, __LINE__), __FILE__, __LINE__, #val1 #OPERATOR #val2)); \
		PTest::PrintAssertionText(false, __FILE__, __LINE__, #val1 " " #OPERATOR " " #val2);\
		if (!(CAT(passertion, __LINE__))) { \
			GET_TYPES(val1, val2)\
			SetConsoleColor(PTest::PColor::LightRed);\
			printf("   -------------------------\n");\
			SetConsoleColor(PTest::PColor::Red);\
			printf("   | %s is not %s %s.\n", #val1, OP_STR, #val2);\
			SetConsoleColor(PTest::PColor::Yellow); printf("   |   Expression: ");\
			SetConsoleColor(PTest::PColor::Normal); printf("%s ", #val1);\
			SetConsoleColor(PTest::PColor::Yellow); printf(" vs. ");\
			SetConsoleColor(PTest::PColor::Normal); printf("%s\n", #val2);\
			SetConsoleColor(PTest::PColor::Yellow); printf("   |   Type: ");\
			SetConsoleColor(PTest::PColor::Normal); printf("%s", type1);\
			SetConsoleColor(PTest::PColor::Yellow); if(std::is_enum<decltype(val1)>::value) printf(" (Enum)");\
			SetConsoleColor(PTest::PColor::Yellow); printf(" vs. ");\
			SetConsoleColor(PTest::PColor::Red); printf("%s", type2);\
			SetConsoleColor(PTest::PColor::Yellow); if(std::is_enum<decltype(val2)>::value) printf(" (Enum)");\
			SetConsoleColor(PTest::PColor::Yellow); printf("\n   |   Value: ");\
			SetConsoleColor(PTest::PColor::Normal); print_value<decltype(val1)>(val1);\
			SetConsoleColor(PTest::PColor::Yellow); printf(" vs. ");\
			SetConsoleColor(PTest::PColor::Red); print_value<decltype(val2)>(val2);\
			SetConsoleColor(PTest::PColor::LightRed);\
			printf("\n   -------------------------\n\n");\
			SetConsoleColor(PTest::PColor::Normal);\
			return; \
		}\
	}

#define P_EXPECT_COMP(val1, val2, OPERATOR, OP_STR) \
	{\
		bool CAT(passertion, __LINE__) = (val1 OPERATOR val2); \
		PTest::PTestRegistry::Get().AddAssertion(PTest::PAssertion(CAT(passertion, __LINE__), __FILE__, __LINE__, #val1 #OPERATOR #val2)); \
		PTest::PrintAssertionText(true, __FILE__, __LINE__, #val1 " " #OPERATOR " " #val2);\
		if (!(CAT(passertion, __LINE__))) { \
			GET_TYPES(val1, val2)\
			SetConsoleColor(PTest::PColor::LightRed);\
			printf("   -------------------------\n");\
			SetConsoleColor(PTest::PColor::Red);\
			printf("   | %s is not %s %s.\n", #val1, OP_STR, #val2);\
			SetConsoleColor(PTest::PColor::Yellow); printf("   |   Expression: ");\
			SetConsoleColor(PTest::PColor::Normal); printf("%s ", #val1);\
			SetConsoleColor(PTest::PColor::Yellow); printf(" vs. ");\
			SetConsoleColor(PTest::PColor::Normal); printf("%s\n", #val2);\
			SetConsoleColor(PTest::PColor::Yellow); printf("   |   Type: ");\
			SetConsoleColor(PTest::PColor::Normal); printf("%s", type1);\
			SetConsoleColor(PTest::PColor::Yellow); if(std::is_enum<decltype(val1)>::value) printf(" (Enum)");\
			SetConsoleColor(PTest::PColor::Yellow); printf(" vs. ");\
			SetConsoleColor(PTest::PColor::Red); printf("%s", type2);\
			SetConsoleColor(PTest::PColor::Yellow); if(std::is_enum<decltype(val2)>::value) printf(" (Enum)");\
			SetConsoleColor(PTest::PColor::Yellow); printf("\n   |   Value: ");\
			SetConsoleColor(PTest::PColor::Normal); print_value<decltype(val1)>(val1);\
			SetConsoleColor(PTest::PColor::Yellow); printf(" vs. ");\
			SetConsoleColor(PTest::PColor::Red); print_value<decltype(val2)>(val2);\
			SetConsoleColor(PTest::PColor::LightRed);\
			printf("\n   -------------------------\n\n");\
			SetConsoleColor(PTest::PColor::Normal);\
		}\
	}

const static char E_STR[] = "equal to";
const static char NE_STR[] = "unequal to";
const static char GT_STR[] = "greater than";
const static char GE_STR[] = "greater than or equal to";
const static char LT_STR[] = "less than";
const static char LE_STR[] = "less than or equal to";

#define P_ASSERT_EQ(val1, val2) P_ASSERT_COMP(val1, val2, ==, E_STR)
#define P_ASSERT_NE(val1, val2) P_ASSERT_COMP(val1, val2, !=, NE_STR)
#define P_ASSERT_GT(val1, val2) P_ASSERT_COMP(val1, val2, >, GT_STR)
#define P_ASSERT_GE(val1, val2) P_ASSERT_COMP(val1, val2, >=, GE_STR)
#define P_ASSERT_LT(val1, val2) P_ASSERT_COMP(val1, val2, <, LT_STR)
#define P_ASSERT_LE(val1, val2) P_ASSERT_COMP(val1, val2, <=, LE_STR)

#define P_EXPECT_EQ(val1, val2) P_EXPECT_COMP(val1, val2, ==, E_STR)
#define P_EXPECT_NE(val1, val2) P_EXPECT_COMP(val1, val2, !=, NE_STR)
#define P_EXPECT_GT(val1, val2) P_EXPECT_COMP(val1, val2, >, GT_STR)
#define P_EXPECT_GE(val1, val2) P_EXPECT_COMP(val1, val2, >=, GE_STR)
#define P_EXPECT_LT(val1, val2) P_EXPECT_COMP(val1, val2, <, LT_STR)
#define P_EXPECT_LE(val1, val2) P_EXPECT_COMP(val1, val2, <=, LE_STR)

#define P_EXPECT(assertion) \
    PTest::PTestRegistry::Get().AddAssertion(PTest::PAssertion(!(#assertion), __FILE__, __LINE__, #assertion), true); \
		PTest::PrintAssertionText(true, __FILE__, __LINE__, #assertion);
		
#define STRINGIFY(x) #x

#define CAT_HELPER(x, y) x##y
#define CAT(val1, val2) CAT_HELPER(val1, val2)

#define MAKE_UNIQUE_TESTNAME(name) CAT(name, __LINE__)

#define P_TEST(TEST_NAME) \
    static void MAKE_UNIQUE_TESTNAME(TEST_NAME)(); \
    PTest::PTestRegistrar MAKE_UNIQUE_TESTNAME(TEST_NAME##Registrar)(STRINGIFY(TEST_NAME), MAKE_UNIQUE_TESTNAME(TEST_NAME)); \
    static void MAKE_UNIQUE_TESTNAME(TEST_NAME)()

#define P_TEST_F(TEST_FIXTURE, TEST_NAME) \
	class MAKE_UNIQUE_TESTNAME(TEST_NAME##Class) : public TEST_FIXTURE { \
		virtual void Test(); \
	}; \
	PTest::PTestRegistrar MAKE_UNIQUE_TESTNAME(TEST_NAME##Registrar)(STRINGIFY(TEST_NAME), STRINGIFY(TEST_FIXTURE), new MAKE_UNIQUE_TESTNAME(TEST_NAME##Class)()); \
	void MAKE_UNIQUE_TESTNAME(TEST_NAME##Class)::Test()

namespace PTest {

	class PTestFixture {
	public:
		virtual ~PTestFixture() {};
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
		Red,
		LightRed,
		Yellow,
		Cyan
	};

	void SetConsoleColor(PColor color) {
#ifdef __linux__
		switch (color) {
		case PColor::Red:		printf("\033[0;91m"); break;
		case PColor::LightRed:	printf("\033[0;31m"); break;
		case PColor::Green:		printf("\033[0;32m"); break;
		case PColor::Yellow:	printf("\033[0;33m"); break;
		case PColor::Cyan:		printf("\033[0;36m"); break;
		case PColor::Blue:		printf("\033[0;34m"); break;
		default:				printf("\033[0;37m");
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
		case PColor::LightRed: attributes = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
		case PColor::Green: attributes = FOREGROUND_GREEN; break;
		case PColor::Yellow: attributes = FOREGROUND_GREEN | FOREGROUND_RED; break;
		case PColor::Cyan: attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
		case PColor::Blue: attributes = FOREGROUND_BLUE | FOREGROUND_GREEN; break;
		
		default: attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
		}
		SetConsoleTextAttribute(handle, attributes);
#endif
	}

	static void PrintAssertionText(bool is_expectation, const char *filename, uint32_t line, const char *assertion) {
		SetConsoleColor(PColor::Red); putchar('\n'); PrintBars();
		printf("%s Failure in %s (Line %u)\n", is_expectation ? "Expectation" : "Assertion",
																					filename, line);
		printf("     Condition not met:\n\t");
		SetConsoleColor(PColor::Normal); printf("%s\n\n", assertion);
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
			/*if (!assertion) {
				SetConsoleColor(PColor::Red); putchar('\n'); PrintBars();
				if (is_expect) {
					printf("Expectation Failure in %s (Line %u)\n", assertion.GetFile().c_str(), assertion.GetLine());
				}
				else {
					printf("Assertion Failure in %s (Line %u)\n", assertion.GetFile().c_str(), assertion.GetLine());
				}
				printf("     Condition not met:\n\t");
				SetConsoleColor(PColor::Normal); printf("(%s)\n\n", assertion.GetCondition().c_str());
			}*/
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
					PTestFixture *fixture = ptest.GetFixture();
					delete fixture;
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
			std::string name;
			if (ptest.HasFixture()) {
				name = ptest.GetFixtureName() + "." + ptest.GetName();
			} else {
				name = ptest.GetName();
			}

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
