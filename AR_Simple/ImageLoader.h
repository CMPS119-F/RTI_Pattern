#ifndef _IMAGELOADER_H_
#define _IMAGELOADER_H_

#include <SDL.h>
#include <AR/ar.h>

/*
	Get the pixel value at x,y from the provided image (an SDL_Surface)	
*/
Uint32 getpixel(SDL_Surface *surface, int x, int y);

/*
	Load the image and convert it into the correct pixel format

	Returns pointer to pixel data.
	Returns NULL if image failed to load

	filename: the .jpg image to load
	dataPtr: the pointer to copy pixel data to
	w: Will be assigned the width of image
	h: Will be assigned the height of the image
*/
ARUint8* loadImage(char* filename, int* w, int* h);

#endif
