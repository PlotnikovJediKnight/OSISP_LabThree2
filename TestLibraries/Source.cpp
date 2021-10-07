#include <iostream>
#include <Windows.h>
#include <utility>
#include "StaticLibraryInjection.h"
#include "test_runner.h"
using namespace std;

char charArray[] = "DOG CAT PECULIAR STRING LAPTOP WINDOWS";
const char constArray[] = "CAT CAT DOG DOG LAPTOP LAPTOP WINDOWS PECULIAR STRING";

string cppstring = "C++ was created by Bjarne Stroustrup";
const string constcppstring = "try replacing me please";

#define WRITE_STRING(title, string) \
cout << #title << ": " << string << endl;

void LongTest() {
	StaticLibraryInjection::Injection a("DOG", "===");
	a.DoReplacement();

	AssertEqual(string(charArray), "=== CAT PECULIAR STRING LAPTOP WINDOWS", "non-const C-array was not changed! DOG");
	AssertEqual(string(constArray), "CAT CAT DOG DOG LAPTOP LAPTOP WINDOWS PECULIAR STRING", "const C-array was changed!");

	a = pair{ "PECULIAR STRING", "FFFFFFFFFFFFFFF" };
	a.DoReplacement();

	AssertEqual(string(charArray), "=== CAT FFFFFFFFFFFFFFF LAPTOP WINDOWS", "non-const C-array was not changed! FFF");
	AssertEqual(string(constArray), "CAT CAT DOG DOG LAPTOP LAPTOP WINDOWS PECULIAR STRING", "const C-array was changed!");

	a = pair{ "Bjarne Stroustrup", "BJARNE STROUSTRUP" };
	a.DoReplacement();

	AssertEqual(cppstring, "C++ was created by BJARNE STROUSTRUP", "C++ string was not changed!");

	a = pair{ "try replacing me please", "youv just been replaced" };
	a.DoReplacement();

	AssertEqual(constcppstring, "youv just been replaced");
}

int main() {
	TestRunner tr;
	tr.RunTest(LongTest, "LongTest");

	WRITE_STRING("Non-const C-array ", charArray);
	WRITE_STRING("Const C-array ", constArray);
	WRITE_STRING("C++ string ", cppstring);
	WRITE_STRING("Const C++ string ", constcppstring);

	return 0;
}