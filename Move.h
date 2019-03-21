#ifndef AC_MOVE_H
#define AC_MOVE_H

#include "ChessPiece.h"
#include <optional>
#include <utility>
#include <ostream>

namespace ac {

struct PositionalMove {
	BoardPos from;
	BoardPos to;
};

enum class CastleKind: unsigned char {
	WhiteKingside,
	WhiteQueenside,
	BlackKingside,
	BlackQueenside
};

enum class MoveKind: unsigned char {
	Commmon,
	EnPassant,
	Castle,
	PawnPromotion
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

	constexpr SimpleMove(CastleKind mv):
		is_positional_(true),
		from_(0u),
		to_(0u),
		castle_(static_cast<CastleKind>(mv))
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

	constexpr std::optional<CastleKind> as_castle_move() const {
		if(is_positional_) {
			return std::nullopt;
		} else {
			return static_cast<CastleKind>(castle_);
		}
	}

private:
	bool is_positional_   : 1u;
	unsigned char from_   : 6u;
	unsigned char to_     : 6u;
	unsigned char castle_ : 2u;
};

struct CommonMove {
	
	constexpr CommonMove(
		PositionalMove mv,
		ChessPiece piece,
		std::optional<ChessPiece> captured = std::nullopt
	):
		piece_(static_cast<unsigned char>(piece)),
		from_(static_cast<unsigned char>(mv.from)),
		to_(static_cast<unsigned char>(mv.to)),
		has_capture_(captured),
		captured_()
	{
		if(captured) {
			captured_ = *captured;
		}
	}

	constexpr CommonMove(
		PositionalMove mv,
		ChessPiece piece,
		std::optional<ChessPiece> captured = std::nullopt
	):
		CommonMove(mv, piece, captured)
	{
		
	}

	constexpr ChessPiece moved_piece() const {
		return static_cast<ChessPiece>(piece_);
	}

	constexpr BoardPos start_position() const {
		return static_cast<BoardPos>(from_);
	}

	constexpr BoardPos end_position() const {
		return static_cast<BoardPos>(to_);
	}

	constexpr std::optional<BoardPos> captured_position() const {
		if(has_capture_) {
			return end_position();
		} else {
			return std::nullopt;
		}
	}

	constexpr std::optional<ChessPiece> captured_piece() const {
		if(has_capture_) {
			return static_cast<ChessPiece>(captured_piece_);
		} else {
			return std::nullopt;
		}
	}

	constexpr std::optional<CommonMove> secondary_move() const {
		return std::nullopt;
	}

	friend constexpr bool operator==(CommonMove l, CommonMove r) {
		if(l.piece_ != r.piece_ or l.from_ != r.from_) {
			return false;
		}
		if(l.has_capture_) {
			if(r.has_capture_) {
				return l.captured_piece_ == r.captured_piece_;
			} else {
				return false;
			}
		}
	}

	friend constexpr bool operator!=(CommonMove l, CommonMove r) {
		return not (l == r);
	}

private:
	unsigned char piece_          : 4u;
	unsigned char from_           : 6u;
	unsigned char to_             : 6u;
	bool has_capture_             : 1u;
	unsigned char captured_piece_ : 6u;
};

struct PawnPromotionMove {

	constexpr PawnPromotionMove(
		ChessPieceColor c,
		ChessPieceKind promotion_kind,
		BoardCol start_col,
		std::optional<PawnCaptureDirection> capture_dir = std::nullopt,
		std::optional<ChessPiece> captured = std::nullopt
	):
		color_(c),
		start_col_(start_col),
		end_col_(end_col),
		captured_kind_(captued ? kind(*captured) : ChessPieceKind::King),
		promoted_kind_(promotion_kind)
	{
		
	}

	constexpr ChessPiece moved_piece() const {
		return color_ == ChessPieceColor::White ? ChessPiece::WhitePawn : ChessPiece::BlackPawn;
	}

