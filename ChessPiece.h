#ifndef AC_CHESS_PIECE_H
#define AC_CHESS_PIECE_H

#include <cstddef>
#include <cassert>
#include "optional_extra.h"

namespace ac {

enum class ChessPieceKind: unsigned char {
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

enum class ChessPieceColor: bool {
	White, Black
};

enum class EnPassantDirection: bool {
	Left, Right
};

enum class PawnCaptureDirection: bool {
	Left, Right
};


enum class ChessPiece: unsigned char {
	WhitePawn,
	WhiteKnight,
	WhiteBishop,
	WhiteRook,
	WhiteQueen,
	WhiteKing,
	BlackPawn,
	BlackKnight,
	BlackBishop,
	BlackRook,
	BlackQueen,
	BlackKing
};

constexpr ChessPiece operator+(ChessPieceKind k, ChessPieceColor c) {
	return static_cast<unsigned>(c) * 6u + static_cast<unsigned>(k);
}

constexpr ChessPiece operator+(ChessPieceColor c, ChessPieceKind k) {
	return k + c;
}

constexpr std::optional<ChessPiece> operator+(std::optional<ChessPieceKind> k, std::optional<ChessPieceColor> c) {
	if(k and c) {
		return *k + *c;
	} else {
		return std::nullopt;
	}
}

constexpr std::optional<ChessPiece> operator+(std::optional<ChessPieceColor> c, std::optional<ChessPieceKind> k) {
	return k + c;
}

enum class OptionalChessPiece: unsigned char {
	WhitePawn   = static_cast<unsigned char>(ChessPiece::WhitePawn),
	WhiteKnight = static_cast<unsigned char>(ChessPiece::WhiteKnight),
	WhiteBishop = static_cast<unsigned char>(ChessPiece::WhiteBishop),
	WhiteRook   = static_cast<unsigned char>(ChessPiece::WhiteRook),
	WhiteQueen  = static_cast<unsigned char>(ChessPiece::WhiteQueen),
	WhiteKing   = static_cast<unsigned char>(ChessPiece::WhiteKing),
	BlackPawn   = static_cast<unsigned char>(ChessPiece::BlackPawn),
	BlackKnight = static_cast<unsigned char>(ChessPiece::BlackKnight),
	BlackBishop = static_cast<unsigned char>(ChessPiece::BlackBishop),
	BlackRook   = static_cast<unsigned char>(ChessPiece::BlackRook),
	BlackQueen  = static_cast<unsigned char>(ChessPiece::BlackQueen),
	BlackKing   = static_cast<unsigned char>(ChessPiece::BlackKing),
	None        = 12
};

enum class CastleStatus: unsigned char {
	/* Flags */
	None           = 0b0000u,
	WhiteKingside  = 0b0001u,
	WhiteQueenside = 0b0010u,
	BlackKingside  = 0b0100u,
	BlackQueenside = 0b1000u
};

constexpr CastleStatus operator|(CastleStatus l, CastleStatus r) {
	return static_cast<CastleStatus>(
		static_cast<unsigned char>(l) | static_cast<unsigned char>(r)
	);
}

constexpr CastleStatus operator&(CastleStatus l, CastleStatus r) {
	return static_cast<CastleStatus>(
		static_cast<unsigned char>(l) & static_cast<unsigned char>(r)
	);
}

constexpr CastleStatus operator^(CastleStatus l, CastleStatus r) {
	return static_cast<CastleStatus>(
		static_cast<unsigned char>(l) ^ static_cast<unsigned char>(r)
	);
}

constexpr CastleStatus operator~(CastleStatus s) {
	return static_cast<CastleStatus>(~static_cast<unsigned char>(s));
}

constexpr CastleStatus& operator|=(CastleStatus& l, CastleStatus r) {
	return l = l | r;
}

constexpr CastleStatus& operator&=(CastleStatus& l, CastleStatus r) {
	return l = l & r;
}

constexpr CastleStatus& operator^=(CastleStatus& l, CastleStatus r) {
	return l = l ^ r;
}


constexpr std::size_t index(ChessPiece p) {
	return static_cast<std::size_t>(p);
}

constexpr std::optional<std::size_t> index(std::optional<ChessPiece> p) {
	return p ? some(index(*p)) : std::nullopt;
}

constexpr ChessPiece chess_piece_from_index(std::size_t index) {
	return static_cast<ChessPiece>(index);
}

constexpr std::optional<ChessPiece> chess_piece_from_index(std::optional<std::size_t> index) {
	return index ? some(chess_piece_from_index(*index)) : std::nullopt;
}

constexpr std::size_t material_value(ChessPieceKind kind) {
	switch(kind) {
	default: assert(!"Bad piece kind ('King' is not a valid input to this function).");
	case ChessPieceKind::Pawn:   return 1u;
	case ChessPieceKind::Knight: return 3u;
	case ChessPieceKind::Bishop: return 3u;
	case ChessPieceKind::Rook:   return 5u;
	case ChessPieceKind::Queen:  return 9u;
	}
}

constexpr ChessPieceColor color(ChessPiece piece) {
	if(piece <= ChessPiece::WhiteKing) {
		return ChessPieceColor::White;
	} else {
		return ChessPieceColor::Black;
	}
}

constexpr std::optional<ChessPieceColor> color(std::optional<ChessPiece> piece) {
	return piece ? some(color(*piece)) : std::nullopt;
}


constexpr ChessPieceKind kind(ChessPiece piece) {
	return static_cast<ChessPieceKind>(static_cast<unsigned>(piece) % 6u);
}

constexpr std::optional<ChessPieceKind> kind(std::optional<ChessPiece> piece) {
	return piece ? std::optional{kind(*piece)} : std::nullopt;
}

enum class BoardRow: unsigned char {
	R1, R2, R3, R4, R5, R6, R7, R8
};

enum class BoardCol: unsigned char {
	A, B, C, D, E, F, G, H
};

constexpr std::size_t index(BoardRow row) {
	return static_cast<std::size_t>(row);
}

constexpr std::optional<std::size_t> index(std::optional<BoardRow> row) {
	return piece ? some(index(*row)) : std::nullopt;
}

constexpr std::size_t index(BoardCol col) {
	return static_cast<std::size_t>(col);
}

constexpr std::optional<std::size_t> index(std::optional<BoardCol> col) {
	return piece ? some(index(*col)) : std::nullopt;
}

constexpr BoardCol col_from_index(std::size_t index) {
	return static_cast<BoardCol>(index);
}

constexpr std::optional<BoardCol> col_from_index(std::optional<std::size_t> index) {
	return piece ? some(index(*col)) : std::nullopt;
}

constexpr BoardRow row_from_index(std::size_t index) {
	return static_cast<BoardRow>(index);
}

constexpr std::optional<BoardRow> row_from_index(std::optional<std::size_t> index) {
	return index ? some(row_from_index(*index)) : std::nullopt;
}
		
constexpr const char* name(BoardCol col) {
	constexpr const char* names[] = {
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H"
	};
	return names[index(col)];
}

constexpr const char* name(ChessPieceKind kind) {
	switch(kind) {
	case ChessPieceKind::King:   return "K";
	case ChessPieceKind::Queen:  return "Q";
	case ChessPieceKind::Rook:   return "R";
	case ChessPieceKind::Bishop: return "B";
	case ChessPieceKind::Knight: return "N";
	case ChessPieceKind::Pawn:   return "";
	}
}

constexpr char forsyth_edwards_encoding(ChessPiece piece) {
	switch(piece) {
	case ChessPiece::WhitePawn:   return 'P';
        case ChessPiece::WhiteKnight: return 'N';
        case ChessPiece::WhiteBishop: return 'B';
        case ChessPiece::WhiteRook:   return 'R';
        case ChessPiece::WhiteQueen:  return 'Q';
        case ChessPiece::WhiteKing:   return 'K';
        case ChessPiece::BlackPawn:   return 'p';
        case ChessPiece::BlackKnight: return 'n';
        case ChessPiece::BlackBishop: return 'b';
        case ChessPiece::BlackRook:   return 'r';
        case ChessPiece::BlackQueen:  return 'q';
        case ChessPiece::BlackKing:   return 'k';
	}
}

constexpr const char* forsyth_edwards_encoding(CastleStatus status) {
	constexpr auto WK = CastleStatus::WhiteKingside;
	constexpr auto WQ = CastleStatus::WhiteQueenside;
	constexpr auto BK = CastleStatus::BlackKingside;
	constexpr auto BQ = CastleStatus::BlackQueenside;
	constexpr auto N  = CastleStatus::None;
	switch(status) {
	default: assert(!"Bad castle status.");
	case (WK | WQ | BK | BQ): return "KQkq";
	case (WK | WQ | BK |  N): return "KQk";
	case (WK | WQ |  N | BQ): return "KQq";
	case (WK | WQ |  N |  N): return "KQ";
	case (WK |  N | BK | BQ): return "Kkq";
	case (WK |  N | BK |  N): return "Kk";
	case (WK |  N |  N | BQ): return "Kq";
	case (WK |  N |  N |  N): return "K";
	case ( N | WQ | BK | BQ): return "Qkq";
	case ( N | WQ | BK |  N): return "Qk";
	case ( N | WQ |  N | BQ): return "Qq";
	case ( N | WQ |  N |  N): return "Q";
	case ( N |  N | BK | BQ): return "kq";
	case ( N |  N | BK |  N): return "k";
	case ( N |  N |  N | BQ): return "q";
	case ( N |  N |  N |  N): return "-";
	}
}



constexpr char name_chr(BoardCol col) {
	return name(col)[0u];
}

constexpr std::optional<char> name_chr(std::optional<BoardCol> col) {
	return col ? some(name_chr(*col)) : std::nullopt;
}

constexpr const char* name(BoardRow row) {
	constexpr const char* names[] = {
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8"
	};
	return names[index(row)];
}

constexpr char name_chr(BoardRow row) {
	return name(row)[0u];
}

constexpr std::optional<char> name_chr(std::optional<BoardRow> row) {
	return row ? some(name_chr(*row)) : std::nullopt;
}

constexpr BoardRow operator ""_row(char c) {
	switch(c) {
	default: assert(!"Bad character in BoardRow literal.");
	case '1': return BoardRow::R1;
	case '2': return BoardRow::R2;
	case '3': return BoardRow::R3;
	case '4': return BoardRow::R4;
	case '5': return BoardRow::R5;
	case '6': return BoardRow::R6;
	case '7': return BoardRow::R7;
	case '8': return BoardRow::R8;
	}
}

template <char ... C>
constexpr BoardRow operator ""_row() {
	static_assert(sizeof...(C) == 1u, "Too many characters in BoardRow literal.");
	constexpr char chrs[] = {C...};
	return operator ""_row(chrs[0]);
}

constexpr BoardCol operator ""_col(char c) {
	switch(c) {
	default: assert(!"Bad character in BoardCol literal.");
	case 'a': return BoardCol::A;
	case 'b': return BoardCol::B;
	case 'c': return BoardCol::C;
	case 'd': return BoardCol::D;
	case 'e': return BoardCol::E;
	case 'f': return BoardCol::F;
	case 'g': return BoardCol::G;
	case 'h': return BoardCol::H;
	case 'A': return BoardCol::A;
	case 'B': return BoardCol::B;
	case 'C': return BoardCol::C;
	case 'D': return BoardCol::D;
	case 'E': return BoardCol::E;
	case 'F': return BoardCol::F;
	case 'G': return BoardCol::G;
	case 'H': return BoardCol::H;
	}
}

template <char ... C>
constexpr BoardCol operator ""_col() {
	static_assert(sizeof...(C) == 1u, "Too many characters in BoardCol literal.");
	constexpr char chrs[] = {C...};
	switch(chrs[0]) {
	case '1': return BoardCol::A;
	case '2': return BoardCol::B;
	case '3': return BoardCol::C;
	case '4': return BoardCol::D;
	case '5': return BoardCol::E;
	case '6': return BoardCol::F;
	case '7': return BoardCol::G;
	case '8': return BoardCol::H;
	default: assert(!"Bad column number for BoardCol literal.");
	}
}

namespace detail {

constexpr unsigned char encode_board_position(BoardRow r, BoardCol c) {
	return index(c) << 3u | index(r);
}

constexpr unsigned char encode_board_position(BoardCol c, BoardRow r) {
	return encode_board_position(r, c);
}

} /* namespace detail */

enum class BoardPos {
	A1, A2, A3, A4, A5, A6, A7, A8,
	B1, B2, B3, B4, B5, B6, B7, B8,
	C1, C2, C3, C4, C5, C6, C7, C8,
	D1, D2, D3, D4, D5, D6, D7, D8,
	E1, E2, E3, E4, E5, E6, E7, E8,
	F1, F2, F3, F4, F5, F6, F7, F8,
	G1, G2, G3, G4, G5, G6, G7, G8,
	H1, H2, H3, H4, H5, H6, H7, H8,
};

namespace detail {

inline constexpr BoardCol structured_binding_columns[] = {
	'A'_col, 'B'_col, 'C'_col, 'D'_col,
	'E'_col, 'F'_col, 'G'_col, 'H'_col
};

inline constexpr BoardRow structured_binding_rows[] = {
	1_row, 2_row, 3_row, 4_row,
	5_row, 6_row, 7_row, 8_row
};

} /* namespace detail */

// For structured bindings.
template <std::size_t I>
constexpr auto get(const BoardPos& p) noexcept
	-> const std::conditional_t<(I == 0u), BoardCol, BoardRow>&
{
	static_assert(I == 0u or I == 1u, "Out-of bounds index for 'get<index>(BoardPos)'");
	if constexpr(I == 0u) {
		return detail::structured_binding_columns[static_cast<std::size_t>(p) >> 3u];
	} else {
		return detail::structured_binding_rows[static_cast<std::size_t>(p) & 0b0111u];
	}
}

} /* namespace ac */

// For structured bindings.
template <>
class std::tuple_size<ac::BoardPos>: std::integral_constant<std::size_t, 2u> {};

// For structured bindings.
template <>
class std::tuple_element<0u, ac::BoardPos> {
public:
	using type = const ac::BoardCol;
};

// For structured bindings.
template <>
class std::tuple_element<1u, ac::BoardPos> {
public:
	using type = const ac::BoardRow;
};

namespace ac {

constexpr BoardRow row(BoardPos pos) {
	auto [c, r] = pos;
	return r;
}

constexpr BoardCol col(BoardPos pos) {
	auto [c, r] = pos;
	return c;
}


template <BoardPos Pos>
inline constexpr const char board_pos_name[] = {
	name_chr(col(Pos)), name_chr(row(Pos)), '\0'
};

namespace detail {

static constexpr char board_position_names[64u][3u] = {
	"a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8",
	"b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8",
	"c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8",
	"d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8",
	"e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8",
	"f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
	"g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8",
	"h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8"
};

} /* namespace detail */

constexpr const char* name(BoardPos pos) {
	return detail::board_position_names[static_cast<unsigned char>(pos)];
}

constexpr std::size_t index(BoardPos pos) {
	return static_cast<std::size_t>(pos);
}

constexpr BoardPos make_board_pos(BoardCol c, BoardRow r) {
	return static_cast<BoardPos>(
		static_cast<unsigned char>(c) * 8u + static_cast<unsigned char>(r)
	);
}

constexpr BoardPos make_board_pos(BoardRow r, BoardCol c) {
	return make_board_pos(c, r);
}

constexpr BoardPos operator ""_pos(const char* s, std::size_t n) {
	assert(n == 2u && "Exactly two characters are required for a board-position literal.");
	return make_board_pos(operator ""_row(s[1]), operator ""_col(s[0]));
}

constexpr BoardPos board_pos_from_string(std::string_view sv) {
	assert(sv.size() == 2);
	return operator""_pos(sv.data(), 2u);
}

constexpr BoardPos board_pos_from_index(std::size_t index) {
	assert(index < 64u);
	return static_cast<BoardPos>(index);
}

constexpr BoardCol col_before(BoardCol c) {
	assert(c != 1_col);
	return col_from_index(index(c) - 1u);
}

constexpr BoardCol col_after(BoardCol c) {
	assert(c != 8_col);
	return col_from_index(index(c) + 1u);
}

constexpr BoardRow row_before(BoardRow r) {
	assert(r != 1_row);
	return row_from_index(index(r) - 1u);
}

constexpr BoardRow row_after(BoardRow r) {
	assert(r != 8_row);
	return row_from_index(index(r) + 1u);
}

constexpr BoardPos col_before(BoardPos pos) {
	auto [c, r] = pos;
	return make_board_pos(r, col_before(c));
}

constexpr BoardPos col_after(BoardPos pos) {
	auto [c, r] = pos;
	return make_board_pos(r, col_after(c));
}

constexpr BoardPos row_before(BoardPos pos) {
	auto [c, r] = pos;
	return make_board_pos(row_before(r), c);
}

constexpr BoardPos row_after(BoardPos pos) {
	auto [c, r] = pos;
	return make_board_pos(row_after(r), c);
}

constexpr bool has_col_before(BoardPos pos) {
	return col(pos) != 1_col;
}

constexpr bool has_col_after(BoardPos pos) {
	return col(pos) != 8_col;
}

constexpr bool has_row_before(BoardPos pos) {
	return row(pos) != 1_row;
}

constexpr bool has_row_after(BoardPos pos) {
	return row(pos) != 8_row;
}

constexpr std::optional<BoardRow> operator+(BoardRow r, std::ptrdiff_t ofs) {
	auto idx = index(r);
	if(ofs < 0) {
		idx -= std::size_t(-ofs);
	} else {
		idx += std::size_t(ofs);
	}
	if(idx > 8u) {
		return std::nullopt;
	}
	return row_from_index(idx);
}

constexpr std::optional<BoardRow> operator+(std::ptrdiff_t ofs, BoardRow r) {
	return r + ofs;
}

constexpr std::optional<BoardRow> operator-(BoardRow r, std::ptrdiff_t ofs) {
	return r + -ofs;
}

constexpr std::optional<BoardCol> operator+(BoardCol c, std::ptrdiff_t ofs) {
	auto idx = index(c);
	if(ofs < 0) {
		idx -= std::size_t(-ofs);
	} else {
		idx += std::size_t(ofs);
	}
	if(idx > 8u) {
		return std::nullopt;
	}
	return col_from_index(idx);
}

constexpr std::optional<BoardCol> operator+(std::ptrdiff_t ofs, BoardCol c) {
	return c + ofs;
}

constexpr std::optional<BoardCol> operator-(BoardCol c, std::ptrdiff_t ofs) {
	return c + -ofs;
}

constexpr BoardPos operator+(BoardRow r, BoardCol c) {
	return make_board_pos(r, c);
}

constexpr BoardPos operator+(BoardCol r, BoardRow c) {
	return make_board_pos(r, c);
}

constexpr std::optional<BoardPos> operator+(std::optional<BoardRow> r, std::optional<BoardCol> c) {
	if(r and c) {
		return *r + *c;
	}
	return std::nullopt;
}

constexpr std::optional<BoardPos> operator+(std::optional<BoardCol> c, std::optional<BoardRow> r) {
	if(r and c) {
		return *r + *c;
	}
	return std::nullopt;
}

constexpr BoardPos en_passant_target_to_capture(BoardPos pos) {
	if(row(pos) == 3_row) {
		return row_after(pos);
	} else if(row(pos) == 6_row) {
		return row_before(pos);
	} else {
		assert(!"Not an en passant target.");	
	}
}

constexpr BoardPos en_passant_capture_to_target(BoardPos pos) {
	if(row(pos) == 4_row) {
		return row_before(pos);
	} else if(row(pos) == 5_row) {
		return row_after(pos);
	} else {
		assert(!"Not an en passant target.");	
	}
}

inline constexpr std::array<BoardRow, 8u> each_row = {
	1_row, 2_row, 3_row, 4_row,
	5_row, 6_row, 7_row, 8_row,
};

inline constexpr std::array<BoardCol, 8u> each_col = {
	1_col, 2_col, 3_col, 4_col,
	5_col, 6_col, 7_col, 8_col,
};

inline constexpr std::array<BoardPos, 64u> each_position = {
	"a1"_pos, "a2"_pos, "a3"_pos, "a4"_pos, "a5"_pos, "a6"_pos, "a7"_pos, "a8"_pos,
	"b1"_pos, "b2"_pos, "b3"_pos, "b4"_pos, "b5"_pos, "b6"_pos, "b7"_pos, "b8"_pos,
	"c1"_pos, "c2"_pos, "c3"_pos, "c4"_pos, "c5"_pos, "c6"_pos, "c7"_pos, "c8"_pos,
	"d1"_pos, "d2"_pos, "d3"_pos, "d4"_pos, "d5"_pos, "d6"_pos, "d7"_pos, "d8"_pos,
	"e1"_pos, "e2"_pos, "e3"_pos, "e4"_pos, "e5"_pos, "e6"_pos, "e7"_pos, "e8"_pos,
	"f1"_pos, "f2"_pos, "f3"_pos, "f4"_pos, "f5"_pos, "f6"_pos, "f7"_pos, "f8"_pos,
	"g1"_pos, "g2"_pos, "g3"_pos, "g4"_pos, "g5"_pos, "g6"_pos, "g7"_pos, "g8"_pos,
	"h1"_pos, "h2"_pos, "h3"_pos, "h4"_pos, "h5"_pos, "h6"_pos, "h7"_pos, "h8"_pos
};

} /* namespace ac */

#endif /* AC_CHESS_PIECE_H */
