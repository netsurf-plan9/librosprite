#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "libsprite.h"

#define LOGDBG(...) printf(__VA_ARGS__);

#define BTUINT(b) (b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24))

struct sprite_header {
	uint32_t width_words; /* width in words */
	/* height defined in sprite struct */
	uint32_t first_used_bit; /* old format only (spriteType = 0) */
	uint32_t last_used_bit;
	uint32_t image_size; /* bytes */
};

static struct sprite_mode oldmodes[256];

static uint8_t sprite_16bpp_translate[] = {
	0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39,
	0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
	0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd,
	0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff
};

void sprite_init()
{
	for (uint32_t i = 0; i < 256; i++) {
		oldmodes[i].colorbpp = 0;
		oldmodes[i].xdpi     = 0;
		oldmodes[i].ydpi     = 0;
	}

	/* TODO: push this into a text file? */
	/* TODO: check these DPIs */
	oldmodes[0].colorbpp  = 1; oldmodes[0].xdpi  = 90; oldmodes[0].ydpi  = 45;
	oldmodes[1].colorbpp  = 2; oldmodes[1].xdpi  = 45; oldmodes[1].ydpi  = 45;
	oldmodes[2].colorbpp  = 4; oldmodes[2].xdpi  = 22; oldmodes[2].ydpi  = 45;
	oldmodes[4].colorbpp  = 1; oldmodes[4].xdpi  = 45; oldmodes[4].ydpi  = 45;
	oldmodes[5].colorbpp  = 2; oldmodes[5].xdpi  = 22; oldmodes[5].ydpi  = 45;
	oldmodes[8].colorbpp  = 2; oldmodes[8].xdpi  = 90; oldmodes[8].ydpi  = 45;
	oldmodes[9].colorbpp  = 4; oldmodes[9].xdpi  = 45; oldmodes[9].ydpi  = 45;
	oldmodes[10].colorbpp = 8; oldmodes[10].xdpi = 22; oldmodes[10].ydpi = 45;
	oldmodes[11].colorbpp = 2; oldmodes[11].xdpi = 90; oldmodes[11].ydpi = 45;
	oldmodes[12].colorbpp = 4; oldmodes[12].xdpi = 90; oldmodes[12].ydpi = 45;
	oldmodes[13].colorbpp = 8; oldmodes[13].xdpi = 45; oldmodes[13].ydpi = 45;
	oldmodes[14].colorbpp = 4; oldmodes[14].xdpi = 90; oldmodes[14].ydpi = 45;
	oldmodes[15].colorbpp = 8; oldmodes[15].xdpi = 90; oldmodes[15].ydpi = 45;
	oldmodes[16].colorbpp = 4; oldmodes[16].xdpi = 90; oldmodes[16].ydpi = 45;
	oldmodes[17].colorbpp = 4; oldmodes[17].xdpi = 90; oldmodes[17].ydpi = 45;
	oldmodes[18].colorbpp = 1; oldmodes[18].xdpi = 90; oldmodes[18].ydpi = 90;
	oldmodes[19].colorbpp = 2; oldmodes[19].xdpi = 90; oldmodes[19].ydpi = 90;
	oldmodes[20].colorbpp = 4; oldmodes[20].xdpi = 90; oldmodes[20].ydpi = 90;
	oldmodes[21].colorbpp = 8; oldmodes[21].xdpi = 90; oldmodes[21].ydpi = 90;
	oldmodes[22].colorbpp = 4; oldmodes[22].xdpi = 90; oldmodes[22].ydpi = 45;
	oldmodes[23].colorbpp = 1; oldmodes[23].xdpi = 90; oldmodes[23].ydpi = 90;
	oldmodes[24].colorbpp = 8; oldmodes[24].xdpi = 90; oldmodes[24].ydpi = 45;
	oldmodes[25].colorbpp = 1; oldmodes[25].xdpi = 90; oldmodes[25].ydpi = 90;
	oldmodes[26].colorbpp = 2; oldmodes[26].xdpi = 90; oldmodes[26].ydpi = 90;
	oldmodes[27].colorbpp = 4; oldmodes[27].xdpi = 90; oldmodes[27].ydpi = 90;
	oldmodes[28].colorbpp = 8; oldmodes[28].xdpi = 90; oldmodes[28].ydpi = 90;
	oldmodes[29].colorbpp = 1; oldmodes[29].xdpi = 90; oldmodes[29].ydpi = 90;
	oldmodes[30].colorbpp = 2; oldmodes[30].xdpi = 90; oldmodes[30].ydpi = 90;
	oldmodes[31].colorbpp = 4; oldmodes[31].xdpi = 90; oldmodes[31].ydpi = 90;
	oldmodes[33].colorbpp = 1; oldmodes[33].xdpi = 90; oldmodes[33].ydpi = 45;
	oldmodes[34].colorbpp = 2; oldmodes[34].xdpi = 90; oldmodes[34].ydpi = 45;
	oldmodes[35].colorbpp = 4; oldmodes[35].xdpi = 90; oldmodes[35].ydpi = 45;
	oldmodes[36].colorbpp = 8; oldmodes[36].xdpi = 90; oldmodes[36].ydpi = 45;
	oldmodes[37].colorbpp = 1; oldmodes[37].xdpi = 90; oldmodes[37].ydpi = 45;
	oldmodes[38].colorbpp = 2; oldmodes[38].xdpi = 90; oldmodes[38].ydpi = 45;
	oldmodes[39].colorbpp = 4; oldmodes[39].xdpi = 90; oldmodes[39].ydpi = 45;
	oldmodes[40].colorbpp = 8; oldmodes[40].xdpi = 90; oldmodes[40].ydpi = 45;
	oldmodes[41].colorbpp = 1; oldmodes[41].xdpi = 90; oldmodes[41].ydpi = 45;
	oldmodes[42].colorbpp = 2; oldmodes[42].xdpi = 90; oldmodes[42].ydpi = 45;
	oldmodes[43].colorbpp = 4; oldmodes[43].xdpi = 90; oldmodes[43].ydpi = 45;
	oldmodes[44].colorbpp = 1; oldmodes[44].xdpi = 90; oldmodes[44].ydpi = 45;
	oldmodes[45].colorbpp = 2; oldmodes[45].xdpi = 90; oldmodes[45].ydpi = 45;
	oldmodes[46].colorbpp = 4; oldmodes[46].xdpi = 90; oldmodes[46].ydpi = 45;

	/* old modes have the same mask bpp as their colour bpp -- PRM1-781 */
	for (uint32_t i = 0; i < 256; i++) {
		oldmodes[i].maskbpp = oldmodes[i].colorbpp;
	}
}

