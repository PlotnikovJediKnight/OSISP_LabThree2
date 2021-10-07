#pragma once
#include <Windows.h>
#include <string>
#include <stdexcept>

template<typename ProcedureType, typename ObjectType>
class DllWrapper {
public:
	DllWrapper(std::string dllName, std::string functionName) : hLib(NULL), function_name_(move(functionName)), proc_(NULL){
		hLib = LoadLibraryA(dllName.c_str());
		if (!hLib) {
			throw std::runtime_error("Requested DLL was not found!");
		}
		proc_ = (ProcedureType)(GetProcAddress(hLib, function_name_.c_str()));
		if (!proc_) {
			throw std::runtime_error("Requested function was not found!");
		}
	}

	DllWrapper(DllWrapper&& other) : hLib(other.hLib), function_name_(std::move(other.function_name_)), proc_(other.proc_) {
		other.hLib = NULL;
	}

	ObjectType* InvokeProcedure(const char* be_replaced, const char* replacement) {
		return proc_(be_replaced, replacement);
	}

	DllWrapper(const DllWrapper&) = delete;
	DllWrapper& operator=(const DllWrapper&) = delete;
	DllWrapper& operator=(DllWrapper&&) = delete;

	~DllWrapper() {
		if (hLib)
			FreeLibrary(hLib);
	}

private:
	HMODULE hLib;
	std::string function_name_;
	ProcedureType proc_;
};