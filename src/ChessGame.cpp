/*
 * ChessGame.cpp
 *
 *  Created on: Nov 12, 2011
 *      Author: a3karbas
 */

#include "ChessGame.hpp"
#include <vector>

ChessGame::ChessGame() {
	//starts as whites turn
	turn = 1;
	is_black_in_check = false;
	is_white_in_check = false;
	set_default_positions();

}

ChessGame::~ChessGame() {
	// TODO Auto-generated destructor stub
}

//when starting the game, lets set the positions for all the pieces
void ChessGame::set_default_positions()
{
	for (int i = 0; i < 64; i++) {
		//create a piece with a default position
		Square *square = new Square(i);
		squares[i] = square;
	}
}

ChessGame::MOVE_OUTCOMES ChessGame::move (int old_pos, int new_pos)
{
	MOVE_OUTCOMES result;
	Square *cur_piece = squares[old_pos];
	if (cur_piece->piece == NULL)
		return NO_PIECE_SELECTED;
	Square *new_square = squares[new_pos];

	if (cur_piece->piece->colour != turn) {
		return NOT_YOUR_TURN;
	}

	if (new_square->piece != NULL && cur_piece->piece->colour == new_square->piece->colour) {
		return GOING_OVER_TEAM;
	}

	if (!can_piece_go(cur_piece, new_pos)) {
		return PIECE_CANT_GO;
	}

	if (cur_piece->piece->type == Piece::KING) {
		if (moving_into_check(cur_piece, new_pos)) {
			return MOVING_INTO_CHECK;
		}
		else if (new_square->piece != NULL && cur_piece->piece->colour == -(new_square->piece->colour)) {
			//this square no longer has a piece on it
			Piece* temp = cur_piece->piece;
			temp->has_moved = true;
			turn *= -1;
			cur_piece->piece = NULL;
			taken_pieces.push_front(new_square->piece);
			new_square->piece = temp;
			return TAKEN;
		}
		else {
			Piece* temp = cur_piece->piece;
			temp->has_moved = true;
			turn *= -1;
			cur_piece->piece = NULL;
			new_square->piece = temp;
			return SUCCESS;
		}
	}
	if (turn == 1 && is_white_checked(cur_piece, new_pos)) {
		return MOVING_INTO_CHECK;
	}
	if (turn == -1 && is_black_checked(cur_piece, new_pos)) {
		return MOVING_INTO_CHECK;
	}
	//now we have established the piece CAN go to that location
	//lets see if there is a check or check_mate
	result = is_check_or_mate(cur_piece, new_pos);
	if (new_square->piece != NULL && cur_piece->piece->colour == -(new_square->piece->colour)){
		std::cout<<"in here"<<std::endl;
		Piece* temp = cur_piece->piece;
		temp->has_moved = true;
		turn *= -1;
		cur_piece->piece = NULL;
		taken_pieces.push_front(new_square->piece);
		new_square->piece = temp;
		if (result == NO_CHECK_OR_MATE)
			result = TAKEN;
	}
	else
	{
		Piece* temp = cur_piece->piece;
		temp->has_moved = true;
		turn *= -1;
		cur_piece->piece = NULL;
		new_square->piece = temp;
		if (result == NO_CHECK_OR_MATE)
			result = SUCCESS;
	}
	if (result != NO_CHECK_OR_MATE)
		return result;
	else return SUCCESS;
}

bool ChessGame::is_black_checked(Square* square, int new_pos)
{
	return false;
}

bool ChessGame::is_white_checked(Square* square, int new_pos)
{
	return false;
}

bool ChessGame::moving_into_check(Square *piece, int new_pos)
{
	return false;
}

