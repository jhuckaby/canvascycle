/**
 * Library for handling 8-bit offscreen bitmaps.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

#define TRANS_LEVELS	32
#define TRANS_BITS		((TRANS_LEVELS / 8) + 1)

class Bitmap {
public:
	short width, height;
	Rect *bounds;
	unsigned char *baseAddr;
	Palette *palette, *maskPalette;
	unsigned char transparency, add;
	short blackMask, pinkMask;
	char filename[256];
	unsigned long modDate;
	short doMask;
	
	Bitmap();
	Bitmap(short newWidth, short newHeight);
	~Bitmap();
	
	void init();
	unsigned char getPixel(short x, short y);
	void setPixel(short x, short y, unsigned char color);	
};

extern unsigned char blitMode, phase;
