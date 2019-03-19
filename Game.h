#ifndef AC_GAME_H
#define AC_GAME_H

#include "ChessPiece.h"

namespace ac {

struct Game {
	Game() = default;

	ChessPieceColor active_color() const {
		return game_state_.active_color;
	}

	
private:
	Board board_;
	TemporalGameState game_state_;
	GameHistory history_;
};

} /* namespace ac */

#endif /* AC_GAME_H */
