#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "libsprite.h"

#define LOGDBG(...) printf(__VA_ARGS__);

#define BTUINT(b) (b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24))
#define BSWAP(word) (((word & (0x000000ff)) << 24) | ((word & 0x0000ff00) << 8) | ((word & 0x00ff0000) >> 8) | ((word & 0xff000000) >> 24))

struct sprite_header {
	uint32_t width_words; /* width in words */
	/* height defined in sprite struct */
	uint32_t first_used_bit; /* old format only (spriteType = 0) */
	uint32_t last_used_bit;
	uint32_t image_size; /* bytes */
	uint32_t mask_size; /* bytes */
};

struct sprite_mask_state {
	uint32_t x;
	uint32_t y;
	uint32_t max_x;
	uint32_t max_y;
	uint32_t offset_into_word;
	uint32_t current_byte_index;
	uint32_t current_word;
};

static struct sprite_mode oldmodes[256];

static const uint8_t sprite_16bpp_translate[] = {
	0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39,
	0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
	0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd,
	0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff
};

/* palettes generated with palette2c.c
 * which in turn requires sprite_load_palette(FILE* f)
 * defined in this file
 */
static const uint32_t sprite_1bpp_palette[] = { 0xffffff00, 0x0 };

static const uint32_t sprite_2bpp_palette[] = { 0xffffff00, 0xbbbbbb00, 0x77777700, 0x0 };

static const uint32_t sprite_4bpp_palette[] = {
0xffffff00, 0xdddddd00, 0xbbbbbb00, 0x99999900,
0x77777700, 0x55555500, 0x33333300, 0x0,
0x449900, 0xeeee0000, 0xcc0000, 0xdd000000,
0xeeeebb00, 0x55880000, 0xffbb0000, 0xbbff00
};

static const uint32_t sprite_8bpp_palette[] = {
0x0, 0x11111100, 0x22222200, 0x33333300,
0x44000000, 0x55111100, 0x66222200, 0x77333300,
0x4400, 0x11115500, 0x22226600, 0x33337700,
0x44004400, 0x55115500, 0x66226600, 0x77337700,
0x88000000, 0x99111100, 0xaa222200, 0xbb333300,
0xcc000000, 0xdd111100, 0xee222200, 0xff333300,
0x88004400, 0x99115500, 0xaa226600, 0xbb337700,
0xcc004400, 0xdd115500, 0xee226600, 0xff337700,
0x440000, 0x11551100, 0x22662200, 0x33773300,
0x44440000, 0x55551100, 0x66662200, 0x77773300,
0x444400, 0x11555500, 0x22666600, 0x33777700,
0x44444400, 0x55555500, 0x66666600, 0x77777700,
0x88440000, 0x99551100,0xaa662200, 0xbb773300,
0xcc440000, 0xdd551100, 0xee662200, 0xff773300,
0x88444400, 0x99555500, 0xaa666600, 0xbb777700,
0xcc444400, 0xdd555500, 0xee666600, 0xff777700,
0x880000, 0x11991100, 0x22aa2200, 0x33bb3300,
0x44880000, 0x55991100, 0x66aa2200, 0x77bb3300,
0x884400, 0x11995500, 0x22aa6600, 0x33bb7700,
0x44884400, 0x55995500, 0x66aa6600, 0x77bb7700,
0x88880000, 0x99991100, 0xaaaa2200, 0xbbbb3300,
0xcc880000, 0xdd991100, 0xeeaa2200, 0xffbb3300,
0x88884400, 0x99995500, 0xaaaa6600, 0xbbbb7700,
0xcc884400, 0xdd995500, 0xeeaa6600, 0xffbb7700,
0xcc0000, 0x11dd1100, 0x22ee2200, 0x33ff3300,
0x44cc0000, 0x55dd1100, 0x66ee2200, 0x77ff3300,
0xcc4400, 0x11dd5500, 0x22ee6600, 0x33ff7700,
0x44cc4400, 0x55dd5500, 0x66ee6600, 0x77ff7700,
0x88cc0000, 0x99dd1100, 0xaaee2200, 0xbbff3300,
0xcccc0000, 0xdddd1100, 0xeeee2200, 0xffff3300,
0x88cc4400, 0x99dd5500, 0xaaee6600, 0xbbff7700,
0xcccc4400, 0xdddd5500, 0xeeee6600, 0xffff7700,
0x8800, 0x11119900, 0x2222aa00, 0x3333bb00,
0x44008800, 0x55119900, 0x6622aa00, 0x7733bb00,
0xcc00, 0x1111dd00, 0x2222ee00, 0x3333ff00,
0x4400cc00, 0x5511dd00, 0x6622ee00, 0x7733ff00,
0x88008800, 0x99119900, 0xaa22aa00, 0xbb33bb00,
0xcc008800, 0xdd119900, 0xee22aa00, 0xff33bb00,
0x8800cc00, 0x9911dd00, 0xaa22ee00, 0xbb33ff00,
0xcc00cc00, 0xdd11dd00, 0xee22ee00, 0xff33ff00,
0x448800, 0x11559900, 0x2266aa00, 0x3377bb00,
0x44448800, 0x55559900, 0x6666aa00, 0x7777bb00,
0x44cc00, 0x1155dd00, 0x2266ee00, 0x3377ff00,
0x4444cc00, 0x5555dd00, 0x6666ee00, 0x7777ff00,
0x88448800, 0x99559900, 0xaa66aa00, 0xbb77bb00,
0xcc448800, 0xdd559900, 0xee66aa00, 0xff77bb00,
0x8844cc00, 0x9955dd00, 0xaa66ee00, 0xbb77ff00,
0xcc44cc00, 0xdd55dd00, 0xee66ee00, 0xff77ff00,
0x888800, 0x11999900, 0x22aaaa00, 0x33bbbb00,
0x44888800, 0x55999900, 0x66aaaa00, 0x77bbbb00,
0x88cc00, 0x1199dd00, 0x22aaee00, 0x33bbff00,
0x4488cc00, 0x5599dd00, 0x66aaee00, 0x77bbff00,
0x88888800, 0x99999900, 0xaaaaaa00, 0xbbbbbb00,
0xcc888800, 0xdd999900, 0xeeaaaa00, 0xffbbbb00,
0x8888cc00, 0x9999dd00, 0xaaaaee00, 0xbbbbff00,
0xcc88cc00, 0xdd99dd00, 0xeeaaee00, 0xffbbff00,
0xcc8800, 0x11dd9900, 0x22eeaa00, 0x33ffbb00,
0x44cc8800, 0x55dd9900, 0x66eeaa00, 0x77ffbb00,
0xcccc00, 0x11dddd00, 0x22eeee00, 0x33ffff00,
0x44cccc00, 0x55dddd00, 0x66eeee00, 0x77ffff00,
0x88cc8800, 0x99dd9900, 0xaaeeaa00, 0xbbffbb00,
0xcccc8800, 0xdddd9900, 0xeeeeaa00, 0xffffbb00,
0x88cccc00, 0x99dddd00, 0xaaeeee00, 0xbbffff00,
0xcccccc00, 0xdddddd00, 0xeeeeee00, 0xffffff00
};