	constexpr BoardPos start_position() const {
		switch(color_) {
		case ChessPieceColor::White:
			return 7_row + static_cast<BoardCol>(start_col_);
		case ChessPieceColor::Black:
			return 2_row + static_cast<BoardCol>(start_col_);
		}
	}

	constexpr BoardPos end_position() const {
		switch(color_) {
		case ChessPieceColor::White:
			return 8_row + static_cast<BoardCol>(end_col_);
		case ChessPieceColor::Black:
			return 1_row + static_cast<BoardCol>(end_col_);
		}
	}

	constexpr std::optional<BoardPos> captured_position() const {
		if(col(end_position()) != col(start_position())) {
			return end_position();
		} else {
			return std::nullopt;
		}
	}

	constexpr std::optional<ChessPiece> captured_piece() const {
		if(captured_position()) {
			return color_ + static_cast<ChessPieceKind>(captured_kind_);
		} else {
			return std::nullopt;
		}
	}

	constexpr std::optional<CommonMove> secondary_move() const {
		return std::nullopt;
	}

	constexpr std::optional<ChessPiece> promotion() const {
		return color_ + static_cast<ChessPieceKind>(promoted_kind_);
	}

	friend constexpr bool operator==(EnPassantMove l, EnPassantMove r) {
		return l.color_ == r.color_
			l.start_col_ == l.start_col_
			l.end_col_ == l.end_col_
			l.captured_kind_ == l.captured_kind_
			l.promoted_kind_ == l.promoted_kind_;
	}

	friend constexpr bool operator!=(EnPassantMove l, EnPassantMove r) {
		return not (l == r);
	}

private:

	ChessPieceColor color_        : 1u;
	unsigned char start_col_      : 3u;
	unsigned char end_col_        : 3u;
	unsigned char captured_kind_  : 3u;
	unsigned char promoted_kind_  : 3u;
};

struct EnPassantMove {
	constexpr EnPassantMove(ChessPieceColor color, BoardCol col, EnPassantDirection dir):
		col_(static_cast<unsigned char>(pos)),
		dir_(dir),
		color_(color)
	{
		if(col == 1_col) {
			assert(dir == EnPassantDirection::Right && "Invalid en passant move.");
		} else if(col == 8_col) {
			assert(dir == EnPassantDirection::Left && "Invalid en passant move.");
		}
	}

	constexpr EnPassantMove(ChessPieceColor color, EnPassantDirection dir, BoardCol col):
		EnPassantMove(color, col, dir)
	{
		
	}

	constexpr EnPassantMove(EnPassantDirection dir, ChessPieceColor color, BoardCol col):
		EnPassantMove(color, col, dir)
	{
		
	}

	constexpr EnPassantMove(EnPassantDirection dir, BoardCol col, ChessPieceColor color):
		EnPassantMove(color, col, dir)
	{
		
	}

	constexpr EnPassantMove(BoardCol col, EnPassantDirection dir, ChessPieceColor color):
		EnPassantMove(color, col, dir)
	{
		
	}

	constexpr EnPassantMove(BoardCol col, ChessPieceColor color, EnPassantDirection dir):
		EnPassantMove(color, col, dir)
	{
		
	}

	constexpr EnPassantMove(PositionalMove mv):
		EnPassantMove(
			col(mv.from), 
			row(mv.from) == BoardRow::R4 ? ChessPieceColor::Black : ChessPieceColor::White,
			col(mv.to) < col(mv.from) ? EnPassantDir::Left : EnPassantDir::Right
		)
	{
		if(row(mv.from) == 4_row) {
			assert(row(mv.to) == 3_row && "Invalid positional description for en passant move.");
		} else if(row(mv.from) == 5_row) {
			assert(row(mv.to) == 6_row && "Invalid positional description for en passant move.");
		} else {
			assert("En passant moves can only originate from row/rank 4 or 5");
		}
		assert(
			col(mv.to) == col(mv.from) + 1 or col(mv.to) == col(mv.from) - 1,
			"En passant moves must be diagonal."
		);
	}

