#include "ppt.h"

int ppt::open_ppt()
{
	WCHAR PROCESS_NAME[19] = { L'p', L'u', L'y', L'o', L'p', L'u', L'y', L'o', L't', L'e', L't', L'r', L'i', L's', L'.', L'e', L'x', L'e'};
	WCHAR WINDOW_NAME[15] = { L'P', L'u', L'y', L'o', L'P', L'u', L'y', L'o', L'T', L'e', L't', L'r', L'i', L's' };

	this->game_window = FindWindow(NULL, WINDOW_NAME);
	if (this->game_window == NULL) {
		///std::cout << " Can't found ppt" << std::endl;
		this->ppt_found = false;

		// destroy ppt sync
		if (this->pptsync != nullptr) {
			pptsync_destroy(this->pptsync);
			this->pptsync = nullptr;
			std::cout << "destroyed ppt sync!" << std::endl;
		}

		return -1;
	}
	//std::cout << "Found ppt" << std::endl;

	if (!ppt_found) {
		GetWindowThreadProcessId(this->game_window, &this->process_id);
		this->process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, this->process_id);
		this->base_address = this->mem_reader.get_module_base(this->process_id, PROCESS_NAME);
	}
	std::cout << "Base address for ppt is " << std::hex << this->base_address << std::endl;

	ppt_found = true;

	// start ppt sync
	if (this->pptsync == nullptr) {
		this->pptsync = pptsync_new();
		std::cout << "started ppt sync!" << std::endl;
	}

	return 1;
}

uintptr_t ppt::get_player_address(int player_index)
{
	uintptr_t prt_offsets[1] = { 0x378 + 0x8 * (uintptr_t)player_index };
	return this->mem_reader.get_pointer_address(this->process_handle, 0x140461B20, prt_offsets, 1);
}

char ppt::index_to_piece(int index)
{
	switch (index)
	{
	case 0:
		return 'S';
		break;
	case 1:
		return 'Z';
		break;
	case 2:
		return 'J';
		break;
	case 3:
		return 'L';
		break;
	case 4:
		return 'T';
		break;
	case 5:
		return 'O';
		break;
	case 6:
		return 'I';
		break;
	default:
		return ' ';
		break;
	}
	return ' ';
}

char ppt::get_current_piece(uintptr_t player_address)
{
	uintptr_t offsets[3] = { 0xA8, 0x3C8, 0x8 };
	uintptr_t current_piece_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);
	return index_to_piece(mem_reader.read_data<int>(this->process_handle, current_piece_address));
}

int ppt::get_current_piece_distance_to_ground(uintptr_t player_address)
{
	uintptr_t offsets[3] = { 0xA8, 0x3C8, 0x14 };
	uintptr_t current_piece_distance_to_ground_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);
	return mem_reader.read_data<int>(this->process_handle, current_piece_distance_to_ground_address);
}

char ppt::get_hold_piece(uintptr_t player_address)
{
	uintptr_t offsets[3] = { 0xA8, 0x3D0, 0x8 };
	uintptr_t hold_piece_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);
	return index_to_piece(mem_reader.read_data<int>(this->process_handle, hold_piece_address));
}

int ppt::get_b2b(uintptr_t player_address)
{
	uintptr_t offsets[2] = { 0xA8, 0x3DD };
	uintptr_t b2b_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 2);
	return (int)mem_reader.read_data<char>(this->process_handle, b2b_address);
}

int ppt::get_ren(uintptr_t player_address)
{
	uintptr_t offsets[2] = { 0xA8, 0x3DC };
	uintptr_t ren_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 2);
	return (int)mem_reader.read_data<char>(this->process_handle, ren_address);
}

int ppt::get_incomming_attack(uintptr_t player_address)
{
	uintptr_t offsets[3] = { 0xA8, 0xD0, 0x60 };
	uintptr_t garbage_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);
	return (int)mem_reader.read_data<int>(this->process_handle, garbage_address);
}

void ppt::get_next_queue(uintptr_t player_address, char queue[5])
{
	uintptr_t offsets[2] = { 0xB8, 0x15C };
	uintptr_t next_queue_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 2);

	char buffer[40];
	BOOL success = ReadProcessMemory(process_handle, (LPCVOID)next_queue_address, buffer, 40, NULL);
	if (success == 0) return;
	for (int i = 0; i < 5; ++i) {
		queue[i] = index_to_piece(buffer[i * 4]);
	}
}

