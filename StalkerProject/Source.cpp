#include <iostream>
#include <Windows.h>
using namespace std;

const char dll_path[] = "D:\\ÁÃÓÈĞ\\Òğåòèé êóğñ\\newRepos\\OSISP_LabThree\\Debug\\DynamicLibrary.dll";

int main() {
	int procID;
	cout << "Please, enter ID of the process you would like to corrupt: " << endl;
	cin >> procID;
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (process == NULL) {
		cout << "The specified process couldn’t be found" << endl;
	}

	LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (addr == NULL) {
		cout << "The LoadLibraryA function was not found inside kernel32.dll library" << endl;
	}


	LPVOID arg = (LPVOID)VirtualAllocEx(process, NULL, strlen(dll_path), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (arg == NULL) {
		cout << "The memory could not be allocated inside the chosen process" << endl;
	}


	int n = WriteProcessMemory(process, arg, dll_path, strlen(dll_path), NULL);
	if (n == 0) {
		cout << "There was no bytes written to the process’s address space" << endl;
	}

	HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)addr, arg, NULL, NULL);
	if (threadID == NULL) {
		cout << "The remote thread could not be created" << endl;
	}
	else {
		cout << "The remote thread was successfully created" << endl;
	}

	CloseHandle(process);
	getchar();

	return 0;
}