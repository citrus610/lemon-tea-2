#include "memory_reader.h"

uintptr_t memory_reader::get_module_base(DWORD process_id, const WCHAR* module_name)
{
	uintptr_t ModuleBaseAddress = (uintptr_t)NULL;
	HANDLE Module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
	MODULEENTRY32 ModuleEntry;
	ModuleEntry.dwSize = sizeof(ModuleEntry);

	if (Module32First(Module, &ModuleEntry)) {
		do {
			if (!wcscmp(ModuleEntry.szModule, module_name)) {
				ModuleBaseAddress = (uintptr_t)ModuleEntry.modBaseAddr;
				break;
			}
		} while (Module32Next(Module, &ModuleEntry));
	}

	CloseHandle(Module);
	return ModuleBaseAddress;
};

uintptr_t memory_reader::get_pointer_address(HANDLE process_handle, uintptr_t start_address, uintptr_t* offsets, int offsets_count)
{
	uintptr_t ptr = read_data<uintptr_t>(process_handle, start_address);
	ptr += offsets[0];
	for (int i = 1; i < offsets_count; i++) {
		ptr = read_data<uintptr_t>(process_handle, ptr);
		ptr += offsets[i];
	}
	return ptr;
};
