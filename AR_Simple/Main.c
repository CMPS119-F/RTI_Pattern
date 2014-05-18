#ifdef _WIN32
	#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

// SDL
#include <SDL.h>
#include <SDL_image.h>

#include "FileWriter.h"

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
	Helper functions, ideall we should move these to another file instead of main.c
*/

double get_dist(double* coord_1, double* coord_2){
	double result = sqrt(pow((coord_2[1] - coord_1[1]), 2) + pow((coord_2[0] - coord_2[1]), 2));
	return  result;
}

void get_pair(double** pair, ARMarkerInfo* marker_info, int marker_num){
	double ab = get_dist(marker_info[0].pos, marker_info[1].pos);
	double ac = get_dist(marker_info[0].pos, marker_info[2].pos);
	double bc = get_dist(marker_info[1].pos, marker_info[2].pos);
	double max = max(max(ab, ac), bc);
	if (max == ab){
		pair[0] = marker_info[0].pos;
		pair[1] = marker_info[1].pos;
	}
	else if (max == ac){
		pair[0] = marker_info[0].pos;
		pair[1] = marker_info[2].pos;

	}
	else if (max == bc){
		pair[0] = marker_info[1].pos;
		pair[1] = marker_info[2].pos;

	}
}

void get_midpoint(double* midpoint, double** pair){
	midpoint[0] = (max(pair[0][0], pair[1][0]) + min(pair[0][0], pair[1][0])) / 2.0;
	midpoint[1] = (max(pair[0][1], pair[1][1]) + min(pair[0][1], pair[1][1])) / 2.0;
}

/*
	Main loop for ARToolkit. Runs until program exits
	img_name: The name of the image file. This is used when writing results to csv file
*/

static void mainLoop(char* img_name)
{
	ARMarkerInfo    *marker_info;
	int             marker_num;

	argDrawMode2D();

	// detect the markers in the video frame
	if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0) 
	{		
		cleanup();
		exit(0);
	}
	printf("%i patterns found\n", marker_num);
	// If you uncomment the following code the program will run until it detects at least 1 marker.
	// It will then print out the pattern locations and exit
	if (marker_num > 2)
	{
		//printf("Found %i markers:\n", marker_num);
		/*for (int i = 0; i < marker_num; i++) // Print position of each marker
		{	
			printf("marker %i: %f, %f\n", i, marker_info[i].pos[0], marker_info[i].pos[1]);
		}*/
		double midpoint[2];
		double pair[2][2];
		get_pair(&pair, marker_info, marker_num);
		get_midpoint(&midpoint, &pair);
		printf("Center is: %5i,%5i\n", (int)midpoint[0], (int)midpoint[1]);
		writeLine(img_name, (int)midpoint[0], (int)midpoint[1]);
	}
	else
	{
		printf("%i patterns isnt enough to find a center point!\n", marker_num);
	}
	//argSwapBuffers();
}

int main(int argc, char **argv)
{
	if (argc == 1) // If no images were provided
	{
		printf("USAGE: AR_Simple.exe img1.jpg img2.jpg ...\n");
		exit(1);
	}

	for (int i = 1; i < argc; i++) // Check that each arg is a .jpg image
	{
		if (strstr(argv[i], ".jpg") == NULL)
		{
			printf("Images must be .jpg format. %s is not a valid file\n", argv[i]);
			exit(1);
		}
	}

	if (loadImage(argv[1]) == -1) // Load the first image (so init() can properly register the image size)
		exit(1);

	glutInit(&argc, argv);
	init();

	createOutputFile(); // Delete any old output.csv file and create a fresh one
	for (int i = 1; i < argc; i++) // For each input image
	{
		if (loadImage(argv[i]) == -1)
		{
			printf("Failed to load image: '%s' It will be skipped...\n", argv[i]);
			continue;
		}
		printf("Running on %s:\n", argv[i]);
		mainLoop(argv[i]); // Detect sphere location on this image
		printf("=================== Done! ===================\n");
	}
	printf("============================\n");
	printf("See output.csv for results\n");
	printf("============================\n");
	return 0;
}
