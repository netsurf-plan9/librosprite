#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

/* method for 16bpp and above (>1 byte per pixel) */
void sprite_high_colour()
{
	/*uint32_t imgSize = 8;*/ /* in bytes */
	/*               0xddccbbaa              0xeeeeffff */
	uint8_t img[] = {0xaa, 0xbb, 0xcc, 0xdd, 0xff, 0xff, 0xee, 0xee};
	uint32_t widthPixels = 2;
	uint32_t currentByteIndex = 0; /* only for standalone test -- fread() will maintain this */

	uint32_t bpp = 24;
	uint32_t bytesPerPixel = bpp >> 3; /* divide by 8 */
	
	
	for (uint32_t x = 0; x < widthPixels; x++) {
		uint32_t pixel = 0;
		for (uint32_t j = 0; j < bytesPerPixel; j++) {
			uint8_t b = img[currentByteIndex++];
			pixel = pixel | (b << (j * 8));
		}
		printf("%x\n", pixel);
	}
}

void sprite_low_colour()
{
	uint8_t img[] = {0xab, 0xcd, 0x12, 0x34 };
	uint32_t widthInBits = 32;
	uint32_t currentByteIndex = 0; /* only for standalone test -- fread() will maintain this */

	uint32_t bpp = 8;

	uint32_t bitmask = (1 << bpp) - 1; /* creates a mask of 1s that is bpp bits wide */
	uint8_t currentbyte = img[currentByteIndex++];
	
	for (uint32_t x = 0; x < widthInBits; x+=bpp) {
		uint32_t offset_into_byte = x % 8;
		uint32_t pixel = (currentbyte & (bitmask << offset_into_byte)) >> offset_into_byte;
		printf("%x\n", pixel);

		if (offset_into_byte + bpp == 8) {
			currentbyte = img[currentByteIndex++];
		}
	}
}

int main()
{
	sprite_low_colour();

	return EXIT_SUCCESS;
}
