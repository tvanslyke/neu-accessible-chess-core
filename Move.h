#ifndef AC_MOVE_H
#define AC_MOVE_H

#include "ChessPiece.h"
#include <optional>
#include <utility>

namespace ac {

struct PositionalMove {
	BoardPos from;
	BoardPos to;
};

enum class CastleMove: unsigned char {
	WhiteKingside,
	WhiteQueenside,
	BlackKingside,
	BlackQueenside
};


struct SimpleMove {
	SimpleMove() = delete;

	constexpr SimpleMove(PositionalMove mv):
		is_positional_(true),
		from_(static_cast<unsigned char>(mv.from)),
		to_(static_cast<unsigned char>(mv.to)),
		castle_(0u)
	{
		
	}

	constexpr SimpleMove(CastleMove mv):
		is_positional_(true),
		from_(0u),
		to_(0u),
		castle_(static_cast<CastleMove>(mv))
	{
		
	}

	constexpr auto move_sequence() const
		-> std::pair<PositionalMove, std::optional<PositionalMove>>
	{
		
	}

	constexpr bool is_positional() const {
		return is_positional_;
	}

	constexpr std::optional<PositionalMove> as_positional_move() const {
		if(is_positional_) {
			return PositionalMove{from_, to_};
		} else {
			return std::nullopt;
		}
	}

	constexpr std::optional<PositionalMove> as_positional_move() const {
		return PositionalMove{from_, to_};
	}

	constexpr std::optional<CastleMove> as_castle_move() const {
		if(is_positional_) {
			return std::nullopt;
		} else {
			return static_cast<CastleMove>(castle_);
		}
	}

private:
	bool is_positional_   : 1u;
	unsigned char from_   : 6u;
	unsigned char to_     : 6u;
	unsigned char castle_ : 3u;
};

} /* namespace ac */

#endif /* AC_MOVE_H */