void sprite_init(void)
{
	for (uint32_t i = 0; i < 256; i++) {
		oldmodes[i].colorbpp = 0;
		oldmodes[i].maskbpp  = 1;
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
		oldmodes[i].mask_width = oldmodes[i].colorbpp;
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
		mode->mask_width = mode->maskbpp;
		mode->xdpi = (spriteMode & 0x07ffc000) >> 14; /* preserve bits 14-26 only */
		mode->ydpi = (spriteMode & 0x00003ffe) >> 1; /* preserve bits 1-13 only */

		mode->color_model = SPRITE_RGB;
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
			mode->colorbpp = 32;
			mode->color_model = SPRITE_CMYK;
			break;
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
	 /* because we're dealing with 8bpp or less */
	if (sprite->has_palette) {
		assert(pixel <= sprite->palettesize); /* TODO: what to do if your color depth is bigger than palette? */
		translated_pixel = sprite->palette[pixel];
	} else {
		switch (sprite->mode->colorbpp) {
		case 8:
			assert(pixel < 256);
			translated_pixel = sprite_8bpp_palette[pixel];
			break;
		case 4:
			assert(pixel < 16);
			translated_pixel = sprite_4bpp_palette[pixel];
			break;
		case 2:
			assert(pixel < 4);
			translated_pixel = sprite_2bpp_palette[pixel];
			break;
		case 1:
			assert(pixel < 2);
			translated_pixel = sprite_1bpp_palette[pixel];
			break;
		default:
			assert(false);
		}
	}
	return translated_pixel;
}

/* TODO: could make static inline? */
uint32_t sprite_cmyk_to_rgb(uint32_t cmyk)
{
        const uint8_t c = cmyk & 0xff;
        const uint8_t m = (cmyk & 0xff00) >> 8;
        const uint8_t y = (cmyk & 0xff0000) >> 16;
        const uint8_t k = cmyk >> 24;
 
        /* Convert to CMY colourspace */
        const uint8_t C = c + k;
        const uint8_t M = m + k;
        const uint8_t Y = y + k;
 
        /* And to RGB */
        const uint8_t r = 255 - C;
        const uint8_t g = 255 - M;
        const uint8_t b = 255 - Y;
 
        return r << 24 | g << 16 | b << 8;
}

/* TODO: could make static inline? */
uint32_t sprite_upscale_color(uint32_t pixel, struct sprite_mode* mode)
{
	switch (mode->colorbpp) {
	case 32:
		if (mode->color_model == SPRITE_RGB) {
			/* swap from 0xAABBGGRR to 0xRRGGBBAA */
			/*uint8_t alpha = pixel & (0xff << 24);
			 TODO: think about mask/alpha */
			return BSWAP(pixel);
		} else {
			return sprite_cmyk_to_rgb(pixel);
		}
	case 24:
		/* reverse byte order */
		return BSWAP(pixel);
	case 16:
		/* incoming format is b_00000000000000000bbbbbgggggrrrrr */
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
		return pixel;
		}
	case 8:
	case 4:
	case 2:
	case 1:
		assert(false); /* shouldn't need to call for <= 8bpp, since a palette lookup will return 32bpp */
	default:
		assert(false); /* unknown bpp */
	}
}

