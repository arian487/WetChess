/*
 * Square.cpp
 *
 *  Created on: Nov 12, 2011
 *      Author: a3karbas
 */

#include "Square.hpp"

Square::Square(int pos) {

	//set up whites pieces
	if (pos >= 0 && pos <= 15) {
		if (pos == 0 || pos == 7) {
			piece = new Piece(pos);
			piece->set_type(Piece::ROOK);
		}
		else if (pos == 1 || pos == 6) {
			piece = new Piece(pos);
			piece->set_type(Piece::KNIGHT);
		}
		else if (pos == 2 || pos == 5) {
			piece = new Piece(pos);
			piece->set_type(Piece::BISHOP);
		}
		else if (pos == 3) {
			piece = new Piece(pos);
			piece->set_type(Piece::QUEEN);
		}
		else if (pos == 4) {
			piece = new Piece(pos);
			piece->set_type(Piece::KING);
		}
		else if (pos >= 8 && pos <= 15) {
			piece = new Piece(pos);
			piece->set_type(Piece::PAWN);
		}
		piece->colour = 1;
	}
	//set the black pieces
	else if (pos >= 48 && pos <= 63) {
		if (pos == 56 || pos == 63) {
			piece = new Piece(pos);
			piece->set_type(Piece::ROOK);
		}
		else if (pos == 57 || pos == 62) {
			piece = new Piece(pos);
			piece->set_type(Piece::KNIGHT);
		}
		else if (pos == 58 || pos == 61) {
			piece = new Piece(pos);
			piece->set_type(Piece::BISHOP);
		}
		else if (pos == 59) {
			piece = new Piece(pos);
			piece->set_type(Piece::QUEEN);
		}
		else if (pos == 60) {
			piece = new Piece(pos);
			piece->set_type(Piece::KING);
		}
		else if (pos >= 48 && pos <= 55) {
			piece = new Piece(pos);
			piece->set_type(Piece::PAWN);
		}
		piece->colour = -1;
	}
	else {
		piece = NULL;
	}
	position = pos;

}

void Square::remove_piece()
{
	if (piece != NULL)
	{
		piece = NULL;
	}
}

Square::~Square() {
	if (piece != NULL)
		delete(piece);
}
