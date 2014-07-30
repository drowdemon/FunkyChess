#include <iostream>
#include "../hdr/piece.h"
#include "../hdr/move.h"
#include "../hdr/board.h"
#include "../hdr/blocker.h"

using namespace std;

void displayBoard(board &game);
void displayPossMoves(board &game);
void displayBlocking(board &game);
void displayPrevInfo(board &game);

int main()
{
	board game;
	displayBoard(game);
	move m = move(6,2);
	game.advance(&m, game.data[7][6]);
	displayBoard(game);
	game.advance(&game.data[2][6]->possMoves[1], game.data[2][6]);
	displayBoard(game);
	displayPossMoves(game);
	cout << (int)game.check << endl;
	return 0;
}

void displayBoard(board &game)
{
	for(int i=0; i<8; i++)
	{
		for(int j=0; j<8; j++)
		{
			cout << (game.data[i][j] ? game.data[i][j]->value==255?2:game.data[i][j]->value : 0)  << " ";
		}
		cout << endl;
	}
	cout << endl << endl;
}
void displayPossMoves(board &game)
{
	for(int i=0; i<2; i++)
	{
		for(unsigned int j=0; j<game.positions[i].size(); j++)
		{
			cout << (int)game.positions[i][j]->p.x << "," << (int)game.positions[i][j]->p.y << ":  ";
			for(unsigned int k=0; k<game.positions[i][j]->possMoves.size(); k++)
			{
				cout << "(" << (int)game.positions[i][j]->possMoves[k].to.x << "," << (int)game.positions[i][j]->possMoves[k].to.y << "); ";
			}
			cout << endl;
		}
	}
}
void displayBlocking(board &game)
{
	for(int i=0; i<2; i++)
	{
		for(unsigned int j=0; j<game.positions[i].size(); j++)
		{
			cout << (int)game.positions[i][j]->p.x << "," << (int)game.positions[i][j]->p.y << ":  " << endl;
			for(int h=0; h<2; h++)
			{
				cout << "My/Opponent moves blocked:   ";
				for(unsigned int k=0; k<game.positions[i][j]->blocking[h].size(); k++)
				{
					cout << (int)game.positions[i][j]->blocking[h][k]->attacker->p.x << "," << (int)game.positions[i][j]->blocking[h][k]->attacker->p.y << ": ";
					for(unsigned int g=0; g<game.positions[i][j]->blocking[h][k]->blocked.size(); g++)
					{
						cout << "(" << (int)game.positions[i][j]->blocking[h][k]->blocked[g].x << "," << (int)game.positions[i][j]->blocking[h][k]->blocked[g].y << "), ";
					}
					if(game.positions[i][j]->blocking[h][k]->defended)
						cout << "Blocked defense/attack: " << (int)game.positions[i][j]->blocking[h][k]->defended->p.x << "," << (int)game.positions[i][j]->blocking[h][k]->defended->p.y;
					cout << ";    ";
				}
				cout << endl;
			}
			cout << endl << endl;
		}
	}
}
void displayPrevInfo(board &game)
{
	cout << "Prev Position" << (int)game.prevPosition.x << " " << (int)game.prevPosition.y << endl;
	cout << "Prev Move" << (int)game.prevMove.to.x << " " << (int)game.prevMove.to.y << endl;
	cout << "Prev Value" <<(int)game.prevValue << endl;
}