	constexpr ChessPiece moved_piece() const {
		switch(color_) {
		case ChessPieceColor::White: return ChessPiece::WhitePawn;
		case ChessPieceColor::Black: return ChessPiece::BlackPawn;
		}
	}

	constexpr std::optional<ChessPiece> captured_piece() const {
		switch(color_) {
		case ChessPieceColor::White: return ChessPiece::BlackPawn;
		case ChessPieceColor::Black: return ChessPiece::WhitePawn;
		}
	}

	constexpr std::optional<BoardPos> captured_position() const {
		switch(dir_) {
		case EnPassantDirection::Left:  return col_before(start_position());
		case EnPassantDirection::Right: return col_after(start_position());
		}
	}

	constexpr BoardPos start_position() const {
		return static_cast<BoardPos>(pos_);
	}

	constexpr BoardPos end_position() const {
		switch(color_) {
		case ChessPieceColor::White:
			switch(dir_) {
			case EnPassantDirection::Left:
				return 6_row + col_before(col_);
			case EnPassantDirection::Right:
				return 6_row + col_after(col_);
			}
		case ChessPieceColor::Black:
			switch(dir_) {
			case EnPassantDirection::Left:
				return 3_row + col_before(col_);
			case EnPassantDirection::Right:
				return 3_row + col_after(col_);
			}
		}
	}

	constexpr std::optional<CommonMove> secondary_move() const {
		return std::nullopt;
	}

	constexpr std::optional<ChessPiece> promotion() const {
		return std::nullopt;
	}

	friend constexpr bool operator==(EnPassantMove l, EnPassantMove r) {
		return l.col_ == r.col_ and l.dir_ == r.dir_ and l.color_ == r.color_;
	}

	friend constexpr bool operator!=(EnPassantMove l, EnPassantMove r) {
		return not (l == r);
	}

private:
	unsigned char col_      : 3u;
	EnPassantDirection dir_ : 1u;
	ChessPieceColor color_  : 1u;
};

struct CastleMove {
	constexpr CastleMove(CastleKind kind):
		kind_(kind)
	{
		
	}

	constexpr ChessPiece moved_piece() const {
		switch(kind_) {
		case CastleKind::WhiteKingside:
		case CastleKind::WhiteQueenside:
			return ChessPiece::WhiteKing;
		case CastleKind::BlackKingside:
		case CastleKind::BlackQueenside:
			return ChessPiece::BlackKing;
		}
	}

	constexpr BoardPos start_position() const {
		return color() == ChessPieceColor::White ? BoardPos::E1 : BoardPos::E8;
	}

	constexpr BoardPos end_position() const {
		switch(kind_) {
		case CastleKind::WhiteKingside:
			return BoardPos::G1;
		case CastleKind::WhiteQueenside:
			return BoardPos::C1;
		case CastleKind::BlackKingside:
			return BoardPos::G8;
		case CastleKind::BlackQueenside:
			return BoardPos::C8;
		}
	}

	constexpr std::optional<BoardPos> captured_position() const {
		return std::nullopt;
	}

	constexpr std::optional<ChessPiece> captured_piece() const {
		return std::nullopt;
	}

	constexpr std::optional<CommonMove> secondary_move() const {
		switch(kind_) {
		case CastleKind::WhiteKingside:
			return CommonMove{
				PositionalMove{"H1"_pos, "F1"_pos},
				ChessPiece::WhiteRook
			};
		case CastleKind::WhiteQueenside:
			return CommonMove{
				PositionalMove{"A1"_pos, "D1"_pos},
				ChessPiece::WhiteRook
			};
		case CastleKind::BlackKingside:
			return CommonMove{
				PositionalMove{"H8"_pos, "F8"_pos},
				ChessPiece::BlackRook
			};
		case CastleKind::BlackQueenside:
			return CommonMove{
				PositionalMove{"A8"_pos, "D8"_pos},
				ChessPiece::BlackRook
			};
		}
	}

	constexpr std::optional<ChessPiece> promotion() const {
		return std::nullopt;
	}

