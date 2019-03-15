#ifndef AC_BOARD_H
#define AC_BOARD_H

#include "BitBoard.h"
#include "ChessPiece.h"
#include <bitset>
#include <optional>
#include <array>

namespace ac {

struct Board {

	struct reference {

		constexpr const reference& operator=(reference&& other) const {
			return *this = std::as_const(other);
		}

		constexpr const reference& operator=(const reference& other) const {
			return *this = static_cast<std::optional<ChessPiece>>(other);
		}
		
		constexpr operator std::optional<ChessPiece>() const {
			return std::as_const(board_)[pos_];
		}

		constexpr operator bool() const {
			return static_cast<bool>(std::as_const(board_)[pos_]);
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

		constexpr const reference& operator=(std::optional<ChessPiece> piece) const {
			for(auto& brd: board_.bit_boards_) {
				if(brd[pos_]) {
					brd[pos_] = false;
					break;
				}
			}
			if(piece) {
				board_.bit_boards_[index(*piece)][pos_] = true;
			}
			return *this;
		}

	private:
		friend class Board;
		constexpr reference(Board& brd, BoardPos pos):
			board_(brd),
			pos_(pos)
		{
			
		}

		Board& board_;
		BoardPos pos_;
	};

	Board() = default;

	constexpr std::optional<ChessPiece> operator[](BoardPos pos) const {
		auto piece_index = 0u;
		for(auto brd: bit_boards_) {
			if(brd[pos]) {
				return chess_piece_from_index(piece_index);
			}
			++piece_index;
		}
		return std::nullopt;
	}

	constexpr reference operator[](BoardPos pos) {
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

} /* namespace ac */

#endif /* AC_BOARD_H */
