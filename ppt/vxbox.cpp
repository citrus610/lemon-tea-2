#include "vxbox.h"

bool vxbox::is_controller_exist(UINT controller_index)
{
	bool out = false;
	ULONG buffer[1];
	ULONG output[1];
	DWORD trasfered = 0;

	if (controller_index < 1 || controller_index > 4)
		return out;

	if (this->bus.g_hBus == INVALID_HANDLE_VALUE)
		this->bus.g_hBus = this->bus.get_bus_handle();
	if (this->bus.g_hBus == INVALID_HANDLE_VALUE)
		return out;

	std::cout << "over check 1\n";

	// Prepare the User Index for sending
	buffer[0] = controller_index;

	// Send request to bus
	if (DeviceIoControl(this->bus.g_hBus, 0x2A4100, buffer, _countof(buffer), output, 4, &trasfered, nullptr))
	{
		std::cout << "over check 2\n";
		if (output[0] != 0)
			out = true;
	};

	return out;
}

bool vxbox::is_controller_owned(UINT controller_index)
{
	ULONG OrigProcID = 0;
	ULONG ThisProcID = 0;

	// Sanity Check
	if (controller_index < 1 || controller_index > 4)
		return false;

	// Does controler exist?
	//if (!is_controller_exist(controller_index))
	//	return false;

	// Get ID of the process that created the controler?
	if (!this->bus.get_create_proc_ID(controller_index, &OrigProcID) || !OrigProcID)
		return false;

	// Get ID of current process
	ThisProcID = GetCurrentProcessId();
	if (!ThisProcID)
		return false;

	// Compare
	if (ThisProcID != OrigProcID)
		return false;

	return true;
}

bool vxbox::plugin(UINT controller_index)
{
	bool out = false;

	if (controller_index < 1 || controller_index > 4)
		return out;

	if (this->bus.g_hBus == INVALID_HANDLE_VALUE)
		this->bus.g_hBus = this->bus.get_bus_handle();
	if (this->bus.g_hBus == INVALID_HANDLE_VALUE)
		return out;

	DWORD trasfered = 0;
	UCHAR buffer[16] = {};

	buffer[0] = 0x10;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;

	buffer[4] = ((controller_index >> 0) & 0xFF);
	buffer[5] = ((controller_index >> 8) & 0xFF);
	buffer[6] = ((controller_index >> 16) & 0xFF);
	buffer[7] = ((controller_index >> 24) & 0xFF);

	if (DeviceIoControl(this->bus.g_hBus, 0x2A4000, buffer, _countof(buffer), nullptr, 0, &trasfered, nullptr))
	{
		out = true;
		this->bus.g_vDevice[controller_index - 1] = TRUE;
	};

	//CloseHandle(h);
	DWORD error = 0;
	if (out)
	{
		//std::cout << "IOCTL_BUSENUM_PLUGIN_HARDWARE 0X" << IOCTL_BUSENUM_PLUGIN_HARDWARE << "\n" << endl;
		error = 0;
	}
	else
	{
		error = GetLastError();
		//std::cout << "IOCTL_BUSENUM_PLUGIN_HARDWARE 0X" << IOCTL_BUSENUM_PLUGIN_HARDWARE << "Failed (Error:0X" << error << ")\n" << std::endl;
	}

	// std::cout << "Plugin controller " << controller_index << " success!" << std::endl;

	return out;
}

bool vxbox::unplug(UINT controller_index)
{
	return this->bus.unplug_opt(controller_index, TRUE);
}

bool vxbox::unplug_all()
{
	bool out = false;

	if (this->bus.g_hBus == INVALID_HANDLE_VALUE)
		this->bus.g_hBus = this->bus.get_bus_handle();
	if (this->bus.g_hBus == INVALID_HANDLE_VALUE)
		return out;

	DWORD trasfered = 0;
	UCHAR buffer[16] = {};

	buffer[0] = 0x10;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;

	if (DeviceIoControl(this->bus.g_hBus, 0x2A4004, buffer, 16, nullptr, 0, &trasfered, nullptr))
	{
		out = true;
	};

	return out;
}

