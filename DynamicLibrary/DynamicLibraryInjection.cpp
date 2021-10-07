#include "pch.h"
using namespace std;

namespace StaticLibraryInjection {
	Injection::Injection(string be_replaced, string replacement) : be_replaced_(move(be_replaced)), replacement_(move(replacement)) {
		if (be_replaced_.size() != replacement_.size()) throw runtime_error("Strings must have equal length!");
		if (be_replaced_.empty() || replacement_.empty()) throw runtime_error("Strings must not be empty!");
		SYSTEM_INFO sinf;
		GetSystemInfo(&sinf);
		page_size_ = sinf.dwPageSize;
		min_address_ = (uint64_t)(sinf.lpMinimumApplicationAddress);
		max_address_ = (uint64_t)(sinf.lpMaximumApplicationAddress);
	}

	bool Injection::DoReplacement() {
		bool result = false;
		HANDLE procHandle = GetCurrentProcess();

		vector<memory_region> regions;

		for (uint64_t address = min_address_; address <= max_address_;) {
			MEMORY_BASIC_INFORMATION mbi; zero_struct(mbi);
			SIZE_T bytes = VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi));
			if (!bytes) {
				address += page_size_;
				continue;
			}
			if (mbi.State == MEM_COMMIT && (mbi.Protect & PAGE_GUARD) != PAGE_GUARD)
				regions.push_back(memory_region{ (std::uint64_t)mbi.BaseAddress, mbi.RegionSize, mbi });

			address += mbi.RegionSize;
		}

#ifdef FILE_OUTPUT_DEBUG
		ofstream out("out.txt");
#endif

		for (auto& region : regions) {
			vector<char> buffer(region.size); SIZE_T read = 0;

			if (!ReadProcessMemory(procHandle, (LPCVOID)region.start, &buffer[0], buffer.size(), &read)) {
				auto error = GetLastError();
				if (error != ERROR_PARTIAL_COPY) {
					std::cerr << "ReadProcessMemory() failed with error " << error << " (0x";
					print_hex(std::cerr, error);
					std::cerr << ")\n";
					return false;
				}
			}

			if (read < region.size) {
				std::cerr << "Warning: region starting at 0x";
				print_address(std::cerr, region.start);
				std::cerr << " has size " << region.size << ", but only " << read
					<< " bytes could be read by ReadProcessMemory().\n";
				memset(&buffer[read], 0, buffer.size() - read);
			}

#ifdef FILE_OUTPUT_DEBUG
			string output_string(buffer.begin(), buffer.end());
			out << output_string << endl;
#endif

			for (size_t i = 0; i < buffer.size(); ++i) {
				TryReplacing(static_cast<char*>(region.info.BaseAddress) + i, buffer, i);
			}
		}

#ifdef FILE_OUTPUT_DEBUG	
		out.close();
#endif

		return result;
	}

	bool Injection::TryReplacing(char* start_pointer, const vector<char>& chars, size_t offset) {
		HANDLE procHandle = GetCurrentProcess();
		const char* be_replaced_pointer = be_replaced_.c_str();
		if (be_replaced_pointer == start_pointer) return false;

		size_t i = 0, j = 0;
		size_t letters = be_replaced_.size();

		while (letters != 0) {
			if (be_replaced_pointer[i++] != chars[offset + j++]) break;
			letters--;
		}

		if (letters == 0) {
			SIZE_T t = 0;
			WriteProcessMemory(procHandle, (LPVOID)start_pointer, (LPVOID)replacement_.c_str(), replacement_.size(), &t);
			return true;
		}

		return false;
	}

	const Injection& Injection::operator=(pair<string, string> strings) {
		be_replaced_ = move(strings.first);
		replacement_ = move(strings.second);
		if (be_replaced_.size() != replacement_.size()) throw runtime_error("Strings must have equal length!");
		if (be_replaced_.empty() || replacement_.empty()) throw runtime_error("Strings must not be empty!");
	}
}
