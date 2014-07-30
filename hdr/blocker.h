#ifndef BLOCKER_H_
#define BLOCKER_H_

#include <vector>
#include "position.h"

using namespace std; 

class piece;

class blocker
{
public:
	piece *attacker;
	piece *defended;
	vector<position> blocked;
};

#endif /* BLOCKER_H_ */
