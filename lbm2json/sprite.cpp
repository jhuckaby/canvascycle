/**
 * Library for loading DeluxePaint II LBM/BBM files.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "geometry.h"
#include "palette.h"
#include "bitmap.h"
#include "sprite.h"

color_register_32 black = setRGB(0, 0, 0);
color_register_32 white = setRGB(255, 255, 255);

short check_id(char *id, char *check) {
	if ((id[0] == check[0])
		&& (id[1] == check[1])
		&& (id[2] == check[2])
		&& (id[3] == check[3])) {
			return(1);
	}
	return(0);
}

void endian_swap(long *value) {
	unsigned char *chs;
	unsigned char temp;
	
	chs = (unsigned char *)value;
	
	temp = chs[0]; chs[0] = chs[3]; chs[3] = temp;
	temp = chs[1]; chs[1] = chs[2]; chs[2] = temp;
}

void endian_swap(unsigned long *value) {
	endian_swap((long *)value);
}

void endian_swap(short *value) {
	unsigned char *chs;
	unsigned char temp;
	
	chs = (unsigned char *)value;
	
	temp = chs[0]; chs[0] = chs[1]; chs[1] = temp;
}

void endian_swap(unsigned short *value) {
	endian_swap((short *)value);
}

Sprite::Sprite () {
	this->init();
}

Sprite::Sprite (char *newFilename) {
	printf("Creating new bitmap from: %s\n", newFilename);
	this->init();
	this->load_bitmap(newFilename);
}

Sprite::~Sprite () {
	if (this->bitmap) delete this->bitmap;
	if (this->alphaSprite) delete this->alphaSprite;
	
	if (this->sourceRect) delete this->sourceRect;
}

void Sprite::init () {
	this->x = 0;
	this->y = 0;
	this->visible = 1;
	this->error = 0;
	this->prepareBlits = 1;
	this->blendShift = 1;
	this->cycleSpeed = 1.0;
	this->isAlpha = 0;
	this->ditherAlpha = 0;
	this->priority = 1;
	this->lastFrameCount = 0;
	this->lastFrameTicks = 0;
	this->sourceRect = NULL;
	
	this->bitmap = NULL;
	this->alphaSprite = NULL;
	this->nextSprite = NULL;
	
	this->timeline = NULL;
	
	short i;
	for (i=0; i<256; i++) this->stencil[i] = 0;
}

void Sprite::load_bitmap(char *newFilename) {
	if (this->bitmap) {
		delete this->bitmap;
		this->bitmap = NULL;
	}
	
	this->bitmap = this->load_from_lbm(newFilename);
	if (this->bitmap) {
		this->sourceRect = new Rect(this->bitmap->bounds);
		
	} // bitmap loaded successfully
}

void Sprite::load_alpha(char *newFilename) {
	if (this->alphaSprite) {
		delete this->alphaSprite;
		this->alphaSprite = NULL;
	}
	
	this->alphaSprite = new Sprite();
	this->alphaSprite->isAlpha = 1;
	this->alphaSprite->load_bitmap(newFilename);
	
}

Bitmap *Sprite::load_from_lbm (char *newFilename) {
	FILE* in;
	chunk mychunk;
	signed char sdata;
	int i, j;
	unsigned char *temp;
	unsigned long len, maxLen, bodyBytes;
	unsigned char udata;
	color_register *color_map = NULL;
	bitmap_header bmhd;
	Bitmap *newBitmap = NULL;
	// struct stat mystat;
	
	// stat(newFilename, &mystat);
	
	in = (FILE*)fopen(newFilename, "rb");
	if (!in) {
		printf("Could not fopen file: %s\n", newFilename);
		this->error = 1;
		return NULL;
	}
	
	fread(&mychunk, sizeof(chunk), 1, in);
	fseek(in, 4, SEEK_CUR); // skip over 'PBM '
	
	if (!check_id(mychunk.id, "FORM")) {
		printf("FORM chunk not found.\n");
		this->error = 1;
		fclose(in);
		return NULL;
	}
	
	endian_swap(&mychunk.length);
	// printf("rest of file: %d\n",mychunk.length);
	
	while (!feof(in)) {
		mychunk.id[0] = ' ';
		mychunk.id[1] = ' ';
		mychunk.id[2] = ' ';
		mychunk.id[3] = ' ';
		fread(&mychunk, sizeof(chunk), 1, in);
		endian_swap(&mychunk.length);
		
		printf("\nChunk: %c%c%c%c (%d)\n", mychunk.id[0], mychunk.id[1], mychunk.id[2], mychunk.id[3], mychunk.length);
		
		if (check_id(mychunk.id, "BMHD")) {
			fread(&bmhd, sizeof(bitmap_header), 1, in);
			
			endian_swap(&bmhd.width);
			endian_swap(&bmhd.height);
			
			bmhd.width += (2-(bmhd.width%2))%2; // even widths only (round up)
			
			printf("\n");
			printf("Bitmap Header found.\n");
			printf("Width: %d\n", bmhd.width);
			printf("Height: %d\n", bmhd.height);
			printf("Compression: %d\n", bmhd.compression);
			printf("Planes: %d\n", bmhd.num_planes);
			printf("Masking: %d\n", bmhd.masking);
			
			if (bmhd.masking == 1) {
				printf("ERROR: bmhd.masking == 1 -- MASK INTERLEAVED WITH BODY PIXELS -- I cannot figure this out.\n");
				this->error = 2;
				fclose(in);
				return NULL;
			}
			
			newBitmap = new Bitmap(bmhd.width, bmhd.height);
			strcpy(newBitmap->filename, newFilename);
			
			if (bmhd.masking == 2) {
				newBitmap->blackMask = 1;
			}
			
		} // BMHD
		
		else if (check_id(mychunk.id, "CMAP")) {
			if (!color_map) color_map = (color_register *)malloc(mychunk.length);
			fread(color_map, mychunk.length, 1, in);
			
			newBitmap->palette->numColors = mychunk.length/sizeof(color_register);
			
			for (i=0; i<newBitmap->palette->numColors; i++) {
				newBitmap->palette->baseColors[i].red = color_map[i].red;
				newBitmap->palette->baseColors[i].green = color_map[i].green;
				newBitmap->palette->baseColors[i].blue = color_map[i].blue;
				newBitmap->palette->baseColors[i].padding = 0;
				
				if (color_map[i].red == 255 && color_map[i].green == 0 && color_map[i].blue == 255)
					newBitmap->pinkMask = 1;
			}
			
			printf("\n");
			printf("Palette found.  %d color entries\n", newBitmap->palette->numColors);
			printf("Color 000: %d, %d, %d\n", color_map[0].red, color_map[0].green, color_map[0].blue);
			printf("Color 001: %d, %d, %d\n", color_map[1].red, color_map[1].green, color_map[1].blue);
		}
		
		else if (check_id(mychunk.id, "CRNG")) {
			if (!newBitmap->palette->numCycles) {
				newBitmap->palette->setupCycles();
				printf("\n");
				printf("Color cycling info found.\n");
			}
			
			fread(&newBitmap->palette->cycles[newBitmap->palette->numCycles], mychunk.length, 1, in);
			
			endian_swap(&newBitmap->palette->cycles[newBitmap->palette->numCycles].rate);
			endian_swap(&newBitmap->palette->cycles[newBitmap->palette->numCycles].reverse);
			
			printf("Cycle %d:  Reverse: %d  Rate: %d  Range: %d-%d\n", newBitmap->palette->numCycles, newBitmap->palette->cycles[newBitmap->palette->numCycles].reverse, newBitmap->palette->cycles[newBitmap->palette->numCycles].rate, newBitmap->palette->cycles[newBitmap->palette->numCycles].low, newBitmap->palette->cycles[newBitmap->palette->numCycles].high);
			
			newBitmap->palette->numCycles++;
		} // CRNG
		
		else if (check_id(mychunk.id, "BODY")) {
			printf("\n");
			printf("Found body.  Length: %d\n", mychunk.length);
			
			maxLen = (unsigned long)((unsigned long)newBitmap->width * (unsigned long)newBitmap->height);
			bodyBytes = 0;
			
			// int maskReadMode = 1;
			// int maskReadCounter = newBitmap->width;
						
			if (bmhd.compression) {
				temp = newBitmap->baseAddr;
				len = mychunk.length;
				sdata=fgetc(in);
				while( len>0 && !feof(in)) {
					len--;
					/* ByteRun1 decompression */
					
					/* [0..127]   : followed by n+1 bytes of data. */
					if (sdata>=0) {
						i=sdata+1;
						for(j=0; j<i; j++) {
							udata=fgetc(in);
							len--;
							if (!feof(in)) {
								/* if (bmhd.masking == 1) {
									if (maskReadMode == 1) {
										*temp = udata;
										if (temp - newBitmap->baseAddr < maxLen - 1) temp++;
										bodyBytes++;
										maskReadCounter--;
										if (!maskReadCounter) {
											maskReadMode = 0;
											maskReadCounter = newBitmap->width / 8;
										}
									}
									else {
										maskReadCounter--;
										if (!maskReadCounter) {
											maskReadMode = 1;
											maskReadCounter = newBitmap->width;
										}
									}
								} // masking
								else { */
									*temp = udata;
									if (temp - newBitmap->baseAddr < maxLen - 1) temp++;
									bodyBytes++;
								// } // no mask
							}
						}
					}
					/* [-1..-127] : followed by byte to be repeated (-n)+1 times*/
					else if (sdata<=-1 && sdata>=-127) {
						i=(-sdata)+1;
						udata=fgetc(in);
						len--;
						for(j=0; j<i; j++) {
							/* if (bmhd.masking == 1) {
								if (maskReadMode == 1) {
									*temp = udata;
									if (temp - newBitmap->baseAddr < maxLen - 1) temp++;
									bodyBytes++;
									maskReadCounter--;
									if (!maskReadCounter) {
										maskReadMode = 0;
										maskReadCounter = newBitmap->width / 8;
									}
								}
								else {
									maskReadCounter--;
									if (!maskReadCounter) {
										maskReadMode = 1;
										maskReadCounter = newBitmap->width;
									}
								}
							} // masking
							else { */
								*temp = udata;
								if (temp - newBitmap->baseAddr < maxLen - 1) temp++;
								bodyBytes++;
							// } // no mask
						}
					}
					/* -128	   : NOOP. */

					sdata=fgetc(in);
				}
			} else {
				fread(newBitmap->baseAddr, maxLen, 1, in);
				bodyBytes += maxLen;
			} // no compression
			
			printf("Uncompressed length: %d\n", bodyBytes);
			printf("Should be: %d x %d = %d\n", newBitmap->width, newBitmap->height, maxLen);
			// printf("Pixel 0x0: %d\n", newBitmap->baseAddr[0]);
			// printf("Pixel 0x1: %d\n", newBitmap->baseAddr[640]);
		} // BODY
		
		else if (check_id(mychunk.id, "GRAB")) {
			printf("Found GRAB -- setting blackMask.\n");
			if (mychunk.length>0) fseek(in, mychunk.length, SEEK_CUR);
			// newBitmap->blackMask = 1;
		}
		
		else {
			// printf("\nUnknown chunk: %c%c%c%c, length: %d\n", mychunk.id[0], mychunk.id[1], mychunk.id[2], mychunk.id[3], mychunk.length);
			if (mychunk.length>0) fseek(in, mychunk.length, SEEK_CUR);
		} // unknown chunk
		
		if (mychunk.length % 2 != 0)
			fseek(in, 2 - (mychunk.length%2), SEEK_CUR);
	} // while !eof
	
	fclose(in);
	
	if (color_map) free(color_map);
	
	if (!newBitmap) this->error = 1;
	// else newBitmap->modDate = (unsigned long)mystat.st_mtime;
	
	return newBitmap;
}

