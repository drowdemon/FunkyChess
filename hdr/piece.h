#ifndef PIECE_H_
#define PIECE_H_

#include <vector>
#include "position.h"

using namespace std;

class move;
class board;
class blocker;

class piece
{
public:
	position p;
	unsigned char color; //white is 1 black is 0
	unsigned char value; //pawn 1, bishop/knight 3, etc. total of all not including king = 39.
	bool isKnight; //true only if it's a knight. To differentiate between knights and bishops for copying purposes
	vector<move> possMoves;
	vector<blocker*> blocking[2];
	virtual void getPossMoves(board &game) = 0;
	static piece *copyPiece(piece *source);
	void addMove(move m, board &game);
	void removeMove(int index, board &game);
	piece();
	piece(unsigned char xpos, unsigned char ypos, bool c, unsigned char value, bool k);
	virtual ~piece();
};

class pawn : public piece
{
public:
	void getPossMoves(board &game);
	void addPromotion(board &game);
	pawn(int px=9, int py=9, int c=false);
	~pawn();
};
class rook : public piece
{
public:
	bool moved; //for castling
	void getPossMoves(board &game);
	rook(int px=9, int py=9, int c=false, bool m=false);
	bool checkSquare(board &game, unsigned char x, unsigned char y);
};
class knight : public piece
{
public:
	void getPossMoves(board &game);
	knight(int px=9, int py=9, int c=false);
	void checkSquare(board &game, unsigned char x, unsigned char y);
};
class bishop : public piece
{
public:
	void getPossMoves(board &game);
	bishop(int px=9, int py=9, int c=false);
	bool checkSquare(board &game, unsigned char x, unsigned char y);
};
class queen : public piece
{
public:
	bool checkRook(board &game, unsigned char x, unsigned char y);
	bool checkBishop(board &game, unsigned char x, unsigned char y);
	void getPossMoves(board &game);
	queen(int px=9, int py=9, int c=false);
};
class king : public piece
{
public:
	bool moved; //for castling
	void getPossMoves(board &game);
	void getPossMoves(board &game, bool castlingCheck);
	king(int px=9, int py=9, int c=false, bool m=false);
	void checkSquare(board &game, unsigned char x, unsigned char y);
};

#endif /* PIECE_H_ */
