#ifndef AC_BITBOARD_H
#define AC_BITBOARD_H

#include "ChessPiece.h"
#include "portable-snippets/builtin/builtin.h"
#include <array>
#include <cstdint>
#include <bitset>

namespace ac {

struct PositionSet;

struct BitBoard {

private:
	using int_type = std::uint_least64_t;
public:

	struct bit_reference {
		constexpr operator bool() const {
			return ((board_ >> static_cast<std::size_t>(pos_)) & 0x01u) != 0u;
		}

		constexpr const bit_reference& operator=(bool b) const {
			if(b) {
				board_ |= int_type(1u) << index(pos_);
				assert(board_ & (int_type(1u) << index(pos_)));
			} else {
				board_ &= ~(int_type(1u) << index(pos_));
				assert(not (board_ & (int_type(1u) << index(pos_))));
			}
			return *this;
		}

		constexpr bool operator~() const {
			return not static_cast<bool>(*this);
		}

		constexpr void clear() {
			*this = false;
		}

		constexpr void set() {
			*this = true;
		}

	private:
		friend class BitBoard;
		constexpr bit_reference(int_type& b, BoardPos p):
			board_(b), pos_(p)
		{
			
		}

		int_type& board_;
		BoardPos pos_;
	};

	BitBoard() = default;

	constexpr bool operator[](BoardPos pos) const {
		return (0x01u & (board_ >> index(pos))) == 1u;
	}

	constexpr bit_reference operator[](BoardPos pos) {
		return {board_, pos};
	}

