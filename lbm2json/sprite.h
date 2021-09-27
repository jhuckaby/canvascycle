/**
 * Library for loading DeluxePaint II LBM/BBM files.
 * Copyright 2001-2002 Joseph Huckaby.  All rights reserved.
 **/

typedef struct {
	char	id[4];
	unsigned long	length;
} chunk;

#define msk_none	0
#define msk_has_mask	1
#define msk_has_transparent_color	2
#define msk_lasso	3

	/* Choice of compression algorithm applied to the rows of all 
	 * source and mask planes. "cmpByteRun1" is the byte run encoding 
	 * described in Appendix C. Do not compress across rows! */
#define cmp_none	0
#define cmp_byte_run	1

#define MAX_CYCLES 256

// BMHD
typedef struct {
	unsigned short width, height;	/* raster width & height in pixels	*/
	short  x, y;	/* pixel position for this image	*/
	unsigned char num_planes;	/* # source bitplanes	*/
	unsigned char masking;
	unsigned char compression;
	unsigned char pad1;	/* unused; for consistency, put 0 here	*/
	unsigned short transparent_color;	/* transparent "color number" (sort of)	*/
	unsigned char x_aspect, y_aspect;	/* pixel aspect, a ratio width : height	*/
	short  page_width, page_height;	/* source "page" size in pixels	*/
} bitmap_header;

// CMAP
typedef struct {
	unsigned char red, green, blue;	/* color intensities 0..255 */
} color_register;	/* size = 3 bytes */

class Sprite {	
public:
	Bitmap *bitmap;
	short x, y;
	char visible, error, prepareBlits, doMask, blendShift, isAlpha, ditherAlpha, priority;
	unsigned long lastFrameCount, lastFrameTicks;
	float cycleSpeed;
	Rect *sourceRect;
	Sprite *nextSprite;
	Sprite *timeline;
	Sprite *alphaSprite;
	char stencil[256];
	
	Sprite();
	Sprite(char *newFilename);
	~Sprite();
	
	void init();
	void cycle(unsigned long timeNow);
	void prepPaletteForAlpha(color_register_32 *colors);
	void load_bitmap(char *newFilename);
	void load_alpha(char *newFilename);
	Bitmap *load_from_lbm(char *newFilename);
};

extern color_register_32 black;
extern color_register_32 white;

extern short check_id(char *id, char *check);
extern void endian_swap(long *value);
extern void endian_swap(unsigned long *value);
extern void endian_swap(short *value);
extern void endian_swap(unsigned short *value);