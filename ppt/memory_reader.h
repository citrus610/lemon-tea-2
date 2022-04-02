#pragma once

#include <iostream>
#include <stdlib.h>

// data struct
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <tchar.h>

// window
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <iomanip>
#pragma comment(lib, "psapi")

class memory_reader
{
public:
	uintptr_t get_module_base(DWORD process_id, const WCHAR* module_name);
public:
	uintptr_t get_pointer_address(HANDLE process_handle, uintptr_t start_address, uintptr_t* offsets, int offsets_count);

	template <typename T>
	T read_data(HANDLE process_handle, uintptr_t address) {
		T value;
		BOOL success = ReadProcessMemory(process_handle, (LPCVOID)address, &value, sizeof(value), NULL);
		if (success == FALSE) return 0xff;
		return value;
	}
};