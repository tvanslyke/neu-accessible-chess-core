#ifndef AC_GAME_SNAPSHOT_H
#define AC_GAME_SNAPSHOT_H

#include "Board.h"
#include "ChessPiece.h"
#include <limits>

namespace ac {

struct TemporalGameState {
	// Color of player whose turn it is.
	ChessPieceColor active_color : 1u;
	// Who can castle.
	CastleStatus castle_status   : 4u; /* Ignore gcc's warning about this bit field being too small */
	// True if en_passant_target is valid. (if this is false, there is no en-passant target).
	bool en_passant_possible     : 1u;
	// The current en passant target, if any.
	BoardPos en_passant_target   : 6u; /* Ignore gcc's warning about this bit field being too small */
	// Number of half-moves since the last piece capture or pawn advance (for the 50 move draw rule).
	std::size_t halfmove_clock   : 6u;
	// Total number of fullmoves.
	std::size_t fullmove_number  : 14u;
};

static_assert(sizeof(TemporalGameState) <= 8u);

struct GameSnapshot {
	CompressedBoard board;
	TemporalGameState temporal_state;
};

inline std::string forsyth_edwards_encoding(const GameSnapshot& snapshot) {
	return fmt::format(
		"{} {} {} {} {} {}",
		forsyth_edwards_encoding(snapshot.board),
		snapshot.temporal_state.active_color == ChessPieceColor::Black ? 'b' : 'w',
		forsyth_edwards_encoding(snapshot.temporal_state.castle_status),
		snapshot.temporal_state.en_passant_possible ? name(snapshot.temporal_state.en_passant_target) : "-",
		snapshot.temporal_state.halfmove_clock,
		snapshot.temporal_state.fullmove_number
	);
}

} /* namespace ac */

#endif /* AC_GAME_SNAPSHOT_H */
