#include "app.h"

app::app() {
    sAppName = "Lemon Tea v2.0";
}

bool app::OnUserCreate()
{
    // Game pad
    vxbox_connect = false;
    // vxbox_index = (UINT)2;
    // this->vxbox.init(vxbox_index);

    // Init bot
    create_json();
    std::ifstream file;
    file.open("config.json");
    json js;
    file >> js;
    set_from_json(js, bot_w, bot_speed, bot_preview, bot_forecast);
    file.close();

    // Init ppt
    ppt.ppt_found = false;

    // PPT
    this->ppt_player_index = 0;
    this->bot_forecast = false;
    this->bot_preview = 5;
    this->bot_input_delay_frame = 0;

    // GUI
    int GUI_y_offset = 4;
    this->tkx_gamepad_connect = Tickbox(125, 70 - GUI_y_offset, 16);
    this->sld_gamepad_index = Slider(125, 45 - GUI_y_offset, 85, 16, 1, 4);
    this->sld_player_index = Slider(125, 160 - GUI_y_offset, 85, 16, 1, 4);
    this->sld_lt_speed = Slider(125, 225 - GUI_y_offset, 85, 16, 1, 10);
    this->sld_lt_preview = Slider(125, 250 - GUI_y_offset, 85, 16, 1, 5);

    this->tkx_gamepad_connect.set_on(false);
    this->sld_gamepad_index.set_value(1);
    this->sld_lt_speed.set_value(10);
    this->sld_lt_preview.set_value(5);

    return true;
}

