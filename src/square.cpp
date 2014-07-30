#include <cstdlib>
#include "../hdr/square.h"

square::square()
{
}

square* square::copySquare(square *source)
{
	if(!source)
		return NULL;
	square *ret = new square();
	for(int i=0; i<2; i++)
	{
		for(unsigned int j=0; j<source->attackedBy[i].size(); j++)
		{
			ret->attackedBy[i].push_back(source->attackedBy[i][j]);
		}
		for(unsigned int j=0; j<source->pinAttackBy[i].size(); j++)
		{
			ret->attackedBy[i].push_back(source->pinAttackBy[i][j]);
		}
	}
	return ret;
}