	friend constexpr bool operator==(CastleMove l, CastleMove r) {
		return l.kind_ == r.kind_;
	}

	friend constexpr bool operator!=(CastleMove l, CastleMove r) {
		return l.kind_ != r.kind_;
	}

private:

	constexpr ChessPieceColor color() const {
		switch(kind_) {
		case CastleKind::WhiteKingside:
		case CastleKind::WhiteQueenside:
			return ChessPieceColor::White;
		case CastleKind::BlackKingside:
		case CastleKind::BlackQueenside:
			return ChessPieceColor::Black;
		}
	}
	CastleKind kind_;
};

struct Move: private std::variant<CommonMove, EnPassantMove, CastleMove, PawnPromotionMove> {
private:
	using base_type = std::variant<CommonMove, EnPassantMove, CastleMove, PawnPromotionMove>;
public:

	using base_type::base_type;
	using base_type::operator=;

	constexpr ChessPiece moved_piece() const {
		return std::visit([](const auto& mv){ return mv.start_position(); }, as_variant());
	}

	constexpr BoardPos start_position() const {
		return std::visit([](const auto& mv){ return mv.start_position(); }, as_variant());
	}

	constexpr BoardPos end_position() const {
		return std::visit([](const auto& mv){ return mv.end_position(); }, as_variant());
	}

	constexpr std::optional<BoardPos> captured_position() const {
		return std::visit([](const auto& mv){ return mv.captured_position(); }, as_variant());
	}

	constexpr std::optional<ChessPiece> captured_piece() const {
		return std::visit([](const auto& mv){ return mv.captured_piece(); }, as_variant());
	}

	constexpr std::optional<CommonMove> secondary_move() const {
		return std::visit([](const auto& mv){ return mv.secondary_move(); }, as_variant());
	}

	constexpr std::optional<ChessPiece> promotion() const {
		return std::visit([](const auto& mv){ return mv.promotion(); }, as_variant());
	}

	constexpr MoveKind kind() const {
		if(is_common_move()) {
			return MoveKind::Common;
		} else if(is_en_passant_move()) {
			return MoveKind::EnPassant;
		} else if(is_castle_move()) {
			return MoveKind::Castle;
		} else {
			assert(is_pawn_promotion_move());
			return MoveKind::PawnPromotion;
		}
	}

	constexpr CastleMove as_castle_move() const {
		return std::get<CastleMove>(as_variant());
	}

	constexpr CommonMove as_common_move() const {
		return std::get<CommonMove>(as_variant());
	}

	constexpr EnPassantMove as_en_passant_move() const {
		return std::get<EnPassantMove>(as_variant());
	}

	constexpr PawnPromotionMove as_pawn_promotion_move() const {
		return std::get<EnPassantMove>(as_variant());
	}


	constexpr bool is_castle_move() const {
		return std::holds_alternative<CastleMove>(as_variant());
	}

	constexpr bool is_common_move() const {
		return std::holds_alternative<CommonMove>(as_variant());
	}

	constexpr bool is_en_passant_move() const {
		return std::holds_alternative<EnPassantMove>(as_variant());
	}

	constexpr bool is_pawn_promotion_move() const {
		return std::holds_alternative<PawnPromotionMove>(as_variant());
	}

	friend constexpr bool operator==(Move l, Move r) {
		return l.as_variant() == r.as_variant();
	}

	friend constexpr bool operator!=(Move l, Move r) {
		return l.as_variant() != r.as_variant();
	}

	friend std::string to_string(Move move) const {
		if(move.is_castle_move()) {
		}
	}

	std::string long_algebraic_notation() const {
		std::string enc;
	}

private:
	constexpr const std::variant<CommonMove, EnPassantMove, CastleMove>& as_variant() const {
		return *this;
	}

	constexpr std::variant<CommonMove, EnPassantMove, CastleMove>& as_variant() {
		return *this;
	}
};

} /* namespace ac */

#endif /* AC_MOVE_H */
