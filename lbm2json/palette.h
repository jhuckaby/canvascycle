/**
 * Library for handling 8-bit palettes.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_COLORS		256
#define MAX_CYCLES		256

typedef struct {
	unsigned char blue, green, red, padding;	/* color intensities 0..255 */
} color_register_32;	/* size = 4 bytes */

typedef struct {
	short  pad1;	/* reserved for future use; store 0 here	*/
	short  rate;	/* color cycle rate	*/
	short  reverse;
	unsigned char low, high; /* lower and upper color registers selected	*/
} c_range;

class Palette {
public:
	color_register_32 *colors, *baseColors;
	c_range *cycles;
	short numColors, numCycles;
	
	Palette();
	~Palette();
	
	void setupCycles();
	void cycle(color_register_32 *sourceColors, unsigned long timeNow, float speedAdjust, char blendShift);
	void fade(color_register_32 destColor, short frame, short max);
	void fade(Palette *destPalette, short frame, short max);
	void invert();
};

extern char USE_BLEND_SHIFT;
extern short CYCLE_SPEED;
extern char ENABLE_CYCLING;

extern color_register_32 setRGB(unsigned char red, unsigned char green, unsigned char blue);
extern char colorEqual(color_register_32 a, color_register_32 b);
extern void copyColors(register color_register_32 *source_colors, register color_register_32 *dest_colors);
extern color_register_32 fadeColor(color_register_32 sourceColor, color_register_32 destColor, short frame, short max);
extern color_register_32 fadeColor(color_register_32 sourceColor, color_register_32 destColor, long frame, long max);
extern unsigned char findColorIndex(color_register_32 *colors, color_register_32 color);
extern void makeGradient(color_register_32 *colors, short low, short high, color_register_32 lowColor, color_register_32 highColor);
extern color_register_32 RGBtoHSV( color_register_32 rgbColor );
extern color_register_32 HSVtoRGB( color_register_32 hsvColor );

