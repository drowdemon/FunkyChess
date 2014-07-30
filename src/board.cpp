#include <cstdlib>
#include <vector>
#include "../hdr/board.h"
#include "../hdr/piece.h"
#include "../hdr/square.h"
#include "../hdr/position.h"
#include "../hdr/blocker.h"

using namespace std;

board::board() : prevMove(), prevPosition()
{
	prevValue=0;
	check=2;
	data.resize(8);
	squareData.resize(8);
	for(int i=0; i<8; i++)
	{
		data[i].resize(8);
		squareData[i].resize(8);
		for(int j=0; j<8; j++)
		{
			data[i][j] = NULL;
			squareData[i][j] = new square();
		}
	}
	//data[0][0] = new rook(0,0,false);
	data[0][7] = new rook(7,0,false);
	/*data[0][1] = new knight(1,0,false);
	data[0][6] = new knight(6,0,false);
	data[0][2] = new bishop(2,0,false);
	data[0][5] = new bishop(5,0,false);
	data[0][3] = new queen(3,0,false);*/
	data[0][4] = new king(4,0,false);
	/*for(int i=0; i<8; i++)
	{
		data[1][i] = new pawn(i,1,false);
		if(data[0][i])
			positions[0].push_back(data[0][i]);
		positions[0].push_back(data[1][i]);
	}*/
	positions[0].push_back(data[0][7]);
	positions[0].push_back(data[0][4]);
	
	data[7][0] = new rook(0,7,true);
	data[7][7] = new rook(7,7,true);
	data[7][1] = new knight(1,7,true);
	data[7][6] = new knight(6,7,true);
	data[7][2] = new bishop(2,7,true);
	data[7][5] = new bishop(5,7,true);
	data[7][3] = new queen(3,7,true);
	data[7][4] = new king(4,7,true);
	for(int i=0; i<8; i++)
	{
		data[6][i] = new pawn(i,6,true);
		positions[1].push_back(data[7][i]);
		positions[1].push_back(data[6][i]);
	}
	
	//initialize possMoves, no need for castling check
	for(int i=0; i<2; i++)
	{
		for(unsigned int j=0; j<positions[i].size(); j++)
		{
			positions[i][j]->getPossMoves(*this);
		}
	}
}

board::board(const board &source) : prevMove()
{
	data.resize(8);
	squareData.resize(8);
	for(int i=0; i<8; i++)
	{
		data[i].resize(8);
		squareData[i].resize(8);
	}
	for(unsigned int i=0; i<data.size(); i++)
	{
		for(unsigned int j=0; j<data[i].size(); j++)
		{
			data[i][j]=piece::copyPiece(source.data[i][j]);
			squareData[i][j]=square::copySquare(source.squareData[i][j]);	
			if(data[i][j])
				positions[data[i][j]->color].push_back(data[i][j]);
		}
	}
	prevPosition = source.prevPosition;
	prevMove.copy(source.prevMove);
	prevValue = source.prevValue;
	check = source.check;
}

board::~board()
{
	for(int i=0; i<8; i++)
	{
		for(int j=0; j<8; j++)
		{
			for(int k=0; k<2; k++)
			{
				for(unsigned int h=0; h<squareData[i][j]->attackedBy[k].size(); h++)
					squareData[i][j]->attackedBy[k][h]=NULL;
			}
			delete squareData[i][j];
			if(data[i][j])
				delete data[i][j];
		}
	}
	if(prevMove.newtype)
		delete prevMove.newtype;
}

board& board::operator=(const board &source)
{
	data.resize(8);
	squareData.resize(8);
	for(int i=0; i<8; i++)
	{
		data[i].resize(8);
		squareData[i].resize(8);
	}
	for(unsigned int i=0; i<data.size(); i++)
	{
		for(unsigned int j=0; j<data[i].size(); j++)
		{
			data[i][j]=piece::copyPiece(source.data[i][j]);
			squareData[i][j]=square::copySquare(source.squareData[i][j]);
			if(data[i][j])
				positions[data[i][j]->color].push_back(data[i][j]);
		}
	}
	prevPosition = source.prevPosition;
	prevMove.copy(source.prevMove);
	prevValue = source.prevValue;
	check = source.check;

	return *this;
}

