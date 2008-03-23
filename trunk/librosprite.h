#ifndef ROSPRITE_H
#define ROSPRITE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum { rosprite_rgb, rosprite_cmyk } rosprite_color_model;

typedef int (*reader)(uint8_t* buf, size_t count, void* ctx);

struct rosprite_file_context;

struct rosprite_area {
	uint32_t extension_size; /* size of extension_words in bytes */
	uint8_t* extension_words;
	uint32_t sprite_count;
	struct rosprite** sprites; /* array of length sprite_count */
};

struct rosprite_mode {
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

struct rosprite_palette {
	uint32_t size; /* in number of entries (each entry is a word) */
	uint32_t* palette;
};

struct rosprite {
	unsigned char name[13]; /* last byte for 0 terminator */
	struct rosprite_mode mode;
	bool has_mask;
	bool has_palette;
	uint32_t palettesize; /* in number of entries (each entry is a word) */
	uint32_t* palette;
	uint32_t width; /* width in pixels */
	uint32_t height; /* height in pixels */
	uint32_t* image; /* image data in 0xRRGGBBAA words */
};

struct rosprite_file_context* rosprite_create_file_context(FILE* f);
void rosprite_destroy_file_context(struct rosprite_file_context* ctx);
int rosprite_file_reader(uint8_t* buf, size_t count, void* ctx);

struct rosprite_mem_context* rosprite_create_mem_context(uint8_t* p, unsigned long total_size);
void rosprite_destroy_mem_context(struct rosprite_mem_context* ctx);
int rosprite_mem_reader(uint8_t* buf, size_t count, void* ctx);

struct rosprite_area* rosprite_load(reader reader, void* ctx);
void rosprite_destroy_sprite_area(struct rosprite_area *);

struct rosprite_palette* rosprite_load_palette(reader reader, void* ctx);
void rosprite_destroy_palette(struct rosprite_palette *);


#endif
