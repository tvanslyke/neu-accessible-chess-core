#ifndef AC_BOARD_H
#define AC_BOARD_H

#include "BitBoard.h"
#include <bitset>
#include <optional>
#include <array>

namespace ac {

struct Board {

	struct reference {

		constexpr reference& operator=(reference&& other) const {
			return *this = std::as_const(other);
		}

		constexpr reference& operator=(const reference& other) const {
			return *this = static_cast<std::optional<ChessPiece>>(other);
		}
		
		constexpr operator std::optional<ChessPiece>() const {
			return std::as_const(board_)[pos_];
		}

		constexpr operator bool() const {
			return std::as_const(board_)[pos_];
		}

		constexpr ChessPiece operator*() const {
			auto piece = std::as_const(board_)[pos_];
			assert(piece);
			return *piece;
		}

		constexpr bool operator==(std::optional<ChessPiece> piece) const {
			return static_cast<std::optional<ChessPiece>>(*this) == piece;
		}

		constexpr bool operator!=(std::optional<ChessPiece> piece) const {
			return static_cast<std::optional<ChessPiece>>(*this) != piece;
		}

		constexpr reference& operator=(std::optional<ChessPiece> piece) const {
			auto brd = std::find_if(
				board_.bit_boards_.begin(),
				board_.bit_boards_.end(),
				[](const auto brd) { return brd[pos_]; }
			);
			if(brd != board_.bit_boards_.end()) {
				(*brd)[pos_] = false;
			}
			if(piece) {
				board_.bit_boards_[index(*piece)] = true;
			}
		}

	private:
		constexpr reference(Board& brd, BoardPos pos):
			board_(brd),
			pos_(pos)
		{
			
		}

		Board& board_;
		BoardPos pos_;
	};

	Board() = default;

	constexpr std::optional<ChessPiece> operator[](BoardPosition pos) const {
		auto pos = std::find_if(bit_boards_.begin(), bit_boards_.end(), [](auto b){ return b.any(); });
		if(pos != bit_boards_.end()) { 
			return static_cast<ChessPiece>(pos - bit_boards_.begin());
		}
		return std::nullopt;
	}

	constexpr reference operator[](BoardPosition pos) const {
		return reference(*this, pos);
	}


	constexpr BitBoard positions(ChessPiece piece) const {
		return bit_boards_[static_cast<std::size_t>(piece)];
	}


	constexpr BitBoard all_positions() const {
		return white_positions() | black_positions();
	}


	constexpr BitBoard white_positions() const {
		return white_king_positions()
			| white_queen_positions()
			| white_rook_positions()
			| white_bishop_positions()
			| white_knight_positions()
			| white_pawn_positions();
	}

	constexpr BitBoard black_positions() const {
		return black_king_positions()
			| black_queen_positions()
			| black_rook_positions()
			| black_bishop_positions()
			| black_knight_positions()
			| black_pawn_positions();
	}


	constexpr BitBoard white_pawn_positions() const {
		return positions(ChessPiece::WhitePawn);
	}

	constexpr BitBoard white_knight_positions() const {
		return positions(ChessPiece::WhiteKnight);
	}

	constexpr BitBoard white_bishop_positions() const {
		return positions(ChessPiece::WhiteBishop);
	}

	constexpr BitBoard white_rook_positions() const {
		return positions(ChessPiece::WhiteRook);
	}

	constexpr BitBoard white_queen_positions() const {
		return positions(ChessPiece::WhiteQueen);
	}

	constexpr BitBoard white_king_positions() const {
		return positions(ChessPiece::WhiteKing);
	}


	constexpr BitBoard black_pawn_positions() const {
		return positions(ChessPiece::BlackPawn);
	}

	constexpr BitBoard black_knight_positions() const {
		return positions(ChessPiece::BlackKnight);
	}

	constexpr BitBoard black_bishop_positions() const {
		return positions(ChessPiece::BlackBishop);
	}

	constexpr BitBoard black_rook_positions() const {
		return positions(ChessPiece::BlackRook);
	}

	constexpr BitBoard black_queen_positions() const {
		return positions(ChessPiece::BlackQueen);
	}

	constexpr BitBoard black_king_positions() const {
		return positions(ChessPiece::BlackKing);
	}


	constexpr bool _invariants() const {
		auto all = BitBoard();
		for(auto board: bit_boards_) {
			if((all & board).any()){
				return false;
			}
			all |= board;
		}
		return true;
	}

	constexpr void _assert_invariants() const {
		assert(_invariants() && "Two pieces cannot be on the same board position.");
	}

private:
	std::array<BitBoard, 12u> bit_boards_;
};

static_assert(sizeof(BitBoard) <= 8u);

} /* namespace ac */

#endif /* AC_BOARD_H */
