#pragma once

#include <thread>
#include <chrono>

#include "memory_reader.h"
#include "vxbox.h"
#include "libppt_sync.h"

enum game_status {
	IDLE,
	CHOOSING_CHARACTER,
	STARTING,
	RUNNING,
	GAMEOVER
};

class ppt
{
public:
	HWND game_window = NULL;
	HANDLE process_handle = NULL;
	DWORD process_id = (DWORD)NULL;
	uintptr_t base_address = (uintptr_t)NULL;

public:
	memory_reader mem_reader;
	bool ppt_found = false;

public:
	int open_ppt();
	uintptr_t get_player_address(int player_index);

public:
	char index_to_piece(int index);
	char get_current_piece(uintptr_t player_address);
	int get_current_piece_distance_to_ground(uintptr_t player_address);
	char get_hold_piece(uintptr_t player_address);
	int get_b2b(uintptr_t player_address);
	int get_ren(uintptr_t player_address);
	int get_incomming_attack(uintptr_t player_address);
	void get_next_queue(uintptr_t player_address, char queue[5]);
	void get_board(uintptr_t player_address, int board[40][10]);
	int get_current_piece_x(uintptr_t player_address);
	int get_current_piece_rotate_state(uintptr_t player_address);
	char get_character_choosing_data(int index);

public:
	game_status get_game_status(uintptr_t player_address);
	void start_clear_line(int player_index);
	void snapshot_state(int player_index);
	bool is_waiting_state(int player_index);
	bool is_waiting_frame(int frame_count);

public:
	char state_current, state_hold, state_queue[5], state_x, state_rotate;
	int line = 0;
	bool clear_line = false, pressing_hold = false;
public:
	PptSync* pptsync = nullptr;
};

