#include <stdio.h>
#include <stdlib.h>

#include "libsprite.h"
 
int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: palette2c palettefile\n");
		exit(EXIT_FAILURE);
	}

	char* filename = argv[1];

	FILE* palettefile = fopen(filename, "rb");
	if (palettefile == NULL) {
		printf("Can't load palettefile %s\n", filename);
		exit(EXIT_FAILURE);
	}

	sprite_init();
	
	struct sprite_palette* palette = sprite_load_palette(palettefile);

	for (uint32_t i = 0; i < palette->size; i++) {
		printf("0x%x, ", palette->palette[i]);
	}

	fclose(palettefile);

	return EXIT_SUCCESS;
}