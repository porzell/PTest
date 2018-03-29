# PTest
## What is PTest?
Glad you asked!  PTest is short for PicoTest.

### PicoTest is a header-only, super-lightweight unit testing library.

PTest allows you to quickly add unit tests to your project without the need for
compiling and linking a separate testing library (like gtest).

At ~230 LOC, PicoTest is absolutely tiny, but still can get the job done.

## So, how do I use this thing?
Easy!  First off you gotta make a test case.  That'll look like this:
```C++
P_TEST(MyTestName) {
    ...
}
```

Inside your P_TEST body, you'll want to add some assertions to test.  Basically,
any expression that would work in an if statement will work inside an assertion.

Here's how simple an example test file can be!

```C++
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
```

And here's what you'll see in your console:

```
Found test MyFirstTest...

[Running all tests]

[Running] MyFirstTest:
============================================
Assertion Failure in my_first_test.cpp (Line 12)
     Condition not met:
        (testString == "goodbye world!")

 Test "MyFirstTest" finished.
   1 assertions failed.
   2 assertions run.
============================================

==================================================================
[Test Summary]:
        Tests Passed:   0
        Tests Failed:   1
        Total Tests:    1
==================================================================
```

Nice, huh?

PTest will automatically gather all ```P_TEST()``` tests to run, so you don't
need to worry about it.

If you would like to run just one test you can also use:
```C++
return PTest::PTestRegistry::Get().RunTest("MyFirstTest");
```

## Planned Features
Currently, here is the planned list of PTest features to be supported:
* Descriptive assertions:
 This will allow you to add a custom text output to display on an assertion failure.
* Gherkin-like test syntax:
 Oh yeah, that's right!  PTest will soon be supporting a Gherkin-like block syntax to allow you to bring Test-driven development to your project in one tiny header.
* Expect(ations):
 For when you want your test to keep going, but want to make sure it fails in the end.
* Test tags/filters
 For when you're really rather choosy about your tests...

## License
PicoTest is licensed under the MIT License, so it's easy to integrate into any project, big or small.

```
MIT License

Copyright (c) 2018 Peter Orzell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Known Issues
Due to the (really stupid) way in which ```std::unordered_map``` currently works
under Linux, the tests get executed in the opposite order in which they were defined.
I will be fixing this as soon as possible.

## Have fun!
Let me know if you end up using this!  I'll be working on it regularly.