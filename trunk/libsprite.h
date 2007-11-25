#ifndef libsprite_h_
#define libsprite_h_

#include <stdint.h>
#include <stdbool.h>

struct sprite_area {
	uint32_t extension_size; /* size of extension_words in bytes */
	uint8_t* extension_words;
	uint32_t sprite_count;
	struct sprite** sprites; /* array of length sprite_count */
};

struct sprite_mode {
	uint32_t colorbpp;
	uint32_t maskbpp;
	uint32_t xdpi;
	uint32_t ydpi;
};

struct sprite {
	unsigned char name[13]; /* last byte for 0 terminator */
	struct sprite_mode* mode;
	bool hasmask;
	bool has_palette;
	uint32_t palettesize; /* in number of entries (each entry is a word) */
	uint32_t* palette;
	uint32_t width; /* width and height in _pixels_ */
	uint32_t height;
	uint32_t* image;
};

void sprite_init();
struct sprite_area* sprite_load_file(FILE* f);

#endif
