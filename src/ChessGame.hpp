/*
 * ChessGame.h
 *
 *  Created on: Nov 12, 2011
 *      Author: a3karbas
 */

#ifndef CHESSGAME_H_
#define CHESSGAME_H_

#include "Piece.hpp"
#include "Square.hpp"
#include <iostream>
#include <list>

class ChessGame {
public:

	enum MOVE_OUTCOMES{
		SUCCESS,
		MOVING_INTO_CHECK,
		PINNED,
		CASTLE,
		PAWN_KINGED,
		CHECK,
		CHECKMATE,
		GOING_OVER_TEAM,
		TAKEN,
		PIECE_CANT_GO,
		NO_PIECE_SELECTED,
		IN_CHECK,
		NOT_YOUR_TURN,
		NO_CHECK_OR_MATE
	};
	ChessGame();
	virtual ~ChessGame();
	//returns whether or not the move was successful
	virtual MOVE_OUTCOMES move (int old_pos, int new_pos);
	//for debugging
	virtual void draw_board();

private:
	virtual void set_default_positions();
	virtual bool moving_into_check(Square *piece, int new_pos);
	virtual bool can_piece_go(Square* piece, int new_pos);
	virtual bool is_pinned(Square*piece, int new_pos);
	virtual bool is_white_checked(Square* piece, int new_pos);
	virtual bool is_black_checked(Square* piece, int new_pos);
	virtual MOVE_OUTCOMES is_check_or_mate(Square *square, int new_pos);

	Square *squares[64];
	std::list<Piece*> taken_pieces;


	//the turn of the player
	int turn;
	bool is_black_in_check;
	bool is_white_in_check;

};

#endif /* CHESSGAME_H_ */
