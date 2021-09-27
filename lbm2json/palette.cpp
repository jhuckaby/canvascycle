/**
 * Library for handling 8-bit palettes.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "palette.h"

#define PRECISION 100
#define DFLOAT_MOD(a,b) (double)((long)(a*PRECISION) % (b*PRECISION))/PRECISION

char USE_BLEND_SHIFT = 1;
short CYCLE_SPEED = 280;
char ENABLE_CYCLING = 1;

void copyColors(register color_register_32 *source_colors, register color_register_32 *dest_colors) {
	register unsigned short i;
	
	for (i=0; i<16; i++) {
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
		*(dest_colors++) = *(source_colors++); *(dest_colors++) = *(source_colors++);
	}
}

color_register_32 fadeColor(color_register_32 sourceColor, color_register_32 destColor, long frame, long max) {
	color_register_32 tempColor;
	
	if (!max) return sourceColor; // avoid divide by zero
	if (frame < 0) frame = 0;
	if (frame > max) frame = max;
	
	tempColor.red = sourceColor.red + (((long)(destColor.red - sourceColor.red) * frame) / max);
	tempColor.green = sourceColor.green + (((long)(destColor.green - sourceColor.green) * frame) / max);
	tempColor.blue = sourceColor.blue + (((long)(destColor.blue - sourceColor.blue) * frame) / max);
	
	return(tempColor);
}

color_register_32 fadeColor(color_register_32 sourceColor, color_register_32 destColor, short frame, short max) {
	return fadeColor(sourceColor, destColor, (long)frame, (long)max);
}

void shiftColors(color_register_32 *colors, c_range *range, short amount) {
	short i,j;
	color_register_32 temp;
	
	for (i = 0; i < amount; i++) {
		temp = colors[range->high];
		for (j=range->high-1; j>=range->low; j--)
			colors[j+1] = colors[j];
		colors[range->low] = temp;
	} // i loop
}

void blendShiftColors(color_register_32 *colors, c_range *range, double amount) {
	short j;
	color_register_32 temp;
	
	shiftColors(colors, range, (short)amount);
	
	short frame = (short)( (double)((double)amount - (short)amount) * PRECISION );
	
	temp = colors[range->high];
	for (j=range->high-1; j>=range->low; j--)
		colors[j+1] = fadeColor(colors[j+1], colors[j], frame, PRECISION);
	colors[range->low] = fadeColor(colors[range->low], temp, frame, PRECISION);
}

color_register_32 setRGB(unsigned char red, unsigned char green, unsigned char blue) {
	color_register_32 temp;
	
	temp.red = red;
	temp.green = green;
	temp.blue = blue;
	temp.padding = 0;
	
	return(temp);
}

char colorEqual(color_register_32 a, color_register_32 b) {
	return (a.red==b.red && a.green==b.green && a.blue==b.blue);
}

char hasPink(color_register_32 *colors, c_range *range) {
	short i;
	
	for (i=range->low; i<=range->high; i++)
		if (colors[i].red==255 && colors[i].green==0 && colors[i].blue==255) return 1;
	
	return 0;
}

void swapColors(color_register_32 *a, color_register_32 *b) {
	color_register_32 temp;
	
	temp = *a; *a = *b; *b = temp;
}

void reverseColors(color_register_32 *colors, c_range *range) {
	short i;
	short cycleSize = (range->high - range->low) + 1;
	
	for (i=0; i<cycleSize/2; i++)
		swapColors(&colors[range->low+i], &colors[range->high-i]);
}

unsigned char findColorIndex(color_register_32 *colors, color_register_32 color) {
	short i;
	
	for (i=0; i<MAX_COLORS; i++)
		if (colorEqual(colors[i], color)) return i;
	
	return 0;
}

void makeGradient(color_register_32 *colors, short low, short high, color_register_32 lowColor, color_register_32 highColor) {
	short index;
	
	for (index=low; index<=high; index++)
		colors[index] = fadeColor(lowColor, highColor, (long)(index-low), (long)(high-low));
}

Palette::Palette() {
	this->colors = (color_register_32 *)malloc(sizeof(color_register_32) * MAX_COLORS);
	this->baseColors = (color_register_32 *)malloc(sizeof(color_register_32) * MAX_COLORS);
	
	this->numColors = MAX_COLORS;
	this->numCycles = 0;
	
	this->cycles = NULL;
}

Palette::~Palette() {
	if (this->colors) free(this->colors);
	if (this->baseColors) free(this->baseColors);
	if (this->cycles) free(this->cycles);
}

void Palette::setupCycles() {
	if (!this->cycles) this->cycles = (c_range *)malloc(sizeof(c_range) * MAX_CYCLES);
}

void Palette::cycle(color_register_32 *sourceColors, unsigned long timeNow, float speedAdjust, char blendShift) {
	short i;
	short cycleSize, cycleRate;
	double cycleAmount;
	
	copyColors(sourceColors, this->colors);
	
	if (ENABLE_CYCLING) {
		for (i=0; i<this->numCycles; i++) {
			if (this->cycles[i].rate) {
				cycleSize = (this->cycles[i].high - this->cycles[i].low) + 1;
				cycleRate = this->cycles[i].rate / (short)((float)CYCLE_SPEED * speedAdjust);
				
				if (this->cycles[i].reverse < 3)
					cycleAmount = DFLOAT_MOD((double)(timeNow / (1000 / (double)cycleRate)), cycleSize);
				else if (this->cycles[i].reverse == 3) {
					cycleAmount = DFLOAT_MOD((double)(timeNow / (1000 / (double)cycleRate)), cycleSize * 2);
					if (cycleAmount >= cycleSize) cycleAmount = (cycleSize*2) - cycleAmount;
				} else if (this->cycles[i].reverse < 6) {
					cycleAmount = DFLOAT_MOD((double)(timeNow / (1000 / (double)cycleRate)), cycleSize);
					cycleAmount = sin((cycleAmount * 3.1415926 * 2)/cycleSize) + 1;
					if (this->cycles[i].reverse == 4) cycleAmount *= (cycleSize / 4);
					else if (this->cycles[i].reverse == 5) cycleAmount *= (cycleSize / 2);
				}
				
				if (this->cycles[i].reverse == 2) reverseColors(this->colors, &this->cycles[i]);
				
				if (USE_BLEND_SHIFT && blendShift && !hasPink(this->colors, &this->cycles[i])) blendShiftColors(this->colors, &this->cycles[i], cycleAmount);
				else shiftColors(this->colors, &this->cycles[i], (short)cycleAmount);
				
				if (this->cycles[i].reverse == 2) reverseColors(this->colors, &this->cycles[i]);
			} // active cycle
		} // i loop
	}
}

void Palette::fade(color_register_32 destColor, short frame, short max) {
	unsigned short index;
	
	for (index=0; index<256; index++)
		this->colors[index] = fadeColor(this->colors[index], destColor, frame, max);
}

void Palette::fade(Palette *destPalette, short frame, short max) {
	unsigned short index;
	
	for (index=0; index<this->numColors; index++)
		this->colors[index] = fadeColor(this->colors[index], destPalette->colors[index], frame, max);
}

void Palette::invert() {
	unsigned short index;
	
	for (index=0; index<this->numColors; index++) {
		this->colors[index].red = 255 - this->colors[index].red;
		this->colors[index].green = 255 - this->colors[index].green;
		this->colors[index].blue = 255 - this->colors[index].blue;
	}
}

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	
	min = r;
	if (g < min) min = g;
	if (b < min) min = b;
	// min = MIN( r, g, b );
	
	max = r;
	if (g > max) max = g;
	if (b > max) max = b;
	// max = MAX( r, g, b );
	
	*v = max; // v
	delta = max - min;
	if( max != 0 ) *s = delta / max; // s
	else { *s = 0; *h = -1; return; } // r = g = b = 0		// s = 0, v is undefined
	if( r == max ) *h = ( g - b ) / delta; // between yellow & magenta
	else if( g == max ) *h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else *h = 4 + ( r - g ) / delta; // between magenta & cyan
	*h *= 60; // degrees
	if( *h < 0 ) *h += 360;
}

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) { *r = *g = *b = v; return; } // achromatic (grey)
	h /= 60;			// sector 0 to 5
	i = int( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0: *r = v; *g = t; *b = p; break;
		case 1: *r = q; *g = v; *b = p; break;
		case 2: *r = p; *g = v; *b = t; break;
		case 3: *r = p; *g = q; *b = v; break;
		case 4: *r = t; *g = p; *b = v; break;
		default: *r = v; *g = p; *b = q; break;
	}
}

color_register_32 RGBtoHSV( color_register_32 rgbColor ) {
	float h2, s2, v2;
	color_register_32 hsvColor;
	RGBtoHSV( (float)((float)rgbColor.red / 255.0), (float)((float)rgbColor.green / 255.0), (float)((float)rgbColor.blue / 255.0), &h2, &s2, &v2 );
	hsvColor.red = (unsigned char)((float)((h2 * 256.0) / 360.0));
	hsvColor.green = (unsigned char)((float)(s2 * 255.0));
	hsvColor.blue = (unsigned char)((float)(v2 * 255.0));
	return hsvColor;
}

color_register_32 HSVtoRGB( color_register_32 hsvColor ) {
	float r2, g2, b2;
	color_register_32 rgbColor;
	HSVtoRGB( &r2, &g2, &b2, (float)(((float)hsvColor.red * 360.0) / 256.0), (float)hsvColor.green / 255.0, (float)hsvColor.blue / 255.0 );
	rgbColor.red = (unsigned char)((float)r2 * 255.0);
	rgbColor.green = (unsigned char)((float)g2 * 255.0);
	rgbColor.blue = (unsigned char)((float)b2 * 255.0);
	return rgbColor;
}

