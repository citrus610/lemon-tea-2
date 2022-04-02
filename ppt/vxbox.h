#pragma once

#include "scpbus.h"

#define DPAD_UP XINPUT_GAMEPAD_DPAD_UP
#define DPAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN
#define DPAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT
#define DPAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define DPAD_OFF 0

class vxbox
{
private:
	scpbus bus;
public:
	bool is_controller_exist(UINT controller_index);
	bool is_controller_owned(UINT controller_index);

	bool plugin(UINT controller_index);
	bool unplug(UINT controller_index);
	bool unplug_all();
public:
	int init(UINT controller_index);
public:
	bool set_button_A(UINT controller_index, BOOL press);
	bool set_button_B(UINT controller_index, BOOL press);
	bool set_button_X(UINT controller_index, BOOL press);
	bool set_button_Y(UINT controller_index, BOOL press);
	bool set_button_Start(UINT controller_index, BOOL press);
	bool set_button_Back(UINT controller_index, BOOL press);
	bool set_button_LT(UINT controller_index, BOOL press);
	bool set_button_RT(UINT controller_index, BOOL press);
	bool set_button_LB(UINT controller_index, BOOL press);
	bool set_button_RB(UINT controller_index, BOOL press);
	bool set_dpad(UINT controller_index, INT value);
	bool set_dpad_up(UINT controller_index);
	bool set_dpad_right(UINT controller_index);
	bool set_dpad_down(UINT controller_index);
	bool set_dpad_left(UINT controller_index);
	bool set_dpad_off(UINT controller_index);
	bool set_trigger_right(UINT controller_index, BYTE value);
	bool set_trigger_left(UINT controller_index, BYTE value);
	bool set_axis_x_left(UINT controller_index, SHORT value);
	bool set_axis_x_right(UINT controller_index, SHORT value);
	bool set_axis_y_left(UINT controller_index, SHORT value);
	bool set_axis_y_right(UINT controller_index, SHORT value);
	bool set_button_off(UINT controller_index);
};

