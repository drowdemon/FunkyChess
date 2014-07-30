#include <vector>
#include <cstdlib>
#include "../hdr/piece.h"
#include "../hdr/move.h"
#include "../hdr/board.h"
#include "../hdr/square.h"
#include "../hdr/blocker.h"

piece::piece() : p()
{
	color=false;
	value=0;
	isKnight=false;
}
piece::piece(unsigned char px, unsigned char py, bool c, unsigned char val, bool k) : p(px, py)
{
	color=c;
	value=val;
	isKnight=k;
}
piece::~piece()
{
	for(int j=0; j<2; j++)
	{
		for(unsigned int i=0; i<blocking[j].size(); i++)
		{
			if(blocking[j][i])
				delete blocking[j][i];
		}
	}
}
pawn::pawn(int px, int py, int c) : piece(px,py,c,1,false)
{
}
pawn::~pawn()
{
	for(unsigned int i=0; i<possMoves.size(); i++)
	{
		if(possMoves[i].newtype)
			delete possMoves[i].newtype;
	}
}
rook::rook(int px, int py, int c, bool m) : piece(px,py,c,5,false)
{
	moved=m;
}
knight::knight(int px, int py, int c) : piece(px,py,c,3,true)
{
}
bishop::bishop(int px, int py, int c) : piece(px,py,c,3,false)
{
}
queen::queen(int px, int py, int c) : piece(px,py,c,9,false)
{
}
king::king(int px, int py, int c, bool m) : piece(px,py,c,255,false)
{
	moved=m;
}

void pawn::addPromotion(board &game)
{
	
	unsigned int s=possMoves.size()-1;
	for(int i=0; i<3; i++)
		addMove(possMoves.back(), game);
	possMoves[s].newtype = new rook(possMoves[s].to.x, possMoves[s].to.y, color);
	possMoves[s+1].newtype = new knight(possMoves[s].to.x, possMoves[s].to.y, color);
	possMoves[s+2].newtype = new bishop(possMoves[s].to.x, possMoves[s].to.y, color);
	possMoves[s+3].newtype = new queen(possMoves[s].to.x, possMoves[s].to.y, color);
}
void pawn::getPossMoves(board &game)
{
	for(int i=0; i<possMoves.size(); i++)
	{
		if(possMoves[i].newtype)
			delete possMoves[i].newtype;
	}
	possMoves.clear();
	int add = (color==1) ? -1 : 1;
	
	if(!game.data[p.y + add][p.x]) //move up 1
	{
		addMove(move(p.x,p.y + add), game);
		if(p.y+add==0 || p.y+add==7)
			addPromotion(game);

		if(((p.y == 1 && color==0) || (p.y==6 && color==1)) && game.data[p.y+2*add][p.x]==NULL) //move up 2 on first turn, inside moving up 1 because the square directly in front of the pawn must be empty
		{
			addMove(move(p.x,p.y+2*add), game);
		}
	}
	if(p.x > 0 && game.data[p.y+add][p.x-1]) //take left
	{
		if(game.data[p.y+add][p.x-1]->color!=color)
		{
			addMove(move(p.x-1,p.y+add), game);
			possMoves.back().taken=game.data[p.y+add][p.x-1];
			if(p.y+add==0 || p.y+add==7)
				addPromotion(game);
		}
		else
			game.squareData[p.y+add][p.x+add]->attackedBy[color].push_back(this); //defending my piece
	}
	if(p.x < 7 && game.data[p.y+add][p.x+1]) //take right
	{
		if(game.data[p.y+add][p.x+1]->color!=color)
		{
			addMove(move(p.x+1,p.y+add), game);
			possMoves.back().taken=game.data[p.y+add][p.x+1];
			if(p.y+add==0 || p.y+add==7)
				addPromotion(game);
		}
		else
			game.squareData[p.y+add][p.x+add]->attackedBy[color].push_back(this); //defending my piece
	}
	if(game.prevValue==1 && ((game.prevMove.to.y==3 && color==1 && game.prevPosition.y==1) || (game.prevMove.to.y==4 && color==0 && game.prevPosition.y==6))) //en passant //the previous move was a move up 2 by a pawn
	{
		if((p.x-1==game.prevMove.to.x || p.x+1==game.prevMove.to.x) && p.y==game.prevMove.to.y) //in the right place to take it, the square I'd move to is empty because the previous move was a move up 2 over that square
		{
			addMove(move(game.prevMove.to.x, game.prevMove.to.y+add), game);
			possMoves.back().taken=game.data[game.prevMove.to.y][game.prevMove.to.x];
		}
	}
}

