#include <SDL_image.h>
#include "ImageLoader.h"

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	// Here p is the address to the pixel we want to retrieve
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0; // shouldn't happen, but avoids warnings
	}
}

ARUint8* loadImage(char* filename, ARUint8* dataPtr, int* w, int* h)
{
	SDL_Surface* img = IMG_Load(filename);
	if (!img)
	{
		printf("Image '%s' failed to load. Error: %s\n", filename, IMG_GetError());
		return NULL;
	}
	*w = img->w; // Assigned width and height to the given pointers
	*h = img->h;

	dataPtr = (ARUint8*)calloc(img->w * img->h * 4, sizeof(ARUint8)); // Allocate space for image data

	// Write image data to the dataPtr variable
	for (int y = 0; y < img->h; y++)
	{
		for (int x = 0; x < img->w; x++)
		{
			Uint8 r, g, b, a;
			SDL_GetRGBA(getpixel(img, x, y), img->format, &r, &g, &b, &a);
			int i = ((y * img->w) + x) * 4;
			dataPtr[i] = b;
			dataPtr[i + 1] = g;
			dataPtr[i + 2] = r;
			dataPtr[i + 3] = 0;
		}
	}
	SDL_FreeSurface(img);
	return dataPtr;
}