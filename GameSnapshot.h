#ifndef AC_GAME_SNAPSHOT_H
#define AC_GAME_SNAPSHOT_H

#include "Board.h"
#include "ChessPiece.h"
#include <limits>

namespace ac {

struct GameSnapshot {
	CompressedBoard board;
	// Color of player whose turn it is.
	ChessPieceColor active_color : 1u;
	// Who can castle.
	CastleStatus castle_status   : 4u; /* Ignore gcc's warning about this bit field being too small */
	// True if en_passant_target is valid. (if this is false, there is no en-passant target).
	bool en_passant_possible     : 1u;
	// The current en passant target, if any.
	BoardPos en_passant_target   : 6u; /* Ignore gcc's warning about this bit field being too small */
	// Number of moves since the last piece capture (for the 50 move draw rule).
	std::size_t halfmove_clock   : 6u;
	// Total number of fullmoves.
	std::size_t fullmove_number;
};


std::string forsyth_edwards_encoding(const GameSnapshot& snapshot) const {
	return fmt::format(
		"{} {} {} {} {} {}",
		forsyth_edwards_encoding(snapshot.board),
		snapshot.active_color == ChessPieceColor::Black ? 'b' : 'w',
		forsyth_edwards_encoding(snapshot.castle_status),
		snapshot.en_passant_possible ? name(snapshot.en_passant_target) : "-",
		snapshot.halfmove_clock,
		snapshot.fullmove_number
	);
}

} /* namespace ac */

#endif /* AC_GAME_SNAPSHOT_H */