bool rook::checkSquare(board &game, unsigned char x, unsigned char y)
{
	if(!game.data[y][x])
	{
		addMove(move(x,y), game);
		return true;
	}
	else if(game.data[y][x]->color!=color)
	{
		addMove(move(x,y), game);
		possMoves.back().taken=game.data[y][x];
	}
	else //defending
	{
		game.squareData[y][x]->attackedBy[color].push_back(this); //defending my piece
	}
	return false; // a piece is in the way
}
void rook::getPossMoves(board &game)
{
	possMoves.clear();
	for(int i=1; i<=p.x; i++) //left
	{
		if(!checkSquare(game,p.x-i,p.y))
			break;
	}
	for(int i=1; i<=7-p.x; i++) //right
	{
		if(!checkSquare(game,p.x+i,p.y))
			break;
	}
	for(int i=1; i<=p.y; i++) //up
	{
		if(!checkSquare(game,p.x,p.y-i))
			break;
	}
	for(int i=1; i<=7-p.y; i++) //down
	{
		if(!checkSquare(game,p.x,p.y+i))
			break;
	}
}

void knight::checkSquare(board &game, unsigned char x, unsigned char y)
{
	if(0<=y && y<=7 && 0<=x && x<=7)
	{
		if(!game.data[y][x])
		{
			addMove(move(x,y), game);
		}
		else if(game.data[y][x]->color!=color)
		{
			addMove(move(x,y), game);
			possMoves.back().taken=game.data[y][x];
		}
		else //defending
		{
			game.squareData[y][x]->attackedBy[color].push_back(this); //defending my piece
		}
	}
}
void knight::getPossMoves(board &game)
{
	possMoves.clear();
	checkSquare(game,p.x+1,p.y+2);
	checkSquare(game,p.x+1,p.y-2);
	checkSquare(game,p.x+2,p.y+1);
	checkSquare(game,p.x+2,p.y-1);
	checkSquare(game,p.x-1,p.y+2);
	checkSquare(game,p.x-1,p.y-2);
	checkSquare(game,p.x-2,p.y+1);
	checkSquare(game,p.x-2,p.y-1);
}

