#include "scpbus.h"

BOOL scpbus::is_bus_installed()
{
	TCHAR path[MAX_PATH];

	int n = get_bus_path(path, MAX_PATH);

	if (n > 0)
		return TRUE;
	else
		return FALSE;
}

#pragma warning( push )
#pragma warning( disable : 4789 )

int scpbus::get_bus_path(LPCTSTR path, UINT size)
{
	SP_DEVICE_INTERFACE_DATA deviceInterfaceData = {};
	deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
	GUID deviceClassGuid = GUID_DEVINTERFACE_SCPVBUS;
	DWORD memberIndex = 0;
	DWORD requiredSize = 0;

	auto deviceInfoSet = SetupDiGetClassDevs(&deviceClassGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &deviceClassGuid, memberIndex, &deviceInterfaceData))
	{
		// get required target buffer size
		SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);

		// allocate target buffer
		auto detailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(requiredSize));
		if (detailDataBuffer == NULL)
			return -1;
		detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		// get detail buffer
		if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, requiredSize, &requiredSize, nullptr))
		{
			SetupDiDestroyDeviceInfoList(deviceInfoSet);
			free(detailDataBuffer);
			return -1;
		}

		// Copy	the path to output buffer
		memcpy((void*)path, detailDataBuffer->DevicePath, requiredSize * sizeof(WCHAR));

		// Cleanup
		SetupDiDestroyDeviceInfoList(deviceInfoSet);
		free(detailDataBuffer);
	}
	else
		return -1;

	return requiredSize;
}

HANDLE scpbus::get_bus_handle(void)
{
	TCHAR path[MAX_PATH];

	int n = get_bus_path(path, MAX_PATH);

	if (n < 1)
		return INVALID_HANDLE_VALUE;

	// bus found, open it and obtain handle
	g_hBus = CreateFile(path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	return g_hBus;
}

BOOL scpbus::get_create_proc_ID(DWORD UserIndex, PULONG ProcID)
{
	BOOL out = FALSE;
	ULONG buffer[1];
	ULONG output[1];
	DWORD trasfered = 0;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	if (g_hBus == INVALID_HANDLE_VALUE)
		g_hBus = get_bus_handle();
	if (g_hBus == INVALID_HANDLE_VALUE)
		return out;

	// Prepare the User Index for sending
	buffer[0] = UserIndex;

	// Send request to bus
	if (DeviceIoControl(g_hBus, IOCTL_BUSENUM_PROC_ID, buffer, _countof(buffer), output, 4, &trasfered, nullptr))
	{
		*ProcID = output[0];
		out = TRUE;
	};

	return out;
}

BOOL scpbus::set_xoutput_state(DWORD UserIndex, XINPUT_GAMEPAD* pGamepad)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex > 4)
		return out;

	if (this->g_vDevice[UserIndex - 1] == 0)
		return out;

	DWORD trasfered = 0;
	BYTE buffer[28] = {};

	buffer[0] = 0x1C;
	buffer[1] = 0x0;
	buffer[2] = 0x0;
	buffer[3] = 0x0;

	// encode user index
	buffer[4] = ((UserIndex >> 0) & 0xFF);
	buffer[5] = ((UserIndex >> 8) & 0xFF);
	buffer[6] = ((UserIndex >> 16) & 0xFF);
	buffer[7] = ((UserIndex >> 24) & 0xFF);
	buffer[8] = 0x0;
	buffer[9] = 0x14;

	// concat gamepad info to buffer
	memcpy_s(&buffer[10], _countof(buffer), pGamepad, sizeof(XINPUT_GAMEPAD));

	// vibration and LED info end up here
	BYTE output[FEEDBACK_BUFFER_LENGTH] = {};

	// send report to bus, receive vibration and LED status
	if (!DeviceIoControl(g_hBus, 0x2A400C, buffer, _countof(buffer), output, FEEDBACK_BUFFER_LENGTH, &trasfered, nullptr))
	{
		return FALSE;
	}

	// Save last successful position data
	memcpy_s(&g_Gamepad[UserIndex - 1], sizeof(XINPUT_GAMEPAD), pGamepad, sizeof(XINPUT_GAMEPAD));

	return TRUE;
}

BOOL scpbus::set_get_xoutput_state(DWORD UserIndex, XINPUT_GAMEPAD* pGamepad, PBYTE bVibrate, PBYTE bLargeMotor, PBYTE bSmallMotor, PBYTE bLed)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	if (!g_vDevice[UserIndex - 1])
		return out;

	DWORD trasfered = 0;
	BYTE buffer[28] = {};

	buffer[0] = 0x1C;

	// encode user index
	buffer[4] = ((UserIndex >> 0) & 0xFF);
	buffer[5] = ((UserIndex >> 8) & 0xFF);
	buffer[6] = ((UserIndex >> 16) & 0xFF);
	buffer[7] = ((UserIndex >> 24) & 0xFF);

	buffer[9] = 0x14;

	// concat gamepad info to buffer
	memcpy_s(&buffer[10], _countof(buffer), pGamepad, sizeof(XINPUT_GAMEPAD));

	// vibration and LED info end up here
	BYTE output[FEEDBACK_BUFFER_LENGTH] = {};

	// send report to bus, receive vibration and LED status
	if (!DeviceIoControl(g_hBus, 0x2A400C, buffer, _countof(buffer), output, FEEDBACK_BUFFER_LENGTH, &trasfered, nullptr))
	{
		return FALSE;
	}

	// Save last successful position data
	memcpy_s(&g_Gamepad[UserIndex - 1], sizeof(XINPUT_GAMEPAD), pGamepad, sizeof(XINPUT_GAMEPAD));

	// cache feedback
	if (bVibrate != nullptr)
	{
		*bVibrate = (output[1] == 0x08) ? 0x01 : 0x00;
	}

	if (bLargeMotor != nullptr)
	{
		*bLargeMotor = output[3];
	}

	if (bSmallMotor != nullptr)
	{
		*bSmallMotor = output[4];
	}

	if (bLed != nullptr)
	{
		*bLed = output[8];
	}

	return TRUE;
}

WORD scpbus::convert_button(LONG vBtns, WORD xBtns, UINT vBtn, UINT xBtn)
{
	WORD out;
	out = ((vBtns & (1 << (vBtn - 1))) == 0) ? xBtns & ~xBtn : xBtns | xBtn;
	return out;
}

BOOL scpbus::unplug_opt(UINT UserIndex, BOOL Force)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	if (g_hBus == INVALID_HANDLE_VALUE)
		g_hBus = get_bus_handle();
	if (g_hBus == INVALID_HANDLE_VALUE)
		return out;

	DWORD trasfered = 0;
	BUSENUM_UNPLUG_HARDWARE buffer = {};


	buffer.Size = sizeof(BUSENUM_UNPLUG_HARDWARE);
	buffer.SerialNo = UserIndex;

	if (Force)
		buffer.Flags = 0x0001;
	else
		buffer.Flags = 0x0000;

	if (DeviceIoControl(g_hBus, 0x2A4004, (LPVOID)(&buffer), buffer.Size, nullptr, 0, &trasfered, nullptr))
	{
		out = TRUE;
		g_vDevice[UserIndex - 1] = FALSE;

		std::cout << "plug out success!" << std::endl;
	}

	return out;
}
#pragma warning( pop )