void ppt::get_board(uintptr_t player_address, int board[40][10])
{
	uintptr_t offsets[3] = { 0xA8, 0x3C0, 0x50 };
	uintptr_t board_address = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);

	for (int i = 0; i < 10; ++i) {
		char buffer[160];
		uintptr_t columm_add = mem_reader.read_data<uintptr_t>(this->process_handle, board_address + (uintptr_t)i * (uintptr_t)0x8);
		BOOL success = ReadProcessMemory(process_handle, (LPCVOID)columm_add, buffer, 160, NULL);
		if (success == 0) return;
		for (int k = 0; k < 40; ++k) {
			board[39 - k][i] = buffer[k * 4];
		}
	}
}

int ppt::get_current_piece_x(uintptr_t player_address)
{
	uintptr_t offsets[3] = { 0xA8, 0x3C8, 0xC };
	uintptr_t add = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);
	return this->mem_reader.read_data<int>(this->process_handle, add);
}

int ppt::get_current_piece_rotate_state(uintptr_t player_address)
{
	uintptr_t offsets[3] = { 0xA8, 0x3C8, 0x18 };
	uintptr_t add = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 3);
	return this->mem_reader.read_data<int>(this->process_handle, add);
}

char ppt::get_character_choosing_data(int index)
{
	uintptr_t c_offsets[1] = { 0x1C8 + (uintptr_t)index * (uintptr_t)0x30 };
	uintptr_t character_ptr = this->mem_reader.get_pointer_address(this->process_handle, 0x140460690, c_offsets, 1);
	return this->mem_reader.read_data<char>(this->process_handle, character_ptr);
}

game_status ppt::get_game_status(uintptr_t player_address)
{
	uintptr_t offsets[2] = { 0xA8, 0x80 };
	uintptr_t game_status_ptr = this->mem_reader.get_pointer_address(this->process_handle, player_address, offsets, 2);
	int raw_game_status = this->mem_reader.read_data<int>(this->process_handle, game_status_ptr);
	char in_match_flag = this->mem_reader.read_data<char>(this->process_handle, this->base_address + 0x463F37);
	switch (raw_game_status)
	{
	case 0:
		if (in_match_flag == 1) {
			return STARTING;
		}
		break;
	case 1:
		return RUNNING;
		break;
	case 8:
		return GAMEOVER;
		break;
	case 9:
		return GAMEOVER;
		break;
	case 12:
		return GAMEOVER;
		break;
	default:
		break;
	}
	
	// character
	uintptr_t c_offsets[1] = { 0x274 };
	uintptr_t character_ptr = this->mem_reader.get_pointer_address(this->process_handle, 0x140460690, c_offsets, 1);
	char raw_character_status = this->mem_reader.read_data<char>(this->process_handle, character_ptr);
	if (raw_character_status == 1 || raw_character_status == 2) {
		return CHOOSING_CHARACTER;
	}
	return IDLE;
}

void ppt::start_clear_line(int player_index)
{
	this->clear_line = true;
	//this->snapshot_state(player_index);
}

void ppt::snapshot_state(int player_index)
{
	uintptr_t player_address = this->get_player_address(player_index);

	this->state_current = this->get_current_piece(player_address);
	this->state_hold = this->get_hold_piece(player_address);
	this->get_next_queue(player_address, this->state_queue);
}

bool ppt::is_waiting_state(int player_index)
{
	if (!this->clear_line) {
		return false;
	}

	uintptr_t player_address = this->get_player_address(player_index);

	char cur_queue[5];
	this->get_next_queue(player_address, cur_queue);
	if (
		this->state_queue[0] != cur_queue[0] ||
		this->state_queue[1] != cur_queue[1] ||
		this->state_queue[2] != cur_queue[2] ||
		this->state_queue[3] != cur_queue[3] ||
		this->state_queue[4] != cur_queue[4]
		) {
		this->clear_line = false;
		return false;
	}

	//std::cout << "waiting state" << std::endl;
	return true;
}

bool ppt::is_waiting_frame(int frame_count)
{
	int current_frame = this->mem_reader.read_data<int>(this->process_handle, 0x140461B7C);

	//if (std::abs(this->frame - current_frame) < frame_count) {
	//	return true;
	//}

	//this->frame = current_frame;
	return false;
}
