#ifndef POSITION_H_
#define POSITION_H_

class position
{
public:
	unsigned char x;
	unsigned char y;//these two could be one unsigned char, but screw efficiency to that extent
	position(unsigned char px=9, unsigned char py=9);
};

#endif /* POSITION_H_ */
