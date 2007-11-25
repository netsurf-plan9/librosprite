#include <stdio.h>
#include <stdlib.h>

#include "libsprite.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: example spritefile\n");
		exit(EXIT_FAILURE);
	}

	char* filename = argv[1];

	FILE* spritefile = fopen(filename, "rb");
	if (spritefile == NULL) {
		printf("Can't load spritefile %s\n", filename);
		exit(EXIT_FAILURE);
	}

	printf("Loading %s\n", filename);
	sprite_init();

	struct sprite_area* sprite_area = sprite_load_file(spritefile);
	printf("sprite_count %u\n", sprite_area->sprite_count);
	printf("extension_size %u\n", sprite_area->extension_size);

	for (uint32_t i = 0; i < sprite_area->sprite_count; i++) {
		struct sprite* sprite = sprite_area->sprites[i];
		printf("\nname %s\n", sprite->name);
		printf("colorbpp %u\n", sprite->mode->colorbpp);
		printf("xdpi %u\n", sprite->mode->xdpi);
		printf("ydpi %u\n", sprite->mode->ydpi);
		printf("width %u px\n", sprite->width);
		printf("height %u px\n", sprite->height);
	
		printf("hasPalette %s\n", sprite->has_palette ? "YES" : "NO");
		if (sprite->has_palette) printf("paletteSize %u\n", sprite->palettesize);

		printf("hasMask %s\n", sprite->hasmask ? "YES" : "NO");
		if (sprite->hasmask) printf("maskbpp %u\n", sprite->mode->maskbpp);
	}

	fclose(spritefile);

	return EXIT_SUCCESS;
}
