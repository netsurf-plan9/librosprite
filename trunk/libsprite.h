#ifndef ROSPRITE_H
#define ROSPRITE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { rosprite_rgb, rosprite_cmyk } rosprite_color_model;

struct sprite_area {
	uint32_t extension_size; /* size of extension_words in bytes */
	uint8_t* extension_words;
	uint32_t sprite_count;
	struct sprite** sprites; /* array of length sprite_count */
};

struct sprite_mode {
	uint32_t colorbpp;
	/* maskbpp denotes the amount of alpha bpp used
	 * while mask_width is the bits used to store the mask.
	 * Old modes have the same mask_width as their colorbpp, but the value
	 * is always all-zeroes or all-ones.
	 * New modes can have 1bpp or 8bpp masks
	 */
	uint32_t maskbpp;
	uint32_t mask_width; /* in pixels */
	uint32_t xdpi;
	uint32_t ydpi;
	rosprite_color_model color_model;
};

struct sprite_palette {
	uint32_t size; /* in number of entries (each entry is a word) */
	uint32_t* palette;
};

struct sprite {
	unsigned char name[13]; /* last byte for 0 terminator */
	struct sprite_mode* mode;
	bool has_mask;
	bool has_palette;
	uint32_t palettesize; /* in number of entries (each entry is a word) */
	uint32_t* palette;
	uint32_t width; /* width in pixels */
	uint32_t height; /* height in pixels */
	uint32_t* image; /* image data in 0xRRGGBBAA words */
};

void sprite_init(void);
struct sprite_area* sprite_load_file(FILE* f);
void rosprite_destroy_sprite_area(struct sprite_area *);

struct sprite_palette* rosprite_load_palette(FILE* f);
void rosprite_destroy_palette(struct sprite_palette *);


#endif