bool bishop::checkSquare(board &game, unsigned char x, unsigned char y)
{
	if(0<=y && y<=7 && 0<=x && x<=7)
	{
		if(!game.data[y][x])
		{
			addMove(move(x,y), game);
			return true;
		}
		else if(game.data[y][x]->color!=color)
		{
			addMove(move(x,y), game);
			possMoves.back().taken=game.data[y][x];
		}
		else
		{
			game.squareData[y][x]->attackedBy[color].push_back(this); //defending my piece
		}
		return false; // a piece is in the way
	}
	return false; //off the board
}
void bishop::getPossMoves(board &game)
{
	possMoves.clear();
	for(int i=1; ; i++)
	{
		if(!checkSquare(game,p.x+i,p.y+i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkSquare(game,p.x-i,p.y+i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkSquare(game,p.x+i,p.y-i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkSquare(game,p.x-i,p.y-i))
			break;
	}
}

bool queen::checkRook(board &game, unsigned char x, unsigned char y)
{
	if(!game.data[y][x])
	{
		addMove(move(x,y), game);
		return true;
	}
	else if(game.data[y][x]->color!=color)
	{
		addMove(move(x,y), game);
		possMoves.back().taken=game.data[y][x];
	}
	else
		game.squareData[y][x]->attackedBy[color].push_back(this); //defending my piece
	return false; // a piece is in the way
}
bool queen::checkBishop(board &game, unsigned char x, unsigned char y)
{
	if(0<=y && y<=7 && 0<=x && x<=7)
	{
		if(!game.data[y][x])
		{
			addMove(move(x,y), game);
			return true;
		}
		else if(game.data[y][x]->color!=color)
		{
			addMove(move(x,y), game);
			possMoves.back().taken=game.data[y][x];
		}
		else
			game.squareData[y][x]->attackedBy[color].push_back(this); //defending my piece
		return false; // a piece is in the way
	}
	return false; //off the board
}
void queen::getPossMoves(board &game)
{
	possMoves.clear();
	for(int i=1; i<=p.x; i++) //left
	{
		if(!checkRook(game,p.x-i,p.y))
			break;
	}
	for(int i=1; i<=7-p.x; i++) //right
	{
		if(!checkRook(game,p.x+i,p.y))
			break;
	}
	for(int i=1; i<=p.y; i++) //up
	{
		if(!checkRook(game,p.x,p.y-i))
			break;
	}
	for(int i=1; i<=7-p.y; i++) //down
	{
		if(!checkRook(game,p.x,p.y+i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkBishop(game,p.x+i,p.y+i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkBishop(game,p.x-i,p.y+i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkBishop(game,p.x+i,p.y-i))
			break;
	}
	for(int i=1; ; i++)
	{
		if(!checkBishop(game,p.x-i,p.y-i))
			break;
	}
}

void king::checkSquare(board &game, unsigned char x, unsigned char y)
{
	if(0<=y && y<=7 && 0<=x && x<=7)
	{
		if(!game.data[y][x])
		{
			addMove(move(x,y), game);
		}
		else if(game.data[y][x]->color!=color)
		{
			addMove(move(x,y), game);
			possMoves.back().taken=game.data[y][x];
		}
		else
			game.squareData[y][x]->attackedBy[color].push_back(this); //defending my piece
	}
}
void king::getPossMoves(board &game)
{
	getPossMoves(game, false);
}
void king::getPossMoves(board &game, bool castlingCheck)
{
	if(!castlingCheck) //when castlingCheck, only check castling
	{
		possMoves.clear();
		checkSquare(game,p.x+1,p.y+1);	
		checkSquare(game,p.x+1,p.y-1);
		checkSquare(game,p.x-1,p.y+1);
		checkSquare(game,p.x-1,p.y-1);
		checkSquare(game,p.x+1,p.y);
		checkSquare(game,p.x-1,p.y);
		checkSquare(game,p.x,p.y+1);
		checkSquare(game,p.x,p.y-1);
	}

	//castling
	if(castlingCheck && !moved && game.check!=color) //checking for castling, the king hasn't moved, isn't in check
	{
		//right
		if(!game.data[p.y][5] && !game.data[p.y][6]) //empty adjacent squares
		{
			if(game.data[p.y][7] && game.data[p.y][7]->value==5 && !((rook*)(game.data[p.y][7]))->moved) //the rook on the h column hasn't moved
			{
				if(game.squareData[p.y][5]->attackedBy[(color==0)?1:0].size()==0 && game.squareData[p.y][6]->attackedBy[(color==0)?1:0].size()==0) //squares I'm moving through/ending on not attacked
				{
					bool good=true;
					for(int i=4; i<=7; i++) //a pawn would cause me to move through check
					{
						if(game.data[(color==1)?6:1][i] && game.data[(color==1)?6:1][i]->color!=color && game.data[(color==1)?6:1][i]->value==1)
						{	
							good=false;
							break;
						}
					}
					if(good) //add the move to possible moves
					{
						addMove(move(6,p.y), game);
						possMoves.back().castling=true;
					}
				}
			}
		}
		//left
		if(!game.data[p.y][1] && !game.data[p.y][2] && !game.data[p.y][3]) //empty adjacent squares
		{
			if(game.data[p.y][0] && game.data[p.y][0]->value==5 && !((rook*)(game.data[p.y][0]))->moved) //the rook on the a column hasn't moved
			{
				if(game.squareData[p.y][2]->attackedBy[(color==0)?1:0].size()==0 && game.squareData[p.y][3]->attackedBy[(color==0)?1:0].size()==0) //squares I'm moving through/ending on not attacked
				{
					bool good=true;
					for(int i=1; i<=4; i++) //a pawn would cause me to move through check
					{
						if(game.data[(color==1)?6:1][i] && game.data[(color==1)?6:1][i]->value==1)
						{	
							good=false;
							break;
						}
					}
					if(good) //add the move to possible moves
					{
						addMove(move(2,p.y), game);
						possMoves.back().castling=true;
					}
				}
			}
		}
	}
}

piece *piece::copyPiece(piece *source)
{
	piece *possMoves;
	if(!source)
		possMoves=NULL;
	else if(source->value==1)
		possMoves = new pawn(*((pawn*)source));
	else if(source->value==3 && source->isKnight)
		possMoves = new knight(*((knight*)source));
	else if(source->value==3 && !source->isKnight)
		possMoves = new bishop(*((bishop*)source));
	else if(source->value==5)
		possMoves = new rook(*((rook*)source));
	else if(source->value==9)
		possMoves = new queen(*((queen*)source));
	else if(source->value==255)
		possMoves = new king(*((king*)source));
	return possMoves;
}
void piece::addMove(move m, board &game)
{
	possMoves.push_back(m);
	if(value!=1 || m.to.x!=p.x) //not a pawn moving straight up, since that's not an attack
		game.squareData[m.to.y][m.to.x]->attackedBy[color].push_back(this); //no need for an exception for castling, since the square must be empty before castling is a possible move
}
void piece::removeMove(int index, board &game)
{
	for(int i=(int)game.squareData[possMoves[index].to.y][possMoves[index].to.x]->attackedBy[color].size()-1; i>=0; i--) //yes it's absolutely necessary for that to be an int not an unsigned int.
	{
		if(game.squareData[possMoves[index].to.y][possMoves[index].to.x]->attackedBy[color][i]==this)
		{
			game.squareData[possMoves[index].to.y][possMoves[index].to.x]->attackedBy[color].erase(game.squareData[possMoves[index].to.y][possMoves[index].to.x]->attackedBy[color].begin()+i);
			break;
		}
	}
	if(possMoves[index].newtype)
		delete possMoves[index].newtype;
	possMoves.erase(possMoves.begin()+index);
}
