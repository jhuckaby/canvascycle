/**
 * Library for handling 8-bit offscreen bitmaps.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "geometry.h"
#include "palette.h"
#include "bitmap.h"

unsigned char *transTables = NULL, phase = 0, blitMode = 2;
unsigned long *ditherTables = NULL;

void swap(short *a, short *b) { short temp; temp = *a; *a = *b; *b = temp; }

Bitmap::Bitmap() {
	this->init();
}

Bitmap::Bitmap (short newWidth, short newHeight) {
	this->init();
	
	this->width = newWidth;
	this->height = newHeight;
	
	this->bounds = new Rect(this->width, this->height);
	
	this->baseAddr = (unsigned char *)malloc((long)this->width * (long)this->height);
}

Bitmap::~Bitmap() {
	if (this->baseAddr) free(this->baseAddr);
	if (this->palette) delete this->palette;
	if (this->maskPalette) delete this->maskPalette;
	if (this->bounds) delete this->bounds;
}

void Bitmap::init() {
	this->width = 0;
	this->height = 0;
	
	this->palette = new Palette();
	this->maskPalette = new Palette();
	
	this->bounds = NULL;
	this->baseAddr = NULL;
	
	this->transparency = 0;
	this->add = 0;
	this->blackMask = 0;
	this->pinkMask = 0;
	
	strcpy(this->filename, "");
	this->modDate = 0;	
}

unsigned char Bitmap::getPixel(short x, short y) {
	return *(this->baseAddr + (long)((long)this->width * (long)y) + (long)x);
}

void Bitmap::setPixel(short x, short y, unsigned char color) {
	*(this->baseAddr + (long)((long)this->width * (long)y) + (long)x) = color;
}