struct sprite_mask_state* sprite_init_mask_state(struct sprite* sprite, struct sprite_header* header, uint8_t* mask)
{
	struct sprite_mask_state* mask_state = malloc(sizeof(struct sprite_mask_state));

	mask_state->x = header->first_used_bit;
	mask_state->y = 0;
	mask_state->max_x = header->width_words * 32 - (31 - header->last_used_bit) - 1;
	mask_state->max_y = sprite->height - 1;
	mask_state->offset_into_word = mask_state->x % 32;
	mask_state->current_byte_index = 0;
	mask_state->current_word = BTUINT((mask + mask_state->current_byte_index));

	return mask_state;
}

/* Get the next mask byte.
 * Mask of 0xff denotes 100% opaque, 0x00 denotes 100% transparent
 */
uint8_t sprite_next_mask_pixel(struct sprite* sprite, struct sprite_header* header, struct sprite_mask_state* mask_state)
{
	/* a 1bpp mask (for new mode sprites), each row is word aligned (therefore potential righthand wastage */
	sprite = sprite; header = header; mask_state = mask_state;
	

	return 0xff;
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
			
			pixel = sprite_upscale_color(pixel, sprite->mode);
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
	mask = mask;
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


	/* sprite has no mask if imageOffset == maskOffset */
	if (imageOffset == maskOffset) {
		sprite->has_mask   = false;
		header->image_size = nextSpriteOffset - 44 - sprite->palettesize;
		header->mask_size  = 0;
	} else {
		sprite->has_mask   = true;
		header->image_size = maskOffset - imageOffset;
		header->mask_size  = nextSpriteOffset - 44 - sprite->palettesize - header->image_size;
	}

	if (sprite->has_mask) LOGDBG("mask_size (bits) %u\n", header->mask_size * 8);
	if (sprite->has_mask) LOGDBG("	w*h %u\n", sprite->width * sprite->height);
	
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
			
			/* swap rr and bb parts (seems to give the right result, but where is it documented? PRM1-731 */
			uint32_t entry = ((word1 & 0xff000000) >> 16) | (word1 & 0x00ff0000) | ((word1 & 0x0000ff00) << 16);
			sprite->palette[j] = entry;
		}
	}

	uint8_t* image = malloc(header->image_size);
	sprite_read_bytes(spritefile, image, header->image_size);

	uint8_t* mask = NULL;
	if (sprite->has_mask) {
		mask = malloc(header->mask_size);
		sprite_read_bytes(spritefile, mask, header->mask_size);
	}

	/* sanity check image_size */
	assert((header->width_words) * 4 * (sprite->height) == header->image_size);
	/* TODO: sanity check mask_size */
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

struct sprite_palette* sprite_load_palette(FILE* f)
{
	/* Palette file is in groups of 6 bytes, each is a VDU 19 (set palette)
	 * http://www.drobe.co.uk/show_manual.php?manual=/sh-cgi?manual=Vdu%26page=19 */

	/* TODO: currently assume palette has linear entries (2nd byte in is 00, 01, 02 etc) */
	struct sprite_palette* palette = malloc(sizeof(struct sprite_palette));

	palette->palette = malloc(sizeof(uint32_t) * 256); /* allocate 256 whether we need them all or not */

	uint32_t c = 0;
	uint8_t b[6];

	size_t bytesRead = fread(&b, 1, 6, f);
	assert(bytesRead % 6 == 0);
	while (bytesRead == 6) {
		assert(b[0] == 19); /* VDU 19 */

		/* only process logical colours */
		if (b[2] == 16) {
			/*assert(c == b[1]);*/
	
			uint32_t entry = (b[3] << 24) | (b[4] << 16) | (b[5] << 8);
			palette->palette[c] = entry;
	
			c++;
			assert(c <= 256);
		}

		bytesRead = fread(&b, 1, 6, f);
		assert(bytesRead % 6 == 0);
	}

	palette->size = c;

	return palette;
}
