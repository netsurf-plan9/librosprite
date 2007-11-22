#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define LOGDBG(...) printf(__VA_ARGS__);

#define BTUINT(b) (b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24))

struct sprite_area_control_block {
	uint32_t spriteCount;
	uint32_t extensionBytes; /* size of extensionWords byte array */
	unsigned char* extensionWords;
};

struct sprite_mode {
	uint32_t colorbpp;
	uint32_t xdpi;
	uint32_t ydpi;
};

struct sprite_header {
	char name[13];
	struct sprite_mode* mode;
	bool hasMask;
	uint32_t maskbpp;
	bool hasPalette;
	uint32_t paletteSize;
	uint32_t* palette;
};

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

void sprite_read_bytes(FILE* stream, unsigned char* buf, size_t count)
{
	size_t bytesRead = fread(buf, 1, count, stream);
	
	if (bytesRead != count) {
		printf("Unexpected EOF\n");
		exit(EXIT_FAILURE);
	}
}

struct sprite_mode* sprite_get_mode(uint32_t spriteMode)
{
	static struct sprite_mode oldmodes[256];
	for (uint32_t i = 0; i < 256; i++) {
		oldmodes[i].colorbpp = 0;
		oldmodes[i].xdpi     = 0;
		oldmodes[i].ydpi     = 0;
	}

	struct sprite_mode* mode = malloc(sizeof(struct sprite_mode));

	uint32_t spriteType = spriteMode >> 27; /* preserve bits 27-31 only */
	LOGDBG("spriteType %u\n", spriteType);

	if (spriteType != 0) {
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
			mode->colorbpp = 23; break; /* CMYK */
		case 8:
			mode->colorbpp = 24; break;
		}
	} else {
		memcpy(mode, &(oldmodes[spriteMode]), sizeof(struct sprite_mode));
	}

	/*LOGDBG("verticalDPI %u\n", verticalDPI);
	LOGDBG("horizontalDPI %u\n", horizontalDPI);*/

	return mode;
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

	uint32_t spriteCount = sprite_read_word(spritefile);
	uint32_t firstSpriteOffset = sprite_read_word(spritefile);
	uint32_t firstFreeWordOffset = sprite_read_word(spritefile);
	uint32_t extensionByteCount = 16 - firstSpriteOffset;

	LOGDBG("spriteCount %u\n", spriteCount);
	LOGDBG("firstSpriteOffset %u\n", firstSpriteOffset);
	LOGDBG("firstFreeWordOffset %u\n", firstFreeWordOffset);
	LOGDBG("extensionByteCount %u\n", extensionByteCount);

	unsigned char* extensionWords = NULL;
	if (extensionByteCount > 0) {
		extensionWords = malloc(extensionByteCount); /* where to free() this? */
		sprite_read_bytes(spritefile, extensionWords, extensionByteCount);
	}

	for (uint32_t i = 0; i < spriteCount; i++) {
		uint32_t nextSpriteOffset = sprite_read_word(spritefile);

		unsigned char* name = malloc(13); /* last byte for 0 terminator */
		sprite_read_bytes(spritefile, name, 12);
		name[12] = '\0';

		uint32_t width         = sprite_read_word(spritefile);
		uint32_t height        = sprite_read_word(spritefile);
		uint32_t firstUsedBit  = sprite_read_word(spritefile); /* old format only (spriteType = 0) */
		uint32_t lastUsedBit   = sprite_read_word(spritefile);
		uint32_t imageOffset   = sprite_read_word(spritefile);
		assert(imageOffset >= 44); /* should never be smaller than the size of the header) */

		uint32_t maskOffset    = sprite_read_word(spritefile);
		uint32_t spriteModeWord= sprite_read_word(spritefile);

		struct sprite_mode* spriteMode = sprite_get_mode(spriteModeWord);

		uint32_t paletteSize  = imageOffset - 44;
		bool hasPalette = (paletteSize > 0);

		bool hasMask;
		uint32_t imageSize;
		uint32_t maskSize;

		if (imageOffset == maskOffset) {
			hasMask   = false;
			imageSize = nextSpriteOffset - 44 - paletteSize;
			maskSize  = 0;
		} else {
			hasMask   = true;
			imageSize = maskOffset - imageOffset;
			maskSize  = nextSpriteOffset - 44 - paletteSize - imageSize;
		}
		
		LOGDBG("\nLoading sprite %u\n", i);
		LOGDBG("nextSpriteOffset %u\n", nextSpriteOffset);
		LOGDBG("name %s\n", name);
		LOGDBG("width %u\n", width);
		LOGDBG("height %u\n", height);
		LOGDBG("firstUsedBit %u\n", firstUsedBit);
		LOGDBG("lastUsedBit %u\n", lastUsedBit);
		LOGDBG("imageOffset %u\n", imageOffset);
		LOGDBG("maskOffset %u\n", maskOffset);
		LOGDBG("spriteModeWord %u\n", spriteModeWord);
		LOGDBG("\tcolorbpp %u\n", spriteMode->colorbpp);
		LOGDBG("\txdpi %u\n", spriteMode->xdpi);
		LOGDBG("\tydpi %u\n", spriteMode->ydpi);

		LOGDBG("hasPalette %s\n", hasPalette ? "YES" : "NO");
		LOGDBG("paletteSize %u\n", paletteSize);
		LOGDBG("imageSize %u\n", imageSize);
		LOGDBG("hasMask %s\n", hasMask ? "YES" : "NO");
		LOGDBG("maskSize %u\n", maskSize);
		
		uint32_t* palette = NULL;
		if (paletteSize > 0) {
			assert(paletteSize % 8 == 0);
			palette = malloc(sizeof(uint32_t) * paletteSize);
			uint32_t paletteEntries = paletteSize / 8;

			/* Each palette entry is two words big
			 * The section entry is a duplicate of the first
             * I think this is in case you ever wanted flashing colours
             */
			for (uint32_t j = 0; j < paletteEntries; j++) {
				uint32_t word1 = sprite_read_word(spritefile);
				uint32_t word2 = sprite_read_word(spritefile);
				assert(word1 == word2); /* TODO: if they aren't, START FLASHING */
				
				palette[j] = word1;
			}
		}

		unsigned char* image = malloc(imageSize);
		sprite_read_bytes(spritefile, image, imageSize);

		unsigned char* mask = malloc(maskSize);
		sprite_read_bytes(spritefile, mask, maskSize);
	}

	return EXIT_SUCCESS;
}
