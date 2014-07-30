#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

using namespace std;

class piece;

class square
{
public:
	vector<piece*> attackedBy[2];
	vector<piece*> pinAttackBy[2];
	square();
	static square* copySquare(square *source);
};

#endif /* SQUARE_H_ */