//initial check to see if the piece can even move to that spot
bool ChessGame::can_piece_go(Square *piece, int new_pos)
{
	int pos = piece->position;
	Piece* game_piece = piece->piece;

	if (game_piece->type == Piece::PAWN) {
		//rules for white pawn and black pawn are different
		if (turn == 1) {
			int adder = 0;
			if (!game_piece->has_moved) {
				adder = 16;
			}
			if (pos + 8 != new_pos && pos + adder != new_pos) {
				//ok so it wasn't a valid pawn move, but maybe it can take a piece
				//diagonally
				int diff = new_pos - pos;

				if (diff != 9 && diff != 7) return false;

				if (diff == 9 && pos % 8 != 7) {
					std::cout<<"got in here"<<std::endl;
					Square* test_square = squares[new_pos];
					if (test_square->piece == NULL) {
						return false;
					}
				}
				else if (diff == 9 && pos % 8 == 7) {
					return false;
				}
				else if (diff == 7 && pos % 8 != 0) {
					Square* test_square = squares[new_pos];
					if (test_square->piece == NULL) {
						return false;
					}
				}
				else if (diff == 7 && pos % 8 != 0) {
					return false;
				}

			}
			else {
				//make sure we're not going over a piece
				Square* test_square = squares[new_pos];
				if (test_square->piece != NULL) {
					//std::cout<<"the difference is "<<diff <<std::endl;
					return false;
				}
			}
		}
		else if (turn == -1) {
			int adder = 0;
			if (!game_piece->has_moved) {
				adder = 16;
			}
			if (pos - 8 != new_pos && pos - adder != new_pos) {
				//ok so it wasn't a valid pawn move, but maybe it can take a piece
				//diagonally
				int diff = (new_pos - pos) * turn;

				if (diff != 9 && diff != 7) return false;

				if (diff == 9 && pos % 8 != 0) {
					std::cout<<"got in here"<<std::endl;
					Square* test_square = squares[new_pos];
					if (test_square->piece == NULL) {
						return false;
					}
				}
				else if (diff == 9 && pos % 8 == 0) {
					return false;
				}
				else if (diff == 7 && pos % 8 != 7) {
					Square* test_square = squares[new_pos];
					if (test_square->piece == NULL) {
						return false;
					}
				}
				else if (diff == 7 && pos % 8 != 7) {
					return false;
				}

			}
			else {
				//make sure we're not going over a piece
				Square* test_square = squares[new_pos];
				if (test_square->piece != NULL) {
					//std::cout<<"the difference is "<<diff <<std::endl;
					return false;
				}
			}
		}
	}
	else if (game_piece->type == Piece::ROOK) {
		if (pos % 8 == new_pos % 8) {
			//on same file, lets check if anything is blocking
			int adder = 8;
			if (new_pos < pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
			}
		}
		else if (pos / 8 == new_pos / 8) {
			int adder = 1;
			if (new_pos <  pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
			}
		}
		else return false;
	}
	else if (game_piece->type == Piece::BISHOP) {
		if ((new_pos - pos) % 9 == 0) {

			int adder = 9;
			if (new_pos < pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
				if (i % 8 == 7) return false;
			}
		}
		else if ((new_pos - pos) % 7 == 0) {

			int adder = 7;
			if (new_pos < pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
				if (i % 8 == 0) return false;
			}
		}
		else return false;
	}
	else if (game_piece->type == Piece::KNIGHT) {
		if (abs(pos % 8 - new_pos % 8) > 2) {
			return false;
		}
		else {
			if (pos + 17 != new_pos && pos + 10 != new_pos && pos + 15 != new_pos && pos + 6 != new_pos
					&& pos - 6 != new_pos && pos - 15 != new_pos && pos - 17 != new_pos && pos - 10 != new_pos) {
				return false;
			}
		}
	}
	else if (game_piece->type == Piece::QUEEN) {
		if (pos % 8 == new_pos % 8) {
			//on same file, lets check if anything is blocking
			int adder = 8;
			if (new_pos < pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
			}
		}
		else if (pos / 8 == new_pos / 8) {
			int adder = 1;
			if (new_pos <  pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
			}
		}
		else if ((new_pos - pos) % 9 == 0) {

			int adder = 9;
			if (new_pos < pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
				if (i % 8 == 7) return false;
			}
		}
		else if ((new_pos - pos) % 7 == 0) {

			int adder = 7;
			if (new_pos < pos) {
				adder *= -1;
			}
			for (int i = pos+adder; i != new_pos; i+=adder) {
				Square* test_square = squares[i];
				if (test_square->piece != NULL) {
					return false;
				}
				if (i % 8 == 0) return false;
			}
		}
		else return false;
	}
	else if (game_piece->type == Piece::KING) {
		if (abs(pos % 8 - new_pos % 8) > 1) {
			return false;
		}
		else {
			if (pos + 1 != new_pos && pos + 8 != new_pos && pos + 7 != new_pos && pos + 9 != new_pos
					&& pos - 1 != new_pos && pos - 8 != new_pos && pos - 7 != new_pos && pos - 9 != new_pos) {
				return false;
			}
		}
	}

	return true;
}