bool app::OnUserUpdate(float dt)
{
    // Render clear
    this->Clear(olc::Pixel(0, 0, 0, 255));

    // Gamepad
    if (this->tkx_gamepad_connect.is_just_on() && vxbox_connect == false) {
        vxbox_index = (UINT)this->sld_gamepad_index.get_value();
        vxbox_connect = true;
        this->vxbox.init(vxbox_index);
    }
    if (this->tkx_gamepad_connect.is_just_off() && vxbox_connect == true) {
        vxbox_connect = false;
        this->vxbox.unplug(vxbox_index);
    }
    if (vxbox_connect) {
        this->sld_gamepad_index.set_enable(false);
    }
    else {
        this->sld_gamepad_index.set_enable(true);
    }

    // Puyo Puyo player index
    this->ppt_player_index = this->sld_player_index.get_value() - 1;

    // Lemon Tea setting
    this->bot_preview = this->sld_lt_preview.get_value();
    this->bot_input_delay_frame = 10 / this->sld_lt_speed.get_value() - 1;

    // Main loop
    if (!ppt.ppt_found) {
        ppt.open_ppt();
        this->end_bot();
        this->status_raw_str.clear();
        this->status_raw_str.append("INACTIVE");
    }
    else {
        bool ppt_ok = pptsync_wait_for_frame(this->ppt.pptsync);
        if (!ppt_ok) {
            ppt.ppt_found = false;
            pptsync_destroy(ppt.pptsync);
            ppt.pptsync = nullptr;
            return true;
        }

        // Prevent online
        // int online_flag = ppt.mem_reader.read_data<int>(this->ppt.process_handle, this->ppt.base_address + 0x59894C);
        // if (online_flag == 1) this->end();

        uintptr_t player_address = ppt.get_player_address(ppt_player_index);
        game_status ppt_status = ppt.get_game_status(player_address);

        if (ppt_status == IDLE) {
            ++this->idle_counter;
            this->end_bot();
            if (this->idle_counter == 2) {
                this->vxbox.set_button_off(this->vxbox_index);
            }
            this->status_raw_str.clear();
            this->status_raw_str.append("IDLE");
        }
        else {
            this->idle_counter = 0;
        }

        // choosing character
        if (ppt_status == CHOOSING_CHARACTER) {
            this->choosing_character_target = 9;
            if (this->choosing_character_input_pressed == false) {
                int current_char = (int)ppt.get_character_choosing_data(ppt_player_index);
                if (current_char != this->choosing_character_target) {
                    this->choosing_character_move_right_char = current_char;
                    if (this->choosing_character_move_right_count >= 5) {
                        this->vxbox.set_dpad_down(this->vxbox_index);
                        this->choosing_character_move_right_count = -1;
                    }
                    else
                    {
                        this->vxbox.set_dpad_right(this->vxbox_index);
                    }
                    this->choosing_character_ok_count = 0;
                }
                else
                {
                    this->vxbox.set_button_A(this->vxbox_index, TRUE);
                    ++this->choosing_character_ok_count;
                }
                this->choosing_character_input_pressed = true;
            }
            else
            {
                if (this->choosing_character_ok_count > 0) {
                    this->vxbox.set_button_A(this->vxbox_index, FALSE);
                    this->choosing_character_input_pressed = false;
                }
                else {
                    this->vxbox.set_dpad_off(this->vxbox_index);
                    this->choosing_character_input_pressed = false;
                    ++this->choosing_character_move_right_count;
                }
            }
            ++this->choosing_character_counter;
            this->status_raw_str.clear();
            this->status_raw_str.append("CHARACTER");
        }
        else {
            this->choosing_character_counter = 0;
            this->choosing_character_move_right_count = 0;
            this->choosing_character_input_pressed = false;
            this->choosing_character_ok_count = 0;
        }

        // starting
        if (ppt_status == STARTING) {
            ++this->starting_counter;
            if (this->starting_counter == 60) {
                bot_input_delay_frame_counter = 0;
                this->init_bot();
            }
            if (this->starting_counter > 62) {
                this->starting_counter = 62;
            }
            this->status_raw_str.clear();
            this->status_raw_str.append("STARTING");
        }
        else {
            this->starting_counter = 0;
        }

        // running
        if (ppt_status == RUNNING) {
            ++this->running_counter;
            if (this->running_counter >= 6) {
                this->bot_elasped_time += dt;
                if (!this->ppt.is_waiting_state(this->ppt_player_index)) {
                    if (this->bot.is_running()) this->update_bot();
                }
                else {
                    // Turn off all inputs
                    this->vxbox.set_dpad_off(this->vxbox_index);
                    this->vxbox.set_button_A(this->vxbox_index, FALSE);
                    this->vxbox.set_button_B(this->vxbox_index, FALSE);
                    this->vxbox.set_button_LB(this->vxbox_index, FALSE);
                }
            }
            this->running_counter = std::min(this->running_counter, 16);
            this->status_raw_str.clear();
            this->status_raw_str.append("RUNNING");
            this->DrawString(40, 300, std::string("NODE"));
            this->DrawString(40, 325, std::to_string(this->bot_node));
            this->DrawString(40, 350, std::string("DEPTH"));
            this->DrawString(40, 375, std::to_string(this->bot_depth));
            // this->DrawString(40, 350, std::string("TIME  ") + std::to_string(this->bot_time) + std::string(" ms"));
            // this->DrawString(40, 375, std::string("NPS   ") + std::to_string(this->bot_nps) + std::string(" kn/s"));
        }
        else {
            this->running_counter = 0;
            this->bot_elasped_time = 0.0;
            this->bot_node = 0;
            this->bot_depth = 0;
            this->bot_time = 0;
            this->bot_nps = 0;
        }

        // gameover
        if (ppt_status == GAMEOVER) {
            ++this->gameover_counter;
            this->end_bot();
            if (this->gameover_counter == 2) {
                this->vxbox.set_button_off(this->vxbox_index);
            }
            this->status_raw_str.clear();
            this->status_raw_str.append("GAMEOVER");
        }
        else {
            this->gameover_counter = 0;
        }
    }

    // GUI
    this->tkx_gamepad_connect.update(this, dt);
    this->sld_gamepad_index.update(this, dt);
    this->sld_player_index.update(this, dt);
    this->sld_lt_speed.update(this, dt);
    this->sld_lt_preview.update(this, dt);

    this->DrawString(20, 20, std::string("GAME PAD"));
    this->DrawString(20, 110, std::string("PUYO PUYO TETRIS"));
    this->DrawString(20, 200, std::string("LEMON TEA"));
    this->DrawString(18, 460, std::string("Lemon Tea v2.0 by citrus610"));
    
    this->DrawString(40, 45, std::string("INDEX"));
    this->DrawString(40, 70, std::string("CONNECT"));
    this->DrawString(40, 135, std::string("STATUS"));
    this->DrawString(40, 160, std::string("PLAYER"));
    this->DrawString(40, 225, std::string("SPEED"));
    this->DrawString(40, 250, std::string("PREVIEW"));

    this->DrawString(125, 135, this->status_raw_str);
    this->DrawString(125 + 85 + 8, 45, std::to_string(this->sld_gamepad_index.get_value()));
    this->DrawString(125 + 85 + 8, 160, std::to_string(this->sld_player_index.get_value()));
    this->DrawString(125 + 85 + 8, 225, std::to_string(this->sld_lt_speed.get_value()));
    this->DrawString(125 + 85 + 8, 250, std::to_string(this->sld_lt_preview.get_value()));

    this->DrawRect(20, 280, 210, 145);

    return true;
}

