/*
 * Square.h
 *
 *  Created on: Nov 12, 2011
 *      Author: a3karbas
 */

#ifndef SQUARE_H_
#define SQUARE_H_

#include "Piece.hpp"
#include <stdlib.h>


class Square {
public:
	Square(int pos);
	virtual ~Square();
	Piece* piece;
	int position;

	//remove the piece on this square
	virtual void remove_piece();
};

#endif /* SQUARE_H_ */
