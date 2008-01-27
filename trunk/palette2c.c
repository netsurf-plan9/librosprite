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

	FILE* f = fopen(filename, "rb");
	if (f == NULL) {
		printf("Can't load palettefile %s\n", filename);
		exit(EXIT_FAILURE);
	}

	struct rosprite_file_context* ctx = rosprite_create_file_context(f);
	
	struct rosprite_palette* palette = rosprite_load_palette(rosprite_file_reader, ctx);

	for (uint32_t i = 0; i < palette->size; i++) {
		printf("0x%x, ", palette->palette[i]);
	}

	fclose(f);

	rosprite_destroy_file_context(ctx);
	rosprite_destroy_palette(palette);

	return EXIT_SUCCESS;
}
