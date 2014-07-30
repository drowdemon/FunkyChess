#ifndef MOVE_H_
#define MOVE_H_

#include "position.h"

class piece;

class move
{
public:
	position to;
	piece *taken; //not necessarily on the same square as the tox,toy. En passant.
	//Fucking special cases.
	piece *newtype; //used only for pawn promotion. If this is not null, then newtype is what the piece at the position tox,toy really is, i.e. a queen. Otherwise it remains whatever it started as.
	bool castling; //Know to teleport the rook.
	move();
	move(unsigned char px, unsigned char py);
	//~move();
	//move& operator=(const move &source);
	void copy(const move &source);
};

#endif /* MOVE_H_ */
