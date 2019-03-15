#ifndef AC_BOARD_H
#define AC_BOARD_H

#include "BitBoard.h"
#include "ChessPiece.h"
#include <bitset>
#include <optional>
#include <utility>
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
	
	constexpr std::optional<ChessPiece> operator[](std::pair<BoardCol, BoardRow> pos) const {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr reference operator[](std::pair<BoardCol, BoardRow> pos) {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr std::optional<ChessPiece> operator[](std::pair<BoardRow, BoardCol> pos) const {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr reference operator[](std::pair<BoardRow, BoardCol> pos) {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

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

	static constexpr Board decode_fen_string(std::string_view fenstr) {
		Board board;
		auto row_idx = 7u;
		auto col_idx = 0u;
		for(char c: fenstr) {
			assert(col_idx <= 8u);
			assert(row_idx < 8u);
			switch(c) {
			default: assert(!"Bad fenstring.");
			case '1': 
			case '2': 
			case '3': 
			case '4': 
			case '5': 
			case '6': 
			case '7':
			case '8': 
				col_idx += c - '0';
				assert(col_idx <= 8u && "Bad fenstring.");
				break;
			case '/':
				assert(col_idx == 8u && "Bad fenstring.");
				++row_idx;
				col_idx = 0u;
				break;
			case 'K':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::WhiteKing;
				break;
			case 'Q':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::WhiteQueen;
				break;
			case 'R':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::WhiteRook;
				break;
			case 'B':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::WhiteBishop;
				break;
			case 'N':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::WhiteKnight;
				break;
			case 'P':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::WhitePawn;
				break;
			case 'k':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::BlackKing;
				break;
			case 'q':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::BlackQueen;
				break;
			case 'r':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::BlackRook;
				break;
			case 'b':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::BlackBishop;
				break;
			case 'n':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::BlackKnight;
				break;
			case 'p':
				board[{row_from_index(row_idx), col_from_index(col_idx)}] = ChessPiece::BlackPawn;
				break;
			}
		}
		return board;
	}

	std::string fenstring() const {
		std::string fenstr;
		const auto& self = *this;
		for(auto row: {8_row, 7_row, 6_row, 5_row, 4_row, 3_row, 2_row, 1_row}) {
			std::size_t blank_count = 0u;
			for(auto col: {1_col, 2_col, 3_col, 4_col, 5_col, 6_col, 7_col, 8_col}) {
				auto piece = self[{row, col}];
				if(not piece) {
					++blank_count;
					continue;
				} else {
					if(blank_count != 0u) {
						fenstr.push_back(static_cast<char>('0' + blank_count));
						blank_count = 0u;
					}
					fenstr.push_back(forsyth_edwards_encoding(*piece));
				}
			}
			if(blank_count != 0u) {
				fenstr.push_back('0' + blank_count);
			}
			if(row != 1_row) {
				fenstr.push_back('/');
			}
		}
		return fenstr;
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
	std::array<BitBoard, 12u> bit_boards_ = {
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{},
		BitBoard{}
	};
};

} /* namespace ac */

#endif /* AC_BOARD_H */