int vxbox::init(UINT controller_index)
{
	// Test if bus exists
	BOOL bus = this->bus.is_bus_installed();
	if (bus) {
		// std::cout << "Found SCP bus driver!" << std::endl;
    }
	else {
		// std::cout << "Can't find SCP bus driver!" << std::endl;
		return -1;
	}

	// Connect to a controller
	plugin(controller_index);

	this->set_dpad_off(controller_index);
	this->set_button_A(controller_index, FALSE);
	this->set_button_B(controller_index, FALSE);
	this->set_button_X(controller_index, FALSE);
	this->set_button_Y(controller_index, FALSE);
	this->set_button_LT(controller_index, FALSE);
	this->set_button_RT(controller_index, FALSE);
	this->set_button_LB(controller_index, FALSE);
	this->set_button_RB(controller_index, FALSE);
	this->set_button_Start(controller_index, FALSE);
	this->set_button_Back(controller_index, FALSE);

	return 1;
}

bool vxbox::set_button_A(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_A;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_B(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_B;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_X(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_X;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_Y(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_Y;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_Start(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_START;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_Back(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_BACK;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_LT(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_LEFT_THUMB;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_RT(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_RIGHT_THUMB;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_LB(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_LEFT_SHOULDER;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_button_RB(UINT controller_index, BOOL press)
{
	UINT Btn = XINPUT_GAMEPAD_RIGHT_SHOULDER;
	this->bus.g_Gamepad[controller_index - 1].wButtons &= ~Btn;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= Btn * press;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_dpad(UINT controller_index, INT value)
{
	this->bus.g_Gamepad[controller_index - 1].wButtons &= 0xFFF0;
	this->bus.g_Gamepad[controller_index - 1].wButtons |= value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
}

bool vxbox::set_dpad_up(UINT controller_index)
{
	return set_dpad(controller_index, DPAD_UP);
}

bool vxbox::set_dpad_right(UINT controller_index)
{
	return set_dpad(controller_index, DPAD_RIGHT);
}

bool vxbox::set_dpad_down(UINT controller_index)
{
	return set_dpad(controller_index, DPAD_DOWN);
}

bool vxbox::set_dpad_left(UINT controller_index)
{
	return set_dpad(controller_index, DPAD_LEFT);
}

bool vxbox::set_dpad_off(UINT controller_index)
{
	return set_dpad(controller_index, DPAD_OFF);
}

bool vxbox::set_trigger_right(UINT controller_index, BYTE value)
{
	this->bus.g_Gamepad[controller_index - 1].bRightTrigger = value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
};

bool vxbox::set_trigger_left(UINT controller_index, BYTE value)
{
	this->bus.g_Gamepad[controller_index - 1].bLeftTrigger = value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
};

bool vxbox::set_axis_x_left(UINT controller_index, SHORT value)
{
	this->bus.g_Gamepad[controller_index - 1].sThumbLX = value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
};

bool vxbox::set_axis_x_right(UINT controller_index, SHORT value)
{
	this->bus.g_Gamepad[controller_index - 1].sThumbRX = value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
};

bool vxbox::set_axis_y_left(UINT controller_index, SHORT value)
{
	this->bus.g_Gamepad[controller_index - 1].sThumbLY = value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
};

bool vxbox::set_axis_y_right(UINT controller_index, SHORT value)
{
	this->bus.g_Gamepad[controller_index - 1].sThumbRY = value;
	return this->bus.set_xoutput_state(controller_index, &this->bus.g_Gamepad[controller_index - 1]);
};

bool vxbox::set_button_off(UINT controller_index)
{
	this->set_button_A(controller_index, FALSE);
	this->set_button_B(controller_index, FALSE);
	this->set_button_X(controller_index, FALSE);
	this->set_button_Y(controller_index, FALSE);
	this->set_button_Start(controller_index, FALSE);
	this->set_button_Back(controller_index, FALSE);
	this->set_button_LT(controller_index, FALSE);
	this->set_button_RT(controller_index, FALSE);
	this->set_button_LB(controller_index, FALSE);
	this->set_button_RB(controller_index, FALSE);
	this->set_dpad_off(controller_index);
	this->set_trigger_left(controller_index, 0);
	this->set_trigger_right(controller_index, 0);
	this->set_axis_x_left(controller_index, 0);
	this->set_axis_y_left(controller_index, 0);
	this->set_axis_x_right(controller_index, 0);
	this->set_axis_y_right(controller_index, 0);
	return true;
}