uint32_t sprite_read_word(FILE* stream)
{
	unsigned char b[4];
	size_t bytesRead = fread(b, 1, 4, stream);
	
	if (bytesRead != 4) {
		printf("Unexpected EOF\n"); /* TODO, have better error handling, don't exit here */
		exit(EXIT_FAILURE);
	}

	return BTUINT(b);
}

void sprite_read_bytes(FILE* stream, uint8_t* buf, size_t count)
{
	size_t bytesRead = fread(buf, 1, count, stream);
	
	if (bytesRead != count) {
		printf("Unexpected EOF\n");
		exit(EXIT_FAILURE);
	}
}

struct sprite_mode* sprite_get_mode(uint32_t spriteMode)
{
	struct sprite_mode* mode = malloc(sizeof(struct sprite_mode));

	uint32_t spriteType = (spriteMode & 0x78000000) >> 27; /* preserve bits 27-30 only */

	if (spriteType != 0) {
		bool hasEightBitAlpha = (spriteMode & 0x80000000) >> 31; /* bit 31 */
		/* new modes have 1bpp masks (PRM5a-111)
		 * unless bit 31 is set (http://select.riscos.com/prm/graphics/sprites/alphachannel.html)
		 */
		mode->maskbpp = (hasEightBitAlpha ? 8 : 1);
		mode->xdpi = (spriteMode & 0x07ffc000) >> 14; /* preserve bits 14-26 only */
		mode->ydpi = (spriteMode & 0x00003ffe) >> 1; /* preserve bits 1-13 only */
		switch (spriteType) {
		case 1:
			mode->colorbpp = 1; break;
		case 2:
			mode->colorbpp = 2; break;
		case 3:
			mode->colorbpp = 4; break;
		case 4:
			mode->colorbpp = 8; break;
		case 5:
			mode->colorbpp = 16; break;
		case 6:
			mode->colorbpp = 32; break;
		case 7:
			mode->colorbpp = 32; break; /* CMYK */
		case 8:
			mode->colorbpp = 24; break;
		}
	} else {
		/* clone station mode and return */
		assert(spriteMode < 256); /* don't think you can have modes over 255? */
		memcpy(mode, &(oldmodes[spriteMode]), sizeof(struct sprite_mode));
	}

	return mode;
}

uint32_t sprite_palette_lookup(struct sprite* sprite, uint32_t pixel)
{
	uint32_t translated_pixel;
	assert(pixel < 256); /* because we're dealing with 8bpp or less */
	if (sprite->has_palette) {
		assert(pixel <= sprite->palettesize); /* TODO: what to do if your color depth is bigger than palette? */
		translated_pixel = sprite->palette[pixel];
	} else {
		/* TODO: use a default palette */
		translated_pixel = pixel;
	}
	return translated_pixel;
}

