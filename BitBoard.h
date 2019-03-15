#ifndef AC_BITBOARD_H
#define AC_BITBOARD_H

#include "ChessPiece.h"
#include <array>
#include <cstdint>

namespace ac {

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
				board_ |= int_type(1u)<< index(pos_);
			} else {
				board_ &= ~(int_type(1u) << index(pos_));
			}
			return *this;
		}

		constexpr bool operator~() const {
			return not static_cast<bool>(*this);
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
		return (0x01u & (board_ >> index(pos))) != 0u;
	}

	constexpr bit_reference operator[](BoardPos pos) {
		return {board_, pos};
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
#ifdef __GNUC__ 
		// Generates better assembly.
		return __builtin_popcountll(board_);
#else
		std::size_t count = 0u;
		auto brd = board_;
		while(brd != 0u) {
			count += brd & 0x01u;
			brd >>= 1u;
		}
		return count;
#endif
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

private:
	constexpr BitBoard(int_type b):
		board_(b)
	{
		
	}

	int_type board_;
};

} /* namespace ac */

#endif /* AC_BITBOARD_H */
