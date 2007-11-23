#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define LOGDBG(...) printf(__VA_ARGS__);

#define BTUINT(b) (b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24))

struct sprite_area {
	uint32_t sprite_count;
	uint32_t extension_size; /* size of extension_words in bytes */
	uint8_t* extension_words;
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
	bool haspalette;
	uint32_t palettesize;
	uint32_t* palette;
	uint32_t width; /* width and height in _pixels_ */
	uint32_t height;
	uint32_t* image;
};

struct sprite_header {
	uint32_t width_words; /* width in words */
	/* height defined in sprite struct */
	uint32_t first_used_bit; /* old format only (spriteType = 0) */
	uint32_t last_used_bit;
	uint32_t image_size; /* bytes */
};

static struct sprite_mode oldmodes[256];

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

void sprite_load_high_color(uint8_t* image_in, uint8_t* mask, struct sprite* sprite, struct sprite_header* header)
{
	mask = mask;

	sprite->image = malloc(sprite->width * sprite->height * 4); /* all image data is 32bpp going out */

	uint32_t currentByteIndex = 0; /* only for standalone test -- fread() will maintain this */
	uint32_t bpp = sprite->mode->colorbpp;
	uint32_t bytesPerPixel = bpp / 8;

	/* TODO: waste */
	
	for (uint32_t y = 0; y < sprite->height; y++) {
		for (uint32_t x = 0; x < header->width_words * 32 /* 32 bits per word */; x+=bpp) {
			uint32_t pixel = 0;
			for (uint32_t j = 0; j < bytesPerPixel; j++) {
				uint8_t b = image_in[currentByteIndex++];
				pixel = pixel | (b << (j * 8));
			}
			printf("%4x", pixel);
			/* TODO: put pixels in sprite->image */
		}
		printf("\n");
	}
}

void sprite_load_low_color(uint8_t* image_in, uint8_t* mask, struct sprite* sprite, struct sprite_header* header)
{
	mask = mask; /* TODO: mask */

	sprite->image = malloc(sprite->width * sprite->height * 4); /* all image data is 32bpp going out */

	uint32_t current_word_index = 0;
	uint32_t bpp = sprite->mode->colorbpp;
	uint32_t bitmask = (1 << bpp) - 1; /* creates a mask of 1s that is bpp bits wide */
	uint32_t currentword = ((uint32_t*) image_in)[current_word_index++];
	
	for (uint32_t y = 0; y < sprite->height; y++) {
		for (uint32_t x = 0; x < header->width_words * 32 /* 32 bits per word */; x+=bpp) {
			
			bool waste = false;
			if (x >= header->width_words * 32 - header->last_used_bit - 1) {
				waste = true;
			} /* TODO: left wastage */

			uint32_t offset_into_word = x % 32;

			if (!waste) {
				uint32_t pixel = (currentword & (bitmask << offset_into_word)) >> offset_into_word;
				printf("%2x", pixel);
				/* TODO: put pixels in sprite->image */
			}
	
			if (offset_into_word + bpp == 32) {
				/* TODO: assert not exceeding image size */
				currentword = ((uint32_t*)image_in)[current_word_index++];
			}
		}
		printf("\n");
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
	LOGDBG("first: %u last: %u\n", header->first_used_bit, header->last_used_bit);

	uint32_t imageOffset    = sprite_read_word(spritefile);
	assert(imageOffset >= 44); /* should never be smaller than the size of the header) */

	uint32_t maskOffset     = sprite_read_word(spritefile);
	uint32_t spriteModeWord = sprite_read_word(spritefile);

	sprite->mode = sprite_get_mode(spriteModeWord);
	assert(sprite->mode->colorbpp > 0);
	assert(sprite->mode->xdpi > 0);
	assert(sprite->mode->ydpi > 0);

	/* TODO l/r wastage */
	
	assert((header->last_used_bit + 1) % sprite->mode->colorbpp == 0);
	/*assert(header->width_words % sprite->mode->colorbpp == 0);*/
	sprite->width = (header->width_words * 32 / sprite->mode->colorbpp) - ((header->last_used_bit + 1) / sprite->mode->colorbpp);

	sprite->palettesize     = imageOffset - 44;
	sprite->haspalette      = (sprite->palettesize > 0);

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
	
	uint32_t* palette = NULL;
	if (sprite->haspalette) {
		assert(sprite->palettesize % 8 == 0);
		palette = malloc(sizeof(uint32_t) * sprite->palettesize);
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
			
			palette[j] = word1;
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

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: libsprite spritefile\n");
		exit(EXIT_FAILURE);
	}

	char* filename = argv[1];

	FILE* spritefile = fopen(filename, "rb");
	if (spritefile == NULL) {
		printf("Can't load spritefile %s\n", filename);
		exit(EXIT_FAILURE);
	}

	LOGDBG("Loading %s\n", filename);
	sprite_init();

	struct sprite_area* sprite_area = malloc(sizeof(struct sprite_area));

	sprite_area->sprite_count = sprite_read_word(spritefile);

	uint32_t firstSpriteOffset = sprite_read_word(spritefile);
	/*uint32_t firstFreeWordOffset = */sprite_read_word(spritefile); /* TODO: use this for some sanity checking? */
	sprite_area->extension_size = 16 - firstSpriteOffset;

	LOGDBG("sprite_count %u\n", sprite_area->sprite_count);
	LOGDBG("extension_size %u\n", sprite_area->extension_size);

	sprite_area->extension_words = NULL;
	if (sprite_area->extension_size > 0) {
		sprite_area->extension_words = malloc(sprite_area->extension_size); /* where to free() this? */
		sprite_read_bytes(spritefile, sprite_area->extension_words, (size_t) (sprite_area->extension_size));
	}

	for (uint32_t i = 0; i < sprite_area->sprite_count; i++) {
		struct sprite* sprite = sprite_load_sprite(spritefile);
		LOGDBG("\nname %s\n", sprite->name);
		LOGDBG("colorbpp %u\n", sprite->mode->colorbpp);
		LOGDBG("xdpi %u\n", sprite->mode->xdpi);
		LOGDBG("ydpi %u\n", sprite->mode->ydpi);
		LOGDBG("width %u px\n", sprite->width);
		LOGDBG("height %u px\n", sprite->height);
	
		LOGDBG("hasPalette %s\n", sprite->haspalette ? "YES" : "NO");
		if (sprite->haspalette) LOGDBG("paletteSize %u\n", sprite->palettesize);

		LOGDBG("hasMask %s\n", sprite->hasmask ? "YES" : "NO");
		if (sprite->hasmask) LOGDBG("maskbpp %u\n", sprite->mode->maskbpp);
	}

	fclose(spritefile);

	return EXIT_SUCCESS;
}

/*
	       word boundary
	             |
32bpp:	bbggrr00 | bbggrr00
24bpp:	bbggrrbb | ggrr....
16bpp:	bgr0bgr0 | bgr0bgr0
*/