void board::advance(move *m, piece *moving)
{
	if(prevMove.newtype)
		delete prevMove.newtype;
	prevPosition = moving->p;
	prevMove.copy(*m);
	prevValue = moving->value;
	//actual movement
	if(m->taken)
	{
		for(unsigned int i=0; i<positions[m->taken->color].size(); i++)
		{
			if(m->taken==positions[m->taken->color][i])
			{
				positions[m->taken->color].erase(positions[m->taken->color].begin()+i);
				delete m->taken;
				m->taken=NULL;
				break;
			}
		}
	}
	data[moving->p.y][moving->p.x]=NULL;
	if(m->newtype) //pawn promotion
	{
		for(unsigned int i=0; i<positions[moving->color].size(); i++)
		{
			if(moving==positions[moving->color][i])
			{
				delete moving;
				moving=piece::copyPiece(m->newtype); //note: valgrind doesn't like this and spawns an error whenever I use moving ever again. 27 errors for each time this line is called.
				positions[moving->color][i]=moving;
				break;
			}
		}
	}
	data[m->to.y][m->to.x]=moving;
	if(m->castling)
	{
		if(m->to.x==2)
		{
			data[m->to.y][3]=data[m->to.y][0];
			data[m->to.y][0]=NULL;
			data[m->to.y][3]->p.x=3;
		}
		else //to.x==6
		{
			data[m->to.y][5]=data[m->to.y][7];
			data[m->to.y][7]=NULL;
			data[m->to.y][5]->p.x=5;
		}
	}
	moving->p.x=m->to.x;
	moving->p.y=m->to.y;
	
	if(moving->value==255)
		((king*)moving)->moved=true;
	if(moving->value==5)
		((rook*)moving)->moved=true;
	
	//update possmoves, squareData (not including castling, check)
	for(unsigned int i=0; i<squareData.size(); i++)
	{
		for(unsigned int j=0; j<squareData[i].size(); j++)
		{
			squareData[i][j]->attackedBy[0].clear();
			squareData[i][j]->attackedBy[1].clear();
		}
	}
	king *k[] = {NULL, NULL};
	for(int i=0; i<2; i++)
	{
		for(unsigned int j=0; j<positions[i].size(); j++)
		{
			if(positions[i][j]->value==255)
				k[i] = (king*)(positions[i][j]);
			positions[i][j]->getPossMoves(*this);
		}
	}
	
	//update check. Including removing moves of things that aren't allowed to move because the king would remain in check, but not including removing moves where the king moves into check. 
	unsigned char otherColor = moving->color==0 ? 1 : 0;
	if(squareData[k[otherColor]->p.y][k[otherColor]->p.x]->attackedBy[moving->color].size()!=0)
	{
		check=otherColor;
		//remove moves that would keep me in check, not including king moves
		/*vector<piece*> *attackers = &squareData[k[otherColor]->p.y][k[otherColor]->p.x]->attackedBy[moving->color]; //because that line is a lot of typing. And more pragmatically a lot of evaluation
		vector<position> **endCheck = new vector<position>*[attackers->size()];
		for(unsigned int i=0; i<attackers->size(); i++)
		{
			endCheck[i] = getBetween((piece*)k[otherColor], (*attackers)[i]);
		}
		if(attackers->size()>1)
		{
			int min=endCheck[0]->size();
			int minIndex=0;
			for(unsigned int i=1; i<attackers->size(); i++)
			{
				if(endCheck[i]->size()<min)
				{
					min=endCheck[i]->size();
					minIndex=i;
				}
			}
			if(minIndex!=0)
			{
				vector<position>* t = endCheck[minIndex];
				endCheck[minIndex] = endCheck[0];
				endCheck[0]=t;
			}
			for(int k=min-1; k>=0; k--) //the intersection of the sets in endcheck will be stored in endcheck[0] after this loop is executed. sets like math, not like data structures.
			{
				for(unsigned int i=1; i<attackers->size(); i++)
				{
					bool good=false;
					for(unsigned int j=0; j<endCheck[i]->size(); j++)
					{
						if((*endCheck[0])[k].x==(*endCheck[i])[j].x && (*endCheck[0])[k].y==(*endCheck[i])[j].y)
						{
							good=true;
							break;
						}
					}
					if(!good)
					{
						endCheck[0]->erase(endCheck[0]->begin()+k);
						k++;
						break;
					}
				}
			}
		}*/
		if(squareData[k[otherColor]->p.y][k[otherColor]->p.x]->attackedBy[moving->color].size()==1) //if there's a check from 2+ pieces simultaneously it cannot be blocked
		{
			vector<position> *endCheck = NULL;
			piece* attacker = squareData[k[otherColor]->p.y][k[otherColor]->p.x]->attackedBy[moving->color][0];
			if(attacker->isKnight || attacker->value==1)
			{
				endCheck = new vector<position>;
				endCheck->push_back(attacker->p);
			}
			else
				endCheck = getBetween(k[otherColor]->p, attacker->p);
			for(unsigned int i=0; i<positions[otherColor].size(); i++)
			{
				if(positions[otherColor][i]->value!=255)
				{
					for(unsigned int j=0; j<positions[otherColor][i]->possMoves.size(); j++)
					{
						bool good = false;
						for(unsigned int k=0; k<endCheck->size(); k++)
						{
							if(positions[otherColor][i]->possMoves[j].to.x==(*endCheck)[k].x && positions[otherColor][i]->possMoves[j].to.y==(*endCheck)[k].y)
							{
								good = true;
								break;
							}
						}
						if(!good)
						{
							positions[otherColor][i]->removeMove(j, *this);
							j--;
						}
					}
				}				
			}
			delete endCheck;
		}
	}
	else
		check=2;
	
	//check for castling
	k[0]->getPossMoves(*this, true);
	k[1]->getPossMoves(*this, true);
	
	//update blocking (including inability to move because blocking check)
	for(int i=0; i<2; i++)
	{
		for(unsigned int j=0; j<positions[i].size(); j++)
		{
			for(int f=0; f<2; f++)
			{
				for(int g=squareData[positions[i][j]->p.y][positions[i][j]->p.x]->attackedBy[f].size()-1; g>=0; g--)
				{
					blocker *t = getBlock(positions[i][j], squareData[positions[i][j]->p.y][positions[i][j]->p.x]->attackedBy[f][g]);
					if(t)
					{
						positions[i][j]->blocking[f].push_back(t);
						//removing moves of a piece pinned to the king, and adding them to square::pinAttackBy
						if(f!=i && t->defended && t->defended->value==255 && t->defended->color==i) //blocking an actual attack on a piece and the piece is a king and it's not the opponents king. i is equivalent to positions[i][j]->color, f is the color of the 'attacking' piece
						{
							for(unsigned int h=0; h<positions[i][j]->possMoves.size(); h++)
							{
								bool good=false;
								for(unsigned int e=0; e<t->blocked.size(); e++)
								{
									if(positions[i][j]->possMoves[h].to.x==t->blocked[e].x && positions[i][j]->possMoves[h].to.y==t->blocked[e].y)
									{
										good=true;
										break;
									}
								}
								vector<position> *between = getBetween(positions[i][j]->p, t->attacker->p);
								for(unsigned int e=0; e<between->size(); e++)
								{
									if(positions[i][j]->possMoves[h].to.x==(*between)[e].x && positions[i][j]->possMoves[h].to.y==(*between)[e].y)
									{
										good=true;
										break;
									}
								}
								if(!good && (positions[i][j]->possMoves[h].to.y != k[i==0?1:0]->p.y || positions[i][j]->possMoves[h].to.x != k[i==0?1:0]->p.x)) //not already approved, and it's not the opponents king, which I'm putting in check. If the second part of that ever happens I'll be in shock. blocking a check while delivering a check with the same piece, same turn, is quite a feat.
								{
									squareData[positions[i][j]->possMoves[h].to.y][positions[i][j]->possMoves[h].to.x]->pinAttackBy[positions[i][j]->color].push_back(positions[i][j]);
									positions[i][j]->removeMove(h, *this);
									h--;
								}
								delete between;
							}
						}
					}
				}
			}
		}
	}
	
	//remove king moves that would keep me in check (has to happen after blocking is computed
	for(unsigned int i=0; i<k[otherColor]->possMoves.size(); i++)
	{
		if(squareData[k[otherColor]->possMoves[i].to.y][k[otherColor]->possMoves[i].to.x]->attackedBy[moving->color].size()!=0 || squareData[k[otherColor]->possMoves[i].to.y][k[otherColor]->possMoves[i].to.x]->pinAttackBy[moving->color].size()!=0) //it's being attacked, including by a pinned piece
		{
			k[otherColor]->removeMove(i,*this);
			i--;
			continue;
		}
		bool good=true;
		for(unsigned int j=0; j<k[otherColor]->blocking[moving->color].size(); j++) //It's a move onto a square that I am currently blocking from being attacked. So if I move there, I will no longer be blocking that move, I'll be being attacked on that square.
		{
			for(unsigned int h=0; h<k[otherColor]->blocking[moving->color][j]->blocked.size(); h++)
			{
				if(k[otherColor]->possMoves[i].to.x == k[otherColor]->blocking[moving->color][j]->blocked[h].x && k[otherColor]->possMoves[i].to.y == k[otherColor]->blocking[moving->color][j]->blocked[h].y)
				{
					k[otherColor]->removeMove(i, *this);
					i--;
					good=false;
					break;
				}
			}
			if(!good)
				break;
		}
	}
}

