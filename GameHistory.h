#ifndef AC_GAME_HISTORY_H
#define AC_GAME_HISTORY_H

#include "GameSnapshot.h"
#include <vector>

namespace ac {

struct GameHistory {
	
	GameHistory(GameSnapshot start_state):
		game_(start_state)
	{
	
	}

	std::size_t turn_count(std::size_t turn_number) const {
		return moves_.size();
	}

	GameSnapshot turn_snapshot(std::size_t turn_number) const {
		
	}

	ChessPieceColor current_turn() const {
		
	}

	

private:
	GameSnapshot game_;
	std::vector<SimpleMove> moves_;
};

} /* namespace ac */


#endif /* AC_GAME_HISTORY_H */
