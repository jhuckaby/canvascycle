#include <iostream>

#include "geometry.h"
#include "palette.h"
#include "bitmap.h"
#include "sprite.h"

// for filename in `ls -1 *.LBM`; do echo $filename; /Users/jhuckaby/Projects/lbm2json/build/Debug/lbm2json $filename; done;


int main (int argc, char * const argv[]) {
	Sprite *sprite = NULL;
	Bitmap *bitmap = NULL;
	Palette *palette = NULL;
	color_register_32 color;
	c_range cycle;
	unsigned char pixel;
	
	/* unsigned char uc;
	printf("Size of unsigned char: %d\n", sizeof(uc));
	
	unsigned char us;
	printf("Size of unsigned short: %d\n", sizeof(us));
	
	unsigned long ul;
	printf("Size of unsigned long: %d\n", sizeof(ul));
	*/
	
    // std::cout << "Hello, World!\n";
	
	printf("Executable: %s\n", argv[0]);
	printf("Argument: %s\n", argv[1]);
	
	char path[80];
	strcpy( path, argv[1] );
	
	sprite = new Sprite( path );
	bitmap = sprite->bitmap;
	palette = bitmap->palette;
	
	char filename[80];
	char* strLastSlash = strrchr( path, '/' );
	if (!strLastSlash) strcpy(filename, path);
	else strcpy(filename, strLastSlash+1);
	
	// write raw binary file first (totally raw pixels, no header)
	char out_file[256];
	sprintf(out_file, "%s.bin", path);
	printf("Output Binary File: %s\n", out_file);
	
	FILE* out;
	out = (FILE*)fopen(out_file, "wb");
	if (!out) {
		printf("Could not fopen file for writing: %s\n", out_file);
		return 0;
	}
	fwrite(bitmap->baseAddr, 1, (unsigned long)bitmap->width * (unsigned long)bitmap->height, out);
	fclose(out);
	
	// now write JSON file
	sprintf(out_file, "%s.json", path);
	printf("Output JSON File: %s\n", out_file);
	
	out = (FILE*)fopen(out_file, "w");
	if (!out) {
		printf("Could not fopen file for writing: %s\n", out_file);
		return 0;
	}
	
	fprintf(out, "{");
	
	// basic info
	fprintf(out, "filename:'%s',width:%d,height:%d", filename, bitmap->width, bitmap->height);
	
	// palette
	fprintf(out, ",colors:[");
	for (int idx = 0; idx < palette->numColors; idx++) {
		color = palette->baseColors[idx];
		if (idx) fprintf(out, ",");
		fprintf(out, "[%d,%d,%d]", color.red, color.green, color.blue);
	}
	fprintf(out, "]");
	
	// cycles
	fprintf(out, ",cycles:[");
	for (int idx = 0; idx < palette->numCycles; idx++) {
		cycle = palette->cycles[idx];
		if (idx) fprintf(out, ",");
		fprintf(out, "{reverse:%d,rate:%d,low:%d,high:%d}", cycle.reverse, cycle.rate, cycle.low, cycle.high);
	}
	fprintf(out, "]");
	
	// pixels
	fprintf(out, ",pixels:[");
	int count = 0;
	for (int y = 0; y < bitmap->height; y++) {
		
		// printf("\nROW %d\n", y);
		fprintf(out, "\n");
		
		for (int x = 0; x < bitmap->width; x++) {
			pixel = bitmap->getPixel( x, y );
			if (count) fprintf(out, ",");
			fprintf(out, "%d", pixel);
			count++;
		} // x loop
	} // y loop
	fprintf(out, "]");
	
	fprintf(out, "}\n");
	fclose(out);
	
	delete sprite;
	
	printf("Done!  Exiting\n");
	
    return 0;
}