bool ChessGame::is_pinned(Square* piece, int new_pos)
{
	return false;
}

ChessGame::MOVE_OUTCOMES ChessGame::is_check_or_mate(Square *square, int new_pos)
{
	int pos = square->position;
	Piece* game_piece = square->piece;
	std::vector<int> attacking_squares;
	if (game_piece->type == Piece::PAWN) {
		if (new_pos % 8 == 7) {
			attacking_squares.push_back(new_pos + (7*turn));
		}
		else if (new_pos % 8 == 0) {
			attacking_squares.push_back(new_pos + (9*turn));
		}
		else {
			attacking_squares.push_back(new_pos + (7*turn));
			attacking_squares.push_back(new_pos + (9*turn));
		}
	}
	else if (game_piece->type == Piece::ROOK) {
		for (int i = new_pos + 8; i <= 63; i+=8) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos - 8; i >= 0; i-=8) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos + 1; i / 8 == new_pos / 8; i++) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos - 1; i / 8 == new_pos / 8; i--) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
	}
	else if (game_piece->type == Piece::BISHOP) {
		for (int i = new_pos + 9; (i <= 63) && (i % 8 != 0); i+=9) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}

		for (int i = new_pos + 7; (i <= 63) && (i % 8 != 7); i+=7) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}

		for (int i = new_pos - 9; (i >= 0) && (i % 8 != 7); i-=9) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos - 7; (i >= 0) && (i % 8 != 0); i-=7) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
	}
	else if (game_piece->type == Piece::QUEEN) {
		for (int i = new_pos + 9; (i <= 63) && (i % 8 != 0); i+=9) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}

		for (int i = new_pos + 7; (i <= 63) && (i % 8 != 7); i+=7) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}

		for (int i = new_pos - 9; (i >= 0) && (i % 8 != 7); i-=9) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos - 7; (i >= 0) && (i % 8 != 0); i-=7) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}

		for (int i = new_pos + 8; i <= 63; i+=8) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos - 8; i >= 0; i-=8) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos + 1; i / 8 == new_pos / 8; i++) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
		for (int i = new_pos - 1; i / 8 == new_pos / 8; i--) {
			Square* test_square = squares[i];
			if (test_square->piece == NULL || (test_square->piece->type == Piece::KING
					&& test_square->piece->colour != turn)) {
				attacking_squares.push_back(i);
			}
			else break;
		}
	}
	else if (game_piece->type == Piece::KNIGHT) {
		int cur_square = new_pos + 17;

		if (cur_square <= 63 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos + 15;
		if (cur_square <= 63 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos + 10;
		if (cur_square <= 63 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos + 6;
		if (cur_square <= 63 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos - 17;
		if (cur_square >= 0 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos - 15;
		if (cur_square >= 0 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos - 10;
		if (cur_square >= 0 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}

		cur_square = new_pos - 6;
		if (cur_square >= 0 && abs(cur_square%8 - new_pos%8) <=2 ) {
			attacking_squares.push_back(cur_square);
		}
	}

	//Ok so we finally figured out what the attacking squares of the moving piece are
	for (int i = 0; i < attacking_squares.size(); i++) {
		int attacked_pos = attacking_squares[i];
		Square* attacked_square = squares[attacked_pos];
		//check if one of the attacked squares contains the enemy king
		if (attacked_square->piece != NULL && attacked_square->piece->type == Piece::KING
				&& attacked_square->piece->colour != turn) {
			return CHECK;
		}
	}

	return NO_CHECK_OR_MATE;
}

void ChessGame::draw_board()
{
	for (int i = 63; i >= 0; i--) {
		Square *square = squares[i];
		if (square->piece == NULL) {
			std::cout<<"X(  )     ";
		}
		else {
			std::cout<< square->piece->type << "("<<i<<")     ";
		}
		if (i%8 == 0) {
			std::cout<<std::endl;
		}
	}
}
