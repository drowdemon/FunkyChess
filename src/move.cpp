#include <cstdlib>
#include "../hdr/move.h"
#include "../hdr/piece.h"

move::move(unsigned char px, unsigned char py) : to(px,py)
{
	taken=NULL;
	newtype=NULL;
	castling=false;
}

move::move() : to()
{
	taken=NULL;
	newtype=NULL;
	castling=false;
}

/*move::~move()
{
	delete newtype;
}*/

/*move& move::operator=(const move &source)
{
	to.x=source.to.x;
	to.y=source.to.y;
	taken = piece::copyPiece(source.taken);
	newtype = piece::copyPiece(source.newtype);
	castling = source.castling;
	return *this;
}*/
void move::copy(const move &source)
{
	to.x=source.to.x;
	to.y=source.to.y;
	taken = source.taken;
	newtype = piece::copyPiece(source.newtype);
	castling = source.castling;
}
