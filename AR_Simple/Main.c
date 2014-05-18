#ifdef _WIN32
	#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

// OpenGL & GLUT
#ifndef __APPLE__
	#include <GL/gl.h>
	#include <GL/glut.h>
#else
	#include <OpenGL/gl.h>
	#include <GLUT/glut.h>
#endif

// ARToolkit
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#include <SDL.h>
#include <SDL_image.h>

// Global variables
ARUint8*		dataPtr;
int             img_width, img_height; // With and height of loaded image
int             thresh = 100;

char           *cparam_name = "Data/camera_para.dat";
ARParam         cparam;

/*
Get the RGB value of the pixel at position x,y on the given surface
Returns as Uint32, use SDL_GetRGBA to get RGBA values
*/
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
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
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

/*
	Load the image given by filename and saves data in dataPtr variable
*/
int loadImage(char* filename)
{
	SDL_Surface* img = IMG_Load(filename);
	if (!img)
	{
		printf("Image '%s' failed to load. Error: %s\n", filename, IMG_GetError());
		return -1;
	}
	img_width = img->w;
	img_height = img->h;

	printf("Image size: %i x %i\n", img->w, img->h);

	dataPtr = calloc(img->w * img->h * 4, sizeof(ARUint8)); // Allocate space for image data

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
	return 0;
}

/*
	Initialize ARToolkit
*/
static void init()
{
	ARParam  wparam;

	// set the initial camera parameters
	if (arParamLoad(cparam_name, 1, &wparam) < 0) 
	{
		printf("Camera parameter load error !!\n");
		exit(0);
	}

	arParamChangeSize(&wparam, img_width, img_height, &cparam);
	arInitCparam(&cparam);

	argInit(&cparam, 1.0, 0, 0, 0, 0); // open the graphics window
}

/* 
	(This probably is not needed anymore, but leaving it be for now...)
cleanup function called when program exits 

*/
static void cleanup()
{
	arVideoCapStop();
	arVideoClose();
	
	argCleanup();
}

/*
	Handles key press events while program is running
*/
static void keyEvent(unsigned char key, int x, int y)
{
	// quit if the ESC key is pressed
	if (key == 0x1b) 
	{
		cleanup();
		exit(0);
	}
}

/*
	Main loop for ARToolkit. Runs until program exits
*/
static void mainLoop(void)
{
	ARMarkerInfo    *marker_info;
	int             marker_num;

	argDrawMode2D();
	//argDispImage(dataPtr, 0, 0);

	// detect the markers in the video frame
	if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0) 
	{		
		cleanup();
		exit(0);
	}

	printf("%i patterns found\n", marker_num);
	
	// If you uncomment the following code the program will run until it detects at least 1 marker.
	// It will then print out the pattern locations and exit
	if (marker_num > 0)
	{
		printf("Found %i markers:\n", marker_num);
		for (int i = 0; i < marker_num; i++) // Print position of each marker
		{
			printf("marker %i: %f, %f\n", i, marker_info[i].pos[0], marker_info[i].pos[1]);
		}
		exit(0);
	}

	argSwapBuffers();
}

int main(int argc, char **argv)
{
	if (loadImage("images/image_huge.jpg") == -1) exit(1); // Load the test image into memory

	glutInit(&argc, argv);
	init();

	//argMainLoop(NULL, keyEvent, mainLoop);
	mainLoop();
	return (0);
}