	constexpr bool operator[](std::pair<BoardRow, BoardCol> pos) const {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr bit_reference operator[](std::pair<BoardRow, BoardCol> pos) {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr bool operator[](std::pair<BoardCol, BoardRow> pos) const {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr bit_reference operator[](std::pair<BoardCol, BoardRow> pos) {
		return (*this)[make_board_pos(pos.first, pos.second)];
	}

	constexpr std::bitset<64u> as_bitset() const {
		return {board_};
	}

	constexpr bool any() const {
		return board_ != 0u;
	}

	constexpr bool all() const {
		return board_ == ~int_type(0u);
	}

	constexpr bool none() const {
		return not any();
	}

	constexpr std::size_t count() const {
		return psnip_builtin_popcount64(board_);
	}

	friend constexpr BitBoard& operator|=(BitBoard& l, BitBoard r) {
		l.board_ |= r.board_;
		return l;
	}

	friend constexpr BitBoard& operator&=(BitBoard& l, BitBoard r) {
		l.board_ &= r.board_;
		return l;
	}

	friend constexpr BitBoard operator^=(BitBoard& l, BitBoard r) {
		l.board_ ^= r.board_;
		return l;
	}

	friend constexpr BitBoard operator|(BitBoard l, BitBoard r) {
		return {l.board_ | r.board_};
	}

	friend constexpr BitBoard operator&(BitBoard l, BitBoard r) {
		return {l.board_ & r.board_};
	}

	friend constexpr BitBoard operator^(BitBoard l, BitBoard r) {
		return {l.board_ ^ r.board_};
	}

	friend constexpr BitBoard operator~(BitBoard b) {
		return BitBoard(~b.board_);
	}

	friend constexpr bool operator==(BitBoard l, BitBoard r) {
		return l.board_ == r.board_;
	}

	friend constexpr bool operator!=(BitBoard l, BitBoard r) {
		return l.board_ != r.board_;
	}

	constexpr PositionSet positions() const;

private:
	friend struct BoardPosIterator;

	constexpr BitBoard(int_type b):
		board_(b)
	{
		
	}

	int_type board_ = 0u;
};

struct BoardPosIterator {
	using value_type        = BoardPos;
	using reference         = BoardPos;
	using pointer           = const BoardPos*;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::input_iterator_tag;
	
	BoardPosIterator() = default;

	constexpr BoardPosIterator(BitBoard board):
		board_(board)
	{
		
	}

	constexpr BoardPosIterator& operator++() {
		assert(board_.count() > 0u);
		auto cnt = board_.count();
		board_[position()].clear();
		return *this;
	}

	constexpr BoardPosIterator operator++(int) {
		auto cpy = *this;
		++(*this);
		return cpy;
	}

	friend constexpr value_type operator*(BoardPosIterator it) {
		return it.position();
	}

	friend constexpr bool operator==(BoardPosIterator l, BoardPosIterator r) {
		return l.board_ == r.board_;
	}

	friend constexpr bool operator!=(BoardPosIterator l, BoardPosIterator r) {
		return not (l == r);
	}

private:

	constexpr std::size_t index() const {
		assert(board_.any() && "Attempt to deference past-the-end BoardPosIterator.");
		return psnip_builtin_ffs64(board_.board_) - 1u;
	}

	constexpr BoardPos position() const {
		return board_pos_from_index(index());
	}

	BitBoard board_;
};

struct PositionSet {

	using value_type      = BoardPos;
	using iterator        = BoardPosIterator;
	using const_iterator  = BoardPosIterator;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;

	PositionSet() = default;

	constexpr PositionSet(BitBoard b):
		board_(b)
	{
	
	}
	
	constexpr std::size_t size() const {
		return as_bitboard().count();
	}

	constexpr bool empty() const {
		return size() == 0u;
	}

	constexpr BoardPosIterator begin() const {
		return BoardPosIterator(board_);
	}

	constexpr BoardPosIterator cbegin() const {
		return BoardPosIterator(board_);
	}

	constexpr BoardPosIterator end() const {
		return BoardPosIterator();
	}

	constexpr BoardPosIterator cend() const {
		return BoardPosIterator();
	}

	constexpr BitBoard as_bitboard() const {
		return board_;
	}

	constexpr void remove(BoardPos pos) {
		board_[pos] = false;
	}

	constexpr void add(BoardPos pos) {
		board_[pos] = true;
	}

	constexpr bool contains(BoardPos pos) const {
		return board_[pos];
	}

	constexpr void remove(std::pair<BoardCol, BoardRow> pos) {
		board_[pos] = false;
	}

	constexpr void add(std::pair<BoardCol, BoardRow> pos) {
		board_[pos] = true;
	}

	constexpr bool contains(std::pair<BoardCol, BoardRow> pos) const {
		return board_[pos];
	}

	constexpr void remove(std::pair<BoardRow, BoardCol> pos) {
		board_[pos] = false;
	}

	constexpr void add(std::pair<BoardRow, BoardCol> pos) {
		board_[pos] = true;
	}

	constexpr bool contains(std::pair<BoardRow, BoardCol> pos) const {
		return board_[pos];
	}

private:
	BitBoard board_;
};

constexpr PositionSet BitBoard::positions() const {
	return PositionSet(*this);
}

constexpr void try_add_position(PositionSet& pset, std::optional<BoardRow> r, std::optional<BoardCol> c) {
	if(not r) {
		return;
	}
	if(not c) {
		return;
	}
	pset.add({*r, *c});
}

constexpr void try_add_position(PositionSet& pset, std::optional<BoardCol> c, std::optional<BoardRow> r) {
	try_add_position(pset, r, c);
}

constexpr PositionSet white_pawn_moveset(BoardPos pos) {
	assert(row(pos) != 8_row);
	PositionSet pset;
	pset.add(row_after(pos));
	if(row(pos) == 2_row) {
		pset.add({col(pos), 4_row});
	}
	return pset;
}

constexpr PositionSet black_pawn_moveset(BoardPos pos) {
	assert(row(pos) != 1_row);
	PositionSet pset;
	pset.add(row_before(pos));
	if(row(pos) == 7_row) {
		pset.add({col(pos), 5_row});
	}
	return pset;
}

constexpr PositionSet white_pawn_attack_moveset(BoardPos pos) {
	PositionSet pset;
	auto [col, row] = pos;
	if(row + 1) {
		if(col + 1) {
			pset.add(*(row + 1) + *(col + 1));
		}
		if(col - 1) {
			pset.add(*(row + 1) + *(col - 1));
		}
	}
	return pset;
}

constexpr PositionSet black_pawn_attack_moveset(BoardPos pos) {
	PositionSet pset;
	auto [col, row] = pos;
	if(row - 1) {
		if(col + 1) {
			pset.add(*(row - 1) + *(col + 1));
		}
		if(col - 1) {
			pset.add(*(row - 1) + *(col - 1));
		}
	}
	return pset;
}

constexpr PositionSet king_moveset(BoardPos pos) {
	PositionSet pset;
	try_add_position(pset, row(pos) - 1, col(pos) - 1);
	try_add_position(pset, row(pos) - 1, col(pos));
	try_add_position(pset, row(pos) - 1, col(pos) + 1);
	try_add_position(pset, row(pos), col(pos) - 1);
	try_add_position(pset, row(pos), col(pos) + 1);
	try_add_position(pset, row(pos) + 1, col(pos) - 1);
	try_add_position(pset, row(pos) + 1, col(pos));
	try_add_position(pset, row(pos) + 1, col(pos) + 1);
	return pset;
}

constexpr PositionSet knight_moveset(BoardPos pos) {
	PositionSet pset;
	auto [c, r] = pos;
	try_add_position(pset, r + 2, c + 1);
	try_add_position(pset, r + 2, c - 1);
	try_add_position(pset, r - 2, c + 1);
	try_add_position(pset, r - 2, c - 1);
	try_add_position(pset, r + 1, c + 2);
	try_add_position(pset, r + 1, c - 2);
	try_add_position(pset, r - 1, c + 2);
	try_add_position(pset, r - 1, c - 2);
	return pset;
}

constexpr PositionSet bishop_moveset(BoardPos pos) {
	PositionSet pset;
	auto [c, r] = pos;
	for(int i = 1; c + i and r + i; ++i) {
		pset.add({*(c + i), *(r + i)});
	}
	for(int i = 1; c + i and r - i; ++i) {
		pset.add({*(c + i), *(r - i)});
	}
	for(int i = 1; c - i and r + i; ++i) {
		pset.add({*(c - i), *(r + i)});
	}
	for(int i = 1; c - i and r - i; ++i) {
		pset.add({*(c - i), *(r - i)});
	}
	return pset;
}

constexpr PositionSet rook_moveset(BoardPos pos) {
	PositionSet pset;
	auto [c, r] = pos;
	for(auto r: each_row) {
		pset.add({r, c});
	}
	for(auto c: each_col) {
		pset.add({r, c});
	}
	pset.remove(pos);
	return pset;
}

constexpr PositionSet queen_moveset(BoardPos pos) {
	return (bishop_moveset(pos).as_bitboard() | rook_moveset(pos).as_bitboard()).positions();
}


constexpr PositionSet moveset(ChessPieceKind k, BoardPos pos) {
	assert(k != ChessPieceKind::Pawn && "The pawn moveset depends on the current game state.");
	switch(k) {
	case ChessPieceKind::King:   return king_moveset(pos);
	case ChessPieceKind::Queen:  return queen_moveset(pos);
	case ChessPieceKind::Rook:   return rook_moveset(pos);
	case ChessPieceKind::Bishop: return bishop_moveset(pos);
	case ChessPieceKind::Knight: return knight_moveset(pos);
	}
}

constexpr PositionSet moveset(ChessPiece piece, BoardPos pos) {
	switch(kind(piece)) {
	case ChessPieceKind::King:   return king_moveset(pos);
	case ChessPieceKind::Queen:  return queen_moveset(pos);
	case ChessPieceKind::Rook:   return rook_moveset(pos);
	case ChessPieceKind::Bishop: return bishop_moveset(pos);
	case ChessPieceKind::Knight: return knight_moveset(pos);
	case ChessPieceKind::Pawn:
		if(color(piece) == ChessPieceColor::Black) {
			return black_pawn_moveset(pos);
		} else {
			return white_pawn_moveset(pos);
		}
	}
}

} /* namespace ac */

#endif /* AC_BITBOARD_H */
