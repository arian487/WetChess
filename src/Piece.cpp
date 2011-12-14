/*
 * Piece.cpp
 *
 *  Created on: Nov 12, 2011
 *      Author: a3karbas
 */

#include "Piece.hpp"

Piece::Piece(int pos) {
	has_moved = false;
	position = pos;
}

Piece::~Piece() {
	// TODO Auto-generated destructor stub
}

void Piece::set_type(PIECE_TYPE piece_type)
{
	type = piece_type;
}
