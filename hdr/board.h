#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include "move.h"
#include "position.h"

using namespace std;

class piece;
class square;
class blocker;

class board
{
public:
	vector<vector<piece*> > data;
	vector<vector<square*> > squareData;
	vector<piece*> positions[2];
	move prevMove; //these three are specifically for en passant.
	position prevPosition;
	unsigned char prevValue;
	unsigned char check; //it equals the color of the player in check. If there exists no check, it's 2
	board();
	board(const board &source);
	~board();
	board& operator=(const board &source);
	void advance(move *m, piece *moving);
	blocker* getBlock(piece *defender, piece *attacker);
	vector<position>* getBetween(position defender, position attacker);
};

#endif /* BOARD_H_ */