blocker* board::getBlock(piece* defender, piece* attacker)
{
	if(attacker->isKnight || attacker->value==1 || attacker->value==255)
		return NULL;
	blocker *ret = new blocker();
	ret->attacker=attacker;
	if(attacker->p.x==defender->p.x) //rook style, vertical
	{
		int direction = (defender->p.y-attacker->p.y)/abs(defender->p.y-attacker->p.y);
		for(int i=1; defender->p.y + i*direction >= 0 && defender->p.y + i*direction <= 7; i++)
		{
			ret->blocked.push_back(position(defender->p.x, defender->p.y+i*direction));
			if(data[defender->p.y+i*direction][defender->p.x])
			{
				ret->defended = data[defender->p.y+i*direction][defender->p.x];
				break;
			}
		}
	}
	else if(attacker->p.y==defender->p.y) //rook style, horizontal
	{
		int direction = (defender->p.x-attacker->p.x)/abs(defender->p.x-attacker->p.x);
		for(int i=1; defender->p.x + i*direction >= 0 && defender->p.x + i*direction <= 7; i++)
		{
			ret->blocked.push_back(position(defender->p.x+i*direction, defender->p.y));
			if(data[defender->p.y][defender->p.x+i*direction])
			{
				ret->defended = data[defender->p.y][defender->p.x+i*direction];
				break;
			}
		}
	}
	else if((attacker->p.x-defender->p.x) == (attacker->p.y-defender->p.y)) //positive slope diagonal (Note to self: positive slope diagonals look like what I normally envision as negative slope diagonals and vice versa. Also, I can simplified the calculation a little because the slope is always +-1)
	{
		int direction = (defender->p.x-attacker->p.x)/abs(defender->p.x-attacker->p.x);
		for(int i=1; defender->p.x + i*direction >= 0 && defender->p.x + i*direction <= 7 && defender->p.y + i*direction >= 0 && defender->p.y + i*direction <= 7; i++)
		{
			ret->blocked.push_back(position(defender->p.x+i*direction, defender->p.y+i*direction));
			if(data[defender->p.y+i*direction][defender->p.x+i*direction])
			{
				ret->defended = data[defender->p.y+i*direction][defender->p.x+i*direction];
				break;
			}
		}
	}
	else //negative slope diagonal
	{
		int direction = (defender->p.x-attacker->p.x)/abs(defender->p.x-attacker->p.x);
		for(int i=1; defender->p.x + i*direction >= 0 && defender->p.x + i*direction <= 7 && defender->p.y - i*direction >= 0 && defender->p.y - i*direction <= 7; i++)
		{
			ret->blocked.push_back(position(defender->p.x+i*direction, defender->p.y-i*direction));
			if(data[defender->p.y-i*direction][defender->p.x+i*direction])
			{
				ret->defended = data[defender->p.y-i*direction][defender->p.x+i*direction];
				break;
			}
		}
	}
	return ret;
}
vector<position>* board::getBetween(position defender, position attacker) //getblock searches past the defending piece, this searches in between. Only real difference is that direction is negated.
{
	vector<position> *ret = new vector<position>;
	//ret->push_back(attacker); //add the position of the attacking piece. Not actually in between, but it will end the check. That'll happen later anyway. Oops.
	if(attacker.x==defender.x) //rook style, vertical
	{
		int direction = (attacker.y-defender.y)/abs(defender.y-attacker.y);
		for(int i=1; defender.y + i*direction >= 0 && defender.y + i*direction <= 7; i++)
		{
			ret->push_back(position(defender.x, defender.y+i*direction));
			if(data[defender.y+i*direction][defender.x])
			{
				break;
			}
		}
	}
	else if(attacker.y==defender.y) //rook style, horizontal
	{
		int direction = (attacker.x-defender.x)/abs(defender.x-attacker.x);
		for(int i=1; defender.x + i*direction >= 0 && defender.x + i*direction <= 7; i++)
		{
			ret->push_back(position(defender.x+i*direction, defender.y));
			if(data[defender.y][defender.x+i*direction])
			{
				break;
			}
		}
	}
	else if((attacker.x-defender.x) == (attacker.y-defender.y)) //positive slope diagonal (Note to self: positive slope diagonals look like what I normally envision as negative slope diagonals and vice versa. Also, I can simplified the calculation a little because the slope is always +-1)
	{
		int direction = (attacker.x-defender.x)/abs(defender.x-attacker.x);
		for(int i=1; defender.x + i*direction >= 0 && defender.x + i*direction <= 7 && defender.y + i*direction >= 0 && defender.y + i*direction <= 7; i++)
		{
			ret->push_back(position(defender.x+i*direction, defender.y+i*direction));
			if(data[defender.y+i*direction][defender.x+i*direction])
			{
				break;
			}
		}
	}
	else //negative slope diagonal
	{
		int direction = (attacker.x-defender.x)/abs(defender.x-attacker.x);
		for(int i=1; defender.x + i*direction >= 0 && defender.x + i*direction <= 7 && defender.y - i*direction >= 0 && defender.y - i*direction <= 7; i++)
		{
			ret->push_back(position(defender.x+i*direction, defender.y-i*direction));
			if(data[defender.y-i*direction][defender.x+i*direction])
			{
				break;
			}
		}
	}
	return ret;
}
