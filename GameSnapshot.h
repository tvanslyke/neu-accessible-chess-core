#ifndef AC_GAME_SNAPSHOT_H
#define AC_GAME_SNAPSHOT_H

#include "Board.h"
#include "ChessPiece.h"
#include <limits>

namespace ac {

struct GameSnapshot {
	Board board;
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

	std::string fenstring() const {
		auto fenstr = board.fenstring();
		fenstr.append(active_color == ChessPieceColor::Black ? " b" : " w");
		fenstr.push_back(' ');
		fenstr.append(forsyth_edwards_encoding(castle_status));
		fenstr.push_back(' ');
		if(en_passant_possible) {
			fenstr.append(name(en_passant_target));
		} else {
			fenstr.push_back(' ');
		}
		auto tens_dig = halfmove_clock / 10u;
		auto ones_dig = halfmove_clock - tens_dig * 10u;
		if(tens_dig != 0u) {
			fenstr.push_back(static_cast<char>('0' + tens_dig));
		}
		fenstr.push_back(static_cast<char>('0' + ones_dig));
		fenstr.push_back(' ');
		char buff[std::numeric_limits<std::size_t>::digits10 + 2u];
		int err = std::snprintf(buff, sizeof(buff), "%zu", fullmove_number);
		assert(err >= 0u);
		fenstr.append(buff);
		return fenstr;
	}
};

} /* namespace ac */

#endif /* AC_GAME_SNAPSHOT_H */
