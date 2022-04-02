#pragma once

#include "../lib/pge/olcPixelGameEngine.h"
#include "tickbox.h"
#include "slider.h"
#include "ppt.h"
#include "vxbox.h"
#include <fstream>
#include <string>
#include <iomanip>
#include "type.h"

struct BotState
{
	LemonTea::Board board;
    LemonTea::PieceType current = LemonTea::PIECE_NONE;
    LemonTea::PieceType hold = LemonTea::PIECE_NONE;
    std::vector<LemonTea::PieceType> queue;
    int b2b = 0;
    int ren = 0;
};

class app : public olc::PixelGameEngine
{
public:
	app();
public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	bool OnUserDestroy() override;
public:
	ppt ppt;
	int ppt_player_index = 0;
public:
	vxbox vxbox;
	UINT vxbox_index = 1;
	bool vxbox_connect = false;
public:
	Tickbox tkx_gamepad_connect;
	Slider sld_gamepad_index;
	Slider sld_player_index;
	Slider sld_lt_speed;
	Slider sld_lt_preview;
public:
	LemonTea::Thread bot;
	std::vector<LemonTea::InputType> bot_input_queue;
	LemonTea::Plan temporary_bot_solution;
	bool bot_input_pressed = false;
	bool bot_action_hold = false;
	LemonTea::Heuristic bot_w;
	int bot_speed = 100;
	int bot_preview = 4;
	bool bot_forecast = true;
	double bot_elasped_time = 0.0;
	int bot_input_delay_frame = 0;
	int bot_input_delay_frame_counter = 0;
	int bot_node = 0;
	int bot_depth = 0;
	int bot_time = 0;
	int bot_nps = 0;
public:
	int choosing_character_target = 0;
	int choosing_character_move_right_count = 0;
	int choosing_character_move_right_char = 0;
	int choosing_character_ok_count = 0;
	bool choosing_character_input_pressed = false;
public:
	int idle_counter = 0;
	int starting_counter = 0;
	int running_counter = 0;
	int gameover_counter = 0;
	int choosing_character_counter = 0;
	std::string status_raw_str;
public:
	void init_bot();
	void update_bot();
	void end_bot();
public:
	BotState ppt_to_bot_data(int player_index);
	LemonTea::PieceType char_to_piece(char _char);
};

static void create_json() 
{
	// Check if existed?
	std::ifstream f("config.json");
	if (f.good()) {
		return;
	};
	f.close();

	// Setting data
	json js;
	LemonTea::Heuristic heuristic = LemonTea::DEFAULT_HEURISTIC();
    to_json(js, heuristic);

	// Create file
	std::ofstream o("config.json");
	o << std::setw(4) << js << std::endl;
	o.close();
};

static void set_from_json(json& js, LemonTea::Heuristic& heuristic, int& speed, int& preview, bool& forecast) 
{
	from_json(js, heuristic);
};