uint32_t sprite_upscale_color(uint32_t pixel, uint32_t bpp)
{
	switch (bpp) {
	case 32:
		/* reverse byte order */
		return ((pixel & (0x000000ff)) << 24) | ((pixel & 0x0000ff00) << 8) | ((pixel & 0x00ff0000) >> 8) | ((pixel & 0xff000000) >> 24);
	case 24:
		return pixel & 0x00001700; /* TODO: mask out alpha -- any point? */
	case 16:
		/* assume incoming format of b_00000000000000000bbbbbgggggrrrrr */
		{
			uint8_t red   = pixel & 31;
			uint8_t green = (pixel & (31 << 5)) >> 5;
			uint8_t blue  = (pixel & (31 << 10)) >> 10;

			/* sanity check */
			assert(red < 32);
			assert(green < 32);
			assert(blue < 32);

			pixel =   (sprite_16bpp_translate[red] << 24)
					| (sprite_16bpp_translate[green] << 16)
					| (sprite_16bpp_translate[blue] << 8);
			printf("%x %x %x ", red, green, blue);
		return pixel;
		}
	case 8:
	case 4:
	case 2:
	case 1:
		assert(false); /* shouldn't need to call for <= 8bpp, since a palette lookup will return 32bpp */
	default:
		assert(false); /* unknown bpp */
		break;
	}
}

void sprite_load_high_color(uint8_t* image_in, uint8_t* mask, struct sprite* sprite, struct sprite_header* header)
{
	mask = mask;

	sprite->image = malloc(sprite->width * sprite->height * 4); /* all image data is 32bpp going out */

	uint32_t currentByteIndex = 0; /* only for standalone test -- fread() will maintain this */
	const uint32_t bpp = sprite->mode->colorbpp;
	const uint32_t bytesPerPixel = bpp / 8;
	const uint32_t row_max_bit = header->width_words * 32 - (31 - header->last_used_bit); /* Last used bit in row */

	/* Spec says that there must be no left-hand wastage */
	assert(header->first_used_bit == 0);
	
	for (uint32_t y = 0; y < sprite->height; y++) {
		uint32_t x_pixels = 0;
		for (uint32_t x = 0; x < row_max_bit; x += bpp) {
			uint32_t pixel = 0;
			for (uint32_t j = 0; j < bytesPerPixel; j++) {
				uint8_t b = image_in[currentByteIndex++];
				pixel = pixel | (b << (j * 8));
			}
			
			pixel = sprite_upscale_color(pixel, bpp);
			sprite->image[y*sprite->width + x_pixels] = pixel;
			x_pixels++;
		}

		/* Ensure byte index is pointing at start of next row */
		if (y + 1 < sprite->height) {
			currentByteIndex = (currentByteIndex + 3) & ~3; /* Round up to next multiple of 4 */
		}
	}
}

void sprite_load_low_color(uint8_t* image_in, uint8_t* mask, struct sprite* sprite, struct sprite_header* header)
{
	mask = mask; /* TODO: mask */

	sprite->image = malloc(sprite->width * sprite->height * 4); /* all image data is 32bpp going out */

	const uint32_t bpp = sprite->mode->colorbpp;
	const uint32_t row_max_bit = header->width_words * 32 - (31 - header->last_used_bit); /* Last used bit in row */
	const uint32_t bitmask = (1 << bpp) - 1; /* creates a mask of 1s that is bpp bits wide */

	uint32_t current_byte_index = 0;
	uint32_t currentword = BTUINT((image_in + current_byte_index));
	current_byte_index += 4;
	
	for (uint32_t y = 0; y < sprite->height; y++) {
		uint32_t x_pixels = 0;
		for (uint32_t x = header->first_used_bit; x < row_max_bit ; x += bpp) {
			const uint32_t offset_into_word = x % 32;

			uint32_t pixel = (currentword & (bitmask << offset_into_word)) >> offset_into_word;
			pixel = sprite_palette_lookup(sprite, pixel); /* lookup returns 32bpp */
			sprite->image[y*sprite->width + x_pixels] = pixel;
			x_pixels++;

			/* If we're not at the end of the row and we've processed all of this word, fetch the next one */
			if (x + bpp < row_max_bit && offset_into_word + bpp == 32) {
				currentword = BTUINT((image_in + current_byte_index));
				current_byte_index += 4;
			}
		}

		/* Advance to first word of next row */
		if (y + 1 < sprite->height) {
			currentword = BTUINT((image_in + current_byte_index));
			current_byte_index += 4;
		}
	}
}

struct sprite* sprite_load_sprite(FILE* spritefile)
{
	uint32_t nextSpriteOffset = sprite_read_word(spritefile);

	struct sprite* sprite = malloc(sizeof(struct sprite));
	struct sprite_header* header = malloc(sizeof(struct sprite_header));

	sprite_read_bytes(spritefile, sprite->name, 12);
	sprite->name[12] = '\0';

