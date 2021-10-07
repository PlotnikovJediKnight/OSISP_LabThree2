#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <exception>
#include <cstdint>
#include <vector>
#include <sstream>
#include <fstream>
#include <utility>
#include <iterator>

//#define FILE_OUTPUT_DEBUG 

#ifdef _EXPORTING
#define DECLSPEC    __declspec(dllexport)
#else
#define DECLSPEC    __declspec(dllimport)
#endif

namespace DynamicLibraryInjection {

	class IInjection {
	public:
		virtual const IInjection& operator=(std::pair<std::string, std::string> strings) = 0;
		virtual bool DoReplacement() = 0;
	private:
		virtual bool TryReplacing(char* start_pointer, const std::vector<char>& chars, size_t offset) = 0;
	};


	class Injection : public IInjection {
	public:
		Injection(std::string be_replaced, std::string replacement);

		virtual const Injection& operator=(std::pair<std::string, std::string> strings) override;

		Injection(Injection&) = delete;
		Injection(Injection&&) = delete;
		Injection& operator=(Injection&) = delete;
		Injection& operator=(Injection&&) = delete;
		virtual bool DoReplacement() override;
	private:
		virtual bool TryReplacing(char* start_pointer, const std::vector<char>& chars, size_t offset) override;
		std::string be_replaced_;
		std::string replacement_;
		DWORD page_size_;
		uint64_t min_address_;
		uint64_t max_address_;
	};

	template <typename T>
	void print_hex(std::ostream& stream, T x, int width = 8) {
		stream << std::hex << std::setw(width) << std::setfill('0') << x << std::dec;
	}

	template <typename T>
	void print_address(std::ostream& stream, T x) {
		if (x < 0x100)
			print_hex(stream, x, 2);
		else if (x < 0x10000)
			print_hex(stream, x, 4);
		else if (x < 0x100000000ULL)
			print_hex(stream, x, 8);
		else
			print_hex(stream, x, 16);
	}

	template <typename T>
	void zero_struct(T& mem) {
		memset(&mem, 0, sizeof(mem));
	}

	struct memory_region {
		std::uint64_t start, size;
		MEMORY_BASIC_INFORMATION info;
	};
}

extern "C" DECLSPEC DynamicLibraryInjection::IInjection * APIENTRY GetInjection(const char* be_replaced, const char* replacement);