bool app::OnUserDestroy()
{
    if (vxbox_connect) {
        this->vxbox.unplug(vxbox_index);
    }
    this->end_bot();
    if (this->ppt.pptsync != nullptr) {
        pptsync_destroy(this->ppt.pptsync);
    }

    return true;
}

void app::init_bot()
{
    this->bot_input_queue.clear();
    BotState init_state = ppt_to_bot_data(this->ppt_player_index);
    this->bot.start(LemonTea::Board(), LemonTea::PIECE_NONE, init_state.queue, LemonTea::Bag(), 0, 0, {this->bot_w});

    this->ppt.clear_line = false;
    this->ppt.pressing_hold = false;
    this->bot_input_pressed = false;
    this->bot_action_hold = false;
    this->ppt.state_current = ' ';
    this->ppt.state_hold = ' ';

    // Turn off all inputs
    this->vxbox.set_dpad_off(this->vxbox_index);
    this->vxbox.set_button_A(this->vxbox_index, FALSE);
    this->vxbox.set_button_B(this->vxbox_index, FALSE);
    this->vxbox.set_button_LB(this->vxbox_index, FALSE);
}

void app::update_bot()
{
    if (bot_input_queue.empty()) {

        uintptr_t player_address = this->ppt.get_player_address(this->ppt_player_index);

        if (!this->ppt.pressing_hold) {

            if (!this->bot.request(this->ppt.get_incomming_attack(player_address), this->temporary_bot_solution)) {
                // If there haven't been any solutions yet, the skip frame
                return;
            }
            bot_input_queue.clear();

            // Check bot death
            if (this->temporary_bot_solution.placement.type == LemonTea::PIECE_NONE) {
                this->end_bot();
                return;
            }

            // Set bot log
            bot_node = this->temporary_bot_solution.node;
            bot_depth = this->temporary_bot_solution.depth;
            bot_time = int(this->bot_elasped_time * 1000.0);
            if (bot_time == 0) {
                bot_nps = 0;
            }
            else {
                bot_nps = bot_node / bot_time;
            }
            this->bot_elasped_time = 0.0;

            // Check if misdrop
            BotState current_state = this->ppt_to_bot_data(this->ppt_player_index);
            if (!(current_state.board == temporary_bot_solution.root.board)) {
                this->bot.reset(current_state.board, current_state.b2b, current_state.ren);
                return;
            }

            this->ppt.state_hold = this->ppt.get_hold_piece(player_address);

			this->bot_action_hold = this->temporary_bot_solution.placement.type != current_state.current;

            if (this->bot_action_hold) {
                this->vxbox.set_button_LB(this->vxbox_index, TRUE);
                this->ppt.pressing_hold = true;
            }
        }

        if (this->ppt.pressing_hold) {
            if (this->ppt.state_hold != this->ppt.get_hold_piece(player_address)) {
                if (this->ppt.get_current_piece(player_address) != ' ') {
                    this->vxbox.set_button_LB(this->vxbox_index, FALSE);
                    this->ppt.pressing_hold = false;
                }
            }
        }

        if (!this->ppt.pressing_hold) {

            // Push moves to bot input vec
            //     Add hard drop move at the end of the queue
            bot_input_queue.clear();
            if (!temporary_bot_solution.move.empty() && temporary_bot_solution.move.back() != LemonTea::MOVE_DOWN) {
                temporary_bot_solution.move.push_back(LemonTea::MOVE_DOWN);
            }
            LemonTea::convert_move_to_input(temporary_bot_solution.move, bot_input_queue);
            bot_input_queue.push_back(LemonTea::INPUT_NONE);

            // Predict next board
            bool first_hold = (this->ppt.state_hold == ' ') && (this->bot_action_hold);
            char temp_queue[5];
            this->ppt.get_next_queue(player_address, temp_queue);

			std::vector<LemonTea::PieceType> new_piece;

            if (first_hold) {
                new_piece.push_back(this->char_to_piece(temp_queue[3]));
                new_piece.push_back(this->char_to_piece(temp_queue[4]));
			}
			else {
				new_piece.push_back(this->char_to_piece(temp_queue[4]));
			}

            this->bot.advance(temporary_bot_solution.placement, new_piece);

            this->bot_input_pressed = false;
            this->bot_action_hold = false;

            // If first hold then skip 1 frame
            if (first_hold) return;
        }
    }

    if (!bot_input_queue.empty()) {

        ++bot_input_delay_frame_counter;
        if (bot_input_delay_frame_counter > bot_input_delay_frame) {
            bot_input_delay_frame_counter = 0;

            uintptr_t player_address = this->ppt.get_player_address(this->ppt_player_index);

            // Turn off all inputs
            this->vxbox.set_dpad_off(this->vxbox_index);
            this->vxbox.set_button_A(this->vxbox_index, FALSE);
            this->vxbox.set_button_B(this->vxbox_index, FALSE);
            this->vxbox.set_button_LB(this->vxbox_index, FALSE);

            switch (bot_input_queue[0])
            {

            case LemonTea::INPUT_RIGHT:
                this->vxbox.set_dpad_right(this->vxbox_index);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                break;

            case LemonTea::INPUT_LEFT:
                this->vxbox.set_dpad_left(this->vxbox_index);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                break;

            case LemonTea::INPUT_CW:
                this->vxbox.set_button_B(this->vxbox_index, TRUE);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                break;

            case LemonTea::INPUT_CCW:
                this->vxbox.set_button_A(this->vxbox_index, TRUE);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                break;

            case LemonTea::INPUT_DROP:
                this->ppt.snapshot_state(this->ppt_player_index);
                this->vxbox.set_dpad_up(this->vxbox_index);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                this->ppt.start_clear_line(this->ppt_player_index);
                break;

            case LemonTea::INPUT_DROP_CW:
                this->ppt.snapshot_state(this->ppt_player_index);
                this->vxbox.set_button_B(this->vxbox_index, TRUE);
                this->vxbox.set_dpad_up(this->vxbox_index);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                this->ppt.start_clear_line(this->ppt_player_index);
                break;

            case LemonTea::INPUT_DROP_CCW:
                this->ppt.snapshot_state(this->ppt_player_index);
                this->vxbox.set_button_A(this->vxbox_index, TRUE);
                this->vxbox.set_dpad_up(this->vxbox_index);
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                this->ppt.start_clear_line(this->ppt_player_index);
                break;

            case LemonTea::INPUT_DOWN:
                this->vxbox.set_dpad_down(this->vxbox_index);
                if (this->ppt.get_current_piece_distance_to_ground(player_address) == 0) {
                    this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                }
                break;

            case LemonTea::INPUT_NONE:
                this->bot_input_queue.erase(this->bot_input_queue.begin() + 0);
                break;
            default:
                break;
            }
        }
    }
}

