/*
 * Piece.h
 *
 *  Created on: Nov 12, 2011
 *      Author: a3karbas
 */

#ifndef PIECE_H_
#define PIECE_H_

class Piece {
public:
	enum PIECE_TYPE {
		ROOK,
		KING,
		QUEEN,
		KNIGHT,
		BISHOP,
		PAWN,
		NONE,
	};
	Piece(int pos);
	virtual ~Piece();

	PIECE_TYPE type;
	int colour;
	int position;
	bool has_moved;

	virtual void set_type(PIECE_TYPE piece_type);
};

#endif /* PIECE_H_ */
