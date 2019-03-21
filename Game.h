#ifndef AC_GAME_H
#define AC_GAME_H

#include "ChessPiece.h"
#include "Move.h"
#include <vector>

namespace ac {


struct BasicGame {
private:
	using sink_type = boost::coroutines::asymmetric_coroutine<Move>::push_type;
public:
	using moveset_type = boost::coroutines::asymmetric_coroutine<Move>;

	Game() = default;

	ChessPieceColor active_color() const {
		return game_state_.active_color;
	}

	move_generator valid_moves(ChessPieceColor c) const {
		return compute_valid_moves(c, board(), game_state_);
	}

	move_generator valid_moves() const {
		return valid_moves(active_color());
	}

	move_generator attackers(BoardPos pos) const {
		return compute_attackers(pos, board() 
	}

	CompressedBoard board() const {
		return board_;
	}

	[[nodiscard]]
	bool move_is_valid(Move move) {
		// Theres a much more efficient way of doing this but this is all we do for now.
		for(Move mv: valid_moves()) {
			if(mv == move) {
				return true;
			}
		}
		return false;
	}

	void apply_move(Move move) {
		if(not move_is_valid(move)) {
			throw std::runtime_error(
		}
	}

	GameSnapshot snapshot() const {
		return GameSnapshot{board_, game_state_};
	}

private:
	CompressedBoard board_;
	TemporalGameState game_state_;
};

} /* namespace ac */

#endif /* AC_GAME_H */