void app::end_bot()
{
    this->bot_input_queue.clear();
    this->vxbox.set_dpad_off(this->vxbox_index);
    this->vxbox.set_button_A(this->vxbox_index, FALSE);
    this->vxbox.set_button_B(this->vxbox_index, FALSE);
    this->vxbox.set_button_LB(this->vxbox_index, FALSE);
    this->bot.stop();
}

BotState app::ppt_to_bot_data(int player_index)
{
    BotState result;

    uintptr_t player_address = this->ppt.get_player_address(player_index);

    char temp_queue[5];
    this->ppt.get_next_queue(player_address, temp_queue);

    // current piece
    result.current = char_to_piece(this->ppt.get_current_piece(player_address));

    // hold piece
    char temp_hold = this->ppt.get_hold_piece(player_address);
    result.hold = char_to_piece(temp_hold);

    // b2b status
    result.b2b = this->ppt.get_b2b(player_address);

    // ren
    result.ren = this->ppt.get_ren(player_address);

    // queue
    for (int i = 0; i < 5; ++i) result.queue.push_back(char_to_piece(temp_queue[i]));

    // board
    int temp_board[40][10];
    this->ppt.get_board(player_address, temp_board);
    for (int i = 0; i < 10; ++i) {
        uint64_t a_column = 0b0;
        for (int k = 0; k < 40; ++k) {
            if ((temp_board[k][i] > -1 && temp_board[k][i] < 7) || temp_board[k][i] == 9) {
                a_column = a_column | (1ULL << (39 - k));
            }
        }
        result.board[i] = a_column;
    }

    return result;
}

LemonTea::PieceType app::char_to_piece(char _char)
{
    switch (_char)
    {
    case 'I':
        return LemonTea::PIECE_I;
        break;
    case 'J':
        return LemonTea::PIECE_J;
        break;
    case 'L':
        return LemonTea::PIECE_L;
        break;
    case 'T':
        return LemonTea::PIECE_T;
        break;
    case 'Z':
        return LemonTea::PIECE_Z;
        break;
    case 'S':
        return LemonTea::PIECE_S;
        break;
    case 'O':
        return LemonTea::PIECE_O;
        break;
    default:
        return LemonTea::PIECE_NONE;
        break;
    }
}
