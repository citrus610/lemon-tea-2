#include <iostream>
#include <cstdint>
#include <array>

#include "../lib/nlohmann/json.hpp"
#include "tbp.h"
#include "../bot/thread.h"

using nlohmann::json;
using tbp::frontend::FrontendMessageKind;
using tbp::bot::BotMessageKind;

template <typename M>
void send_message(BotMessageKind kind, M message)
{
	json messageJson = message;
	messageJson["type"] = kind;
	std::cout << messageJson << std::endl;
};

LemonTea::PieceType convert_piece_tbp_to_lt(tbp::Piece piece)
{
    switch (piece)
    {
    case tbp::Piece::I:
        return LemonTea::PIECE_I;
    case tbp::Piece::J:
        return LemonTea::PIECE_J;
    case tbp::Piece::L:
        return LemonTea::PIECE_L;
    case tbp::Piece::O:
        return LemonTea::PIECE_O;
    case tbp::Piece::T:
        return LemonTea::PIECE_T;
    case tbp::Piece::S:
        return LemonTea::PIECE_S;
    case tbp::Piece::Z:
        return LemonTea::PIECE_Z;
    default:
        return LemonTea::PIECE_NONE;
    }
    return LemonTea::PIECE_NONE;
};

tbp::Piece convert_piece_lt_to_tbp(LemonTea::PieceType piece)
{
    switch (piece)
    {
    case LemonTea::PIECE_I:
        return tbp::Piece::I;
    case LemonTea::PIECE_J:
        return tbp::Piece::J;
    case LemonTea::PIECE_L:
        return tbp::Piece::L;
    case LemonTea::PIECE_O:
        return tbp::Piece::O;
    case LemonTea::PIECE_T:
        return tbp::Piece::T;
    case LemonTea::PIECE_S:
        return tbp::Piece::S;
    case LemonTea::PIECE_Z:
        return tbp::Piece::Z;
    }
    return tbp::Piece::I;
};

LemonTea::PieceRotation convert_rotation_tbp_to_lt(tbp::Orientation rotation)
{
    switch (rotation)
    {
    case tbp::Orientation::North:
        return LemonTea::PIECE_UP;
    case tbp::Orientation::East:
        return LemonTea::PIECE_RIGHT;
    case tbp::Orientation::South:
        return LemonTea::PIECE_DOWN;
    case tbp::Orientation::West:
        return LemonTea::PIECE_LEFT;
    }
    return LemonTea::PIECE_UP;
};

tbp::Orientation convert_rotation_lt_to_tbp(LemonTea::PieceRotation rotation)
{
    switch (rotation)
    {
    case LemonTea::PIECE_UP:
        return tbp::Orientation::North;
    case LemonTea::PIECE_RIGHT:
        return tbp::Orientation::East;
    case LemonTea::PIECE_DOWN:
        return tbp::Orientation::South;
    case LemonTea::PIECE_LEFT:
        return tbp::Orientation::West;
    }
    return tbp::Orientation::North;
};

int main()
{
    tbp::bot::Info info
    {
        .name = "Lemon Tea",
        .version = "v2.0",
        .author = "citrus610",
        .features {}
    };
    send_message(BotMessageKind::Info, info);

    LemonTea::Thread thread;
    std::vector<LemonTea::PieceType> new_piece;

    while (true)
    {
        json message;
        std::cin >> message;

        switch (message["type"].get<FrontendMessageKind>())
        {

        case FrontendMessageKind::Rules:
        {
            send_message(BotMessageKind::Ready, tbp::bot::Ready());
            break;
        }

        case FrontendMessageKind::Start:
        {
            thread.stop();
            
            new_piece.clear();

            auto start_message = message.get<tbp::frontend::Start>();

            // Set queue
            std::vector<LemonTea::PieceType> queue;
            queue.clear();
            for (auto piece : start_message.queue) {
                queue.push_back(convert_piece_tbp_to_lt(piece));
            }

            // Set board
            LemonTea::Board board;
            for (size_t y = 0; y < start_message.board.size(); y++) {
                auto& row = start_message.board[y];
                for (size_t x = 0; x < row.size(); x++) {
                    if (row[x].has_value()) {
                        board[x] |= 1ULL << y;
                    }
                }
            }

            // Set hold
            LemonTea::PieceType hold = LemonTea::PIECE_NONE;
            if (auto message_hold = start_message.hold) {
                hold = convert_piece_tbp_to_lt(*message_hold);
            }

            // Set bag
            LemonTea::Bag bag;
            memset(bag.data, false, 7);
            for (auto piece : start_message.randomizer.bag_state) {
                bag.data[convert_piece_tbp_to_lt(piece)] = true;
            }
            for (int i = int(queue.size()) - 1; i >=0; --i) {
                bag.deupdate(queue[i]);
            }

            // Set b2b
            int b2b = start_message.back_to_back;

            // Set ren
            int ren = start_message.combo + 1;

            // std::cerr << "[DBG] started!\n";
            thread.start(board, hold, queue, bag, b2b, ren, {LemonTea::DEFAULT_HEURISTIC()});
            // std::cerr << "thread started!\n";

            break;
        }

        case FrontendMessageKind::Suggest:
        {
            LemonTea::Plan plan;
            // std::cerr << "Getting suggestion...\n";
            if (!thread.request(0, plan)) {
                // std::cerr << "Error getting suggestion.\n";
                thread.stop();
                return 0;
            }
            // std::cerr << "Got suggestion.\n" << "\n";

            tbp::bot::Suggestion suggestion;

            std::array spins
            {
                tbp::Spin::Full,
                tbp::Spin::None,
                tbp::Spin::Mini
            };

            for (auto spin : spins) {
                suggestion.moves.push_back
                ({
                    .location = {
                        .type = convert_piece_lt_to_tbp(plan.placement.type),
                        .orientation = convert_rotation_lt_to_tbp(plan.placement.rotation),
                        .x = plan.placement.x,
                        .y = plan.placement.y
                    },
                    .spin = spin
                });
            }

            // std::cerr << "Sending suggestion.\n";
            send_message(BotMessageKind::Suggestion, suggestion);
            // std::cerr << "Sent suggestion.\n" << "\n";

            break;
        }

        case FrontendMessageKind::Play:
        {
            auto play_message = message.get<tbp::frontend::Play>();

            LemonTea::Piece piece = LemonTea::Piece
            (
                int8_t(play_message.move.location.x),
                int8_t(play_message.move.location.y),
                convert_piece_tbp_to_lt(play_message.move.location.type),
                convert_rotation_tbp_to_lt(play_message.move.location.orientation)
            );

            // std::cerr << "Advancing state...\n";
            if (!thread.advance(piece, new_piece)) {
                // std::cerr << "Error advancing state...\n";
                thread.stop();
                return 0;
            }

            // std::cerr << "Advanced state...\n" << "\n";
            new_piece.clear();

            break;
        }

        case FrontendMessageKind::NewPiece: 
        {
            auto new_piece_message = message.get<tbp::frontend::NewPiece>();
            new_piece.push_back(convert_piece_tbp_to_lt(new_piece_message.piece));
            break;
        }

        case FrontendMessageKind::Stop:
        {
            thread.stop();
            // std::cerr << "Stop lemon tea...\n";
            break;
        }
        
        case FrontendMessageKind::Quit: {
            return 0;
        }

        default:
            break;
        }
    }

    return 0;
};