	header->width_words     = sprite_read_word(spritefile) + 1; /* file has width - 1 and height - 1 */
	sprite->height          = sprite_read_word(spritefile) + 1;
	header->first_used_bit  = sprite_read_word(spritefile); /* old format only (spriteType = 0) */
	header->last_used_bit   = sprite_read_word(spritefile);

	uint32_t imageOffset    = sprite_read_word(spritefile);
	assert(imageOffset >= 44); /* should never be smaller than the size of the header) */

	uint32_t maskOffset     = sprite_read_word(spritefile);
	uint32_t spriteModeWord = sprite_read_word(spritefile);

	sprite->mode = sprite_get_mode(spriteModeWord);
	assert(sprite->mode->colorbpp > 0);
	assert(sprite->mode->xdpi > 0);
	assert(sprite->mode->ydpi > 0);

	/* TODO left-hand wastage */
	
	assert((header->last_used_bit + 1) % sprite->mode->colorbpp == 0);
	/*assert(header->width_words % sprite->mode->colorbpp == 0);*/
	sprite->width = (header->width_words * 32 - header->first_used_bit - (31 - header->last_used_bit)) / sprite->mode->colorbpp;

	sprite->palettesize     = imageOffset - 44;
	sprite->has_palette     = (sprite->palettesize > 0);

	uint32_t image_size;
	uint32_t maskSize;

	if (imageOffset == maskOffset) {
		sprite->hasmask = false;
		image_size = nextSpriteOffset - 44 - sprite->palettesize;
		maskSize  = 0;
	} else {
		sprite->hasmask   = true;
		image_size = maskOffset - imageOffset;
		maskSize  = nextSpriteOffset - 44 - sprite->palettesize - image_size;
	}

	header->image_size = image_size;

	if (sprite->hasmask) LOGDBG("maskSize %u\n", maskSize);
	
	if (sprite->has_palette) {
		assert(sprite->palettesize % 8 == 0);
		sprite->palette = malloc(sizeof(uint32_t) * sprite->palettesize);
		uint32_t paletteEntries = sprite->palettesize / 8;

		/* Each palette entry is two words big
		 * The second word is a duplicate of the first
		 * I think this is in case you ever wanted flashing colours
		 * PRM1-730
		 */
		for (uint32_t j = 0; j < paletteEntries; j++) {
			uint32_t word1 = sprite_read_word(spritefile);
			uint32_t word2 = sprite_read_word(spritefile);
			assert(word1 == word2); /* TODO: if they aren't, START FLASHING */
			
			/* swap rr and bb parts (seems to give the right result, but where is it documented? */
			uint32_t entry = ((word1 & 0xff000000) >> 16) | (word1 & 0x00ff0000) | ((word1 & 0x0000ff00) << 16);
			sprite->palette[j] = entry;
		}
	}

	uint8_t* image = malloc(image_size);
	sprite_read_bytes(spritefile, image, image_size);

	uint8_t* mask = malloc(maskSize);
	sprite_read_bytes(spritefile, mask, maskSize);

	/* sanity check imageSize */
	assert((header->width_words) * 4 * (sprite->height) == header->image_size);
	/* TODO: sanity check maskSize */
	if (sprite->mode->colorbpp > 8) {
		sprite_load_high_color(image, mask, sprite, header);
	} else {
		sprite_load_low_color(image, mask, sprite, header);
	}

	free(image);
	free(mask);
	free(header);

	return sprite;
}

struct sprite_area* sprite_load_file(FILE* f)
{
	struct sprite_area* sprite_area = malloc(sizeof(struct sprite_area));

	sprite_area->sprite_count = sprite_read_word(f);

	uint32_t firstSpriteOffset = sprite_read_word(f);
	/*uint32_t firstFreeWordOffset = */sprite_read_word(f); /* TODO: use this for some sanity checking? */
	sprite_area->extension_size = 16 - firstSpriteOffset;

	sprite_area->extension_words = NULL;
	if (sprite_area->extension_size > 0) {
		sprite_area->extension_words = malloc(sprite_area->extension_size); /* where to free() this? */
		sprite_read_bytes(f, sprite_area->extension_words, (size_t) (sprite_area->extension_size));
	}

	sprite_area->sprites = malloc(sizeof(struct sprite*) * sprite_area->sprite_count); /* allocate array of pointers */
	for (uint32_t i = 0; i < sprite_area->sprite_count; i++) {
		struct sprite* sprite = sprite_load_sprite(f);
		sprite_area->sprites[i] = sprite;
		
		
	}

	return sprite_area;
}

/*
	       word boundary
	             |
32bpp:	bbggrr00 | bbggrr00
24bpp:	bbggrrbb | ggrr....
16bpp:	bgr0bgr0 | bgr0bgr0
*/
