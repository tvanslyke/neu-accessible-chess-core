#ifndef AC_MOVE_COMPUTATION_H
#define AC_MOVE_COMPUTATION_H

#include "Move.h"
#include "ChessPiece.h"
#include "optional_extra.h"
#include "GameSnaption.h"
#include <boost/coroutine>


namespace ac {

namespace coro = boost::coroutine;

using move_generator = coro::asymmetric_coroutine<Move>;
using move_sink = coro::asymmetric_coroutine<Move>::push_type;

namespace detail {

void compute_pawn_attackers(
	move_sink& sink,
	BoardPos pos,
	CompressedBoard board,
	TemporalGameState state
) {
	auto defender = board[pos];
	if(state.en_passant_possible and en_passant_target_to_capture(state.en_passant_target) == pos) {
		auto capture_pos = en_passant_target_to_capture(state.en_passant_target);
		assert(defender);
		assert(kind(*defender) == ChessPieceKind::Pawn);
		sink(EnPassantMove(PositionalMove{pos, state.en_passant_target}));
	}
	if(row(pos) - 1) {
		if(col(row_before(pos)) - 1) {
			auto p = col_before(row_before(pos));
			auto piece = board[p];
			if(kind(*piece) == ChessPieceKind::WhitePawn) {
				sink(CommonMove(*piece, PositionalMove(p, pos), defender));
			}
		}
		if(col(row_before(pos)) + 1) {
			auto p = col_after(row_before(pos));
			auto piece = board[p];
			if(kind(*piece) == ChessPieceKind::WhitePawn) {
				sink(CommonMove(*piece, PositionalMove(p, pos), defender));
			}
		}
	}
	if(row(pos) + 1) {
		if(col(row_after(pos)) - 1) {
			auto p = col_before(row_after(pos));
			auto piece = board[];
			if(kind(*piece) == ChessPieceKind::BlackPawn) {
				sink(CommonMove(*piece, PositionalMove(p, pos), defender));
			}
		}
		if(col(row_after(pos)) + 1) {
			auto p = col_after(row_after(pos));
			auto piece = board[p];
			if(kind(*piece) == ChessPieceKind::BlackPawn) {
				sink(CommonMove(*piece, PositionalMove(p, pos), defender));
			}
		}
	}
}

void compute_king_attackers(
	move_sink& sink,
	BoardPos pos,
	CompressedBoard board,
	TemporalGameState state
) {
	auto defender = board[pos];
	for(auto p: king_moveset(pos)) {
		auto piece = board[p];
		if(piece and kind(*piece) == ChessPieceKind::King) {
			sink(CommonMove(*piece, PositionalMove(p, pos), defender));
		}
	}
}

void compute_knight_attackers(
	move_sink& sink,
	BoardPos pos,
	CompressedBoard board,
	TemporalGameState state
) {
	auto defender = board[pos];
	for(auto p: knight_moveset(pos)) {
		auto piece = board[p];
		if(piece and kind(piece) == ChessPieceKind::Knight) {
			sink(CommonMove(*piece, PositionalMove(p, pos), defender));
		}
	}
}

void compute_diagonal_attackers(
	move_sink& sink,
	BoardPos pos,
	CompressedBoard board,
	TemporalGameState state
) {
	auto defender = board[pos];
	auto [col, row] = pos;
	auto visit_diagonal = [&](int c_step, int r_step) {
		for(auto [c, r] = std::pair{c + c_step, r + r_step}; c and r; c = *c + c_step, r = *r + r_step) {
			auto p = *c + *r;
			auto piece = board[p];
			if(piece) {
				if(kind(*piece) == ChessPieceKind::Rook or and kind(*piece) == ChessPieceKind::Queen) {
					sink(CommonMove(*piece, PositionalMove(p, pos), defender));
				}
				break;
			}
		}
	};
	visit_diagonal( 1,  1);
	visit_diagonal( 1, -1);
	visit_diagonal(-1,  1);
	visit_diagonal(-1, -1);
}

void compute_rook_attackers(
	move_sink& sink,
	BoardPos pos,
	CompressedBoard board,
	TemporalGameState state
) {
	auto defender = board[pos];
	auto [col, row] = pos;
	auto visit_strip = [&](int c_step, int r_step) {
		for(auto [c, r] = std::pair{c + c_step, r + r_step}; c and r; c = *c + c_step, r = *r + r_step) {
			auto p = *c + *r;
			auto piece = board[p];
			if(piece) {
				if(kind(*piece) == ChessPieceKind::Rook or kind(*piece) == ChessPieceKind::Queen) {
					sink(CommonMove(*piece, PositionalMove(p, pos), defender));
				}
				break;
			}
		}
	};
	visit_strip( 0,  1);
	visit_strip( 0, -1);
	visit_strip( 1,  0);
	visit_strip(-1,  0);
}

} /* namespace detail */


auto compute_attackers(BoardPos pos, CompressedBoard board, TemporalGameState state) 
	-> boost::coroutine::asymmetric_coroutine<Move>
{
	return boost::coroutine::asymmetric_coroutine<Move>([=](auto& sink){
		detail::compute_pawn_attackers(sink, pos, board, state);
		detail::compute_king_attackers(sink, pos, board, state);
		detail::compute_knight_attackers(sink, pos, board, state);
		detail::compute_diagonal_attackers(sink, pos, board, state);
		detail::compute_rook_attackers(sink, pos, board, state);
	});
}

bool has_attackers(BoardPos pos, CompressedBoard board, TemporalGameState state, std::optional<ChessPieceColor> color = std::nullopt) {
	auto attackers = compute_attackers(pos, board, stat);
	if(not color) {
		return std::distance(begin(attackers), end(attackers)) > 0;
	}
	for(auto attack: attackers) {
		if(ac::color(attack.moved_piece()) == *color) {
			return true;
		}
	}
	return false;
}

namespace detail {

void yield_sequence(move_sink& sink, BoardPos pos, ChessPiece attacker, CompressedBoard board, int c_step, int r_step) {
	auto [col, row] = pos;
	for(auto [c, r] = std::pair{col + c_step, row + r_step}; c and r; c = *c + c_step, r = *r + r_step) {
		auto piece = board[*c + *r];
		if(piece and color(*piece) == color(attacker)) {
			return;
		}
		sink(CommonMove{attacker, PositionalMove{pos, *c + *r}, piece});
	}
};

void compute_valid_bishop_moves(move_sink& sink, BoardPos pos, ChessPiece bishop, CompressedBoard board, TemporalGameState state) {
	yield_sequence(sink, pos, bishop, board,  1,  1);
	yield_sequence(sink, pos, bishop, board,  1, -1);
	yield_sequence(sink, pos, bishop, board, -1,  1);
	yield_sequence(sink, pos, bishop, board, -1, -1);
}

void compute_valid_rook_moves(move_sink& sink, BoardPos pos, ChessPiece rook, CompressedBoard board, TemporalGameState state) {
	auto [col, row] = pos;
	yield_sequence(sink, pos, rook, board,  0,  1);
	yield_sequence(sink, pos, rook, board,  0, -1);
	yield_sequence(sink, pos, rook, board,  1,  0);
	yield_sequence(sink, pos, rook, board, -1,  0);
}

void compute_valid_queen_moves(move_sink& sink, BoardPos pos, ChessPiece queen, CompressedBoard board, TemporalGameState state) {
	compute_valid_rook_moves(sink, pos, queen, board, state);
	compute_valid_bishop_moves(sink, pos, queen, board, state);
}

void compute_valid_knight_moves(move_sink& sink, BoardPos pos, ChessPiece knight, CompressedBoard board, TemporalGameState state) {
	auto moves = knight_moveset(pos);
	for(auto p: moves) {
		auto piece = board[p];
		if(color(piece) != color(knight)) {
			sink(CommonMove{knight, PositionalMove{pos, p}, piece});
		}
	}
}

void compute_valid_king_moves(move_sink& sink, BoardPos pos, ChessPiece king, CompressedBoard board, TemporalGameState state) {
	auto moves = king_moveset(pos);
	for(auto p: moves) {
		auto piece = board[p];
		if(color(piece) != color(king)) {
			sink(CommonMove{king, PositionalMove{pos, p}, piece});
		}
	}
}

void compute_valid_pawn_moves(move_sink& sink, BoardPos pos, ChessPiece pawn, CompressedBoard board, TemporalGameState state) {
	PositionSet attacks;
	if(color(pawn) == ChessPieceColor::White) {
		attacks = white_pawn_attack_set(pos);
		if(row(pos) + 1 and not board[row_after(pos)]) {
			sink(CommonMove(pawn, PositionalMove(pos, row_after(pos)), std::nullopt));
			if(row(pos) == 2_row and not board[row_after(row_after(pos))]) {
				sink(CommonMove(pawn, PositionalMove(pos, row_after(row_after(pos))), std::nullopt));
			}
		}
	} else {
		attacks = black_pawn_attack_set(pos);
		if(row(pos) - 1 and not board[row_before(pos)]) {
			sink(CommonMove(pawn, PositionalMove(pos, row_after(pos)), std::nullopt));
			if(row(pos) == 2_row and not board[row_after(row_after(pos))]) {
				sink(CommonMove(pawn, PositionalMove(pos, row_after(row_after(pos))), std::nullopt));
			}
		}
	}
	if(state.en_passant_possible and attacks.contains(state.en_passant_target)) {
		sink(EnPassantMove(PositionalMove(pos, state.en_passant_target)));
	}
	for(auto p: attacks) {
		auto piece = board[p];
		if(piece and color(piece) != color(pawn)) {
			sink(CommonMove(pawn, PositionalMove(pos, p), piece));
		}
	}
	
}

void compute_valid_castle_moves(move_sink& sink, BoardPos pos, ChessPiece king, CompressedBoard board, TemporalGameState state) {
	if(state.castle_status == CastleStatus::None) {
		return;
	}
	// lots of conditions in the below if statements because castling through check is illegal
	if(color(king) == ChessPieceColor::White) {
		constexpr auto rook = ChessPiece::WhiteRook;
		assert(pos == "E1"_pos);
		// remove the king from the board during checking
		board["E1"_pos] = std::nullopt;
		if(
			(state.castle_status & CastleStatus::WhiteKingside)
			and not board["F1"_pos]
			and not board["G1"_pos]
			and not has_attackers("F1"_pos, board, state, ChessPieceColor::Black)
			and not has_attackers("G1"_pos, board, state, ChessPieceColor::Black)
		) {
			assert(board["H1"_pos] == rook);
			sink(CastleMove(CastleKind::WhiteKingside));
		}
		if(
			(state.castle_status & CastleStatus::WhiteQueenside)
			and not board["B1"_pos]
			and not board["C1"_pos]
			and not board["D1"_pos]
			and not has_attackers("B1"_pos, board, state, ChessPieceColor::Black)
			and not has_attackers("C1"_pos, board, state, ChessPieceColor::Black)
			and not has_attackers("D1"_pos, board, state, ChessPieceColor::Black)
		) {
			assert(board["A1"_pos] == rook);
			sink(CastleMove(CastleKind::WhiteQueenside));
		}
	} else {
		constexpr auto rook = ChessPiece::BlackRook;
		assert(pos == "E8"_pos);
		// remove the king from the board during checking
		board["E8"_pos] = std::nullopt;
		if(
			(state.castle_status & CastleStatus::BlackKingside)
			and not board["F8"_pos]
			and not board["G8"_pos]
			and not has_attackers("F8"_pos, board, state, ChessPieceColor::White) 
			and not has_attackers("G8"_pos, board, state, ChessPieceColor::White)
		) {
			assert(board["H8"_pos] == rook);
			sink(CastleMove(CastleKind::BlackKingside));
		}
		if(
			(state.castle_status & CastleStatus::BlackQueenside)
			and not board["B8"_pos]
			and not board["C8"_pos]
			and not board["D8"_pos]
			and not has_attackers("B8"_pos, board, state, ChessPieceColor::White) 
			and not has_attackers("C8"_pos, board, state, ChessPieceColor::White)
			and not has_attackers("D8"_pos, board, state, ChessPieceColor::White)
		) {
			assert(board["A8"_pos] == rook);
			sink(CastleMove(CastleKind::BlackQueenside));
		}
	}
}

move_generator compute_valid_moves_naive(ChessPieceColor color, CompressedBoard board, TemporalGameState state) {
	return move_generator([=](auto& sink) {
		for(auto pos: each_position) {
			auto piece = std::as_const(board)[pos];
			if(ac::color(piece) != color) {
				continue;
			}
			switch(kind(*piece)) {
			case ChessPieceKind::Pawn:
				compute_valid_pawn_moves(sink, pos, *piece, board, state);
				break;
			case ChessPieceKind::Knight:
				compute_valid_knight_moves(sink, pos, *piece, board, state);
				break;
			case ChessPieceKind::Bishop:
				compute_valid_bishop_moves(sink, pos, *piece, board, state);
				break;
			case ChessPieceKind::Rook:
				compute_valid_rook_moves(sink, pos, *piece, board, state);
				break;
			case ChessPieceKind::Queen:
				compute_valid_queen_moves(sink, pos, *piece, board, state);
				break;
			case ChessPieceKind::King:
				compute_valid_king_moves(sink, pos, *piece, board, state);
				compute_valid_castle_moves(sink, pos, *piece, board, state);
				break;
			}
		}
	});

}

} /* namespace detail */

move_generator valid_castle_moves(Color c, CompressedBoard board, TemporalGameState state) {
	return move_generator([=](auto& sink) {
		if(c == ChessPieceColor::White) {
			detail::compute_valid_castle_moves(sink, "E1"_pos, ChessPiece::WhiteKing, board, state);
		} else {
			detail::compute_valid_castle_moves(sink, "E8"_pos, ChessPiece::BlackKing, board, state);
		}
	});
}

move_generator valid_moves(ChessPieceColor color, CompressedBoard board, TemporalGameState state) {
	auto king = color == ChessPieceColor::White
		? ChessPiece::WhiteKing
		: ChessPiece::BlackKing;
	auto king_positions = board.positions(king);
	assert(king_positions.size() == 1u);
	auto king_pos = *king_positions.begin();
	return move_generator([=](auto& sink) {
		auto board_cpy = board;
		// Filter out all moves that put/keep the king in check.
		for(auto move: detail::compute_valid_moves_naive(color, board, state)) {
			auto scratch_board = board_cpy;
			scratch_board.apply_move(move);
			auto new_king_pos = move.moved_piece() == king ? move.end_position() : king_pos;
			auto attackers = compute_attackers(new_king_pos, scratch_board, state);
			auto attacker = std::find_if(begin(attackers), end(attackers), [&](auto mv) -> bool {
				return ac::color(attack.moved_piece()) != color;
			});
			if(attacker == attackers.end()) {
				// nobody attacking the king, move is legal
				sink(move);
			}
		}
	});
}

} /* namespace ac */

#endif /* AC_MOVE_COMPUTATION_H */
