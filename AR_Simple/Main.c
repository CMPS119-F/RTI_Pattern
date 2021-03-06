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
#include <AR/arMulti.h>

#include "FileWriter.h"
#include "ImageLoader.h"
#include "object.h"

// Global variables
ARUint8*		dataPtr;
int             img_width, img_height; // With and height of loaded image
int             thresh = 100;

char* patt_name = "Data/multi.pattern"; // The specs for the patterns to look for
ARMultiMarkerInfoT* config;

char           *cparam_name = "Data/camera_para.dat";
ARParam         cparam;
ObjectData_T* object;
int objectNum = -1; // Number of patterns in file pointed to by patt_name

int** centers;		// Contains the center for each image
int img_num = 0;    // The currect image the program is on

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

	if (objectNum == -1) // If pattern data has not been loaded yet
	{
		if ((object = read_ObjData(patt_name, &objectNum)) == NULL)
		{
			exit(1);
		}
	}

	argInit(&cparam, 1.0, 0, 0, 0, 0); // open the graphics window
}

/*
	Helper functions, ideally we should move these to another file instead of main.c
*/

/*
	Get the distance between two points
	coord_1: the first point
	coord_2: the second point
*/
double get_dist(double* coord_1, double* coord_2)
{
	double result = sqrt(pow(coord_2[1] - coord_1[1], 2) + pow(coord_2[0] - coord_1[0], 2));
	return  result;
}

/*
	Get the position of the 2 markers to use for calulcating the center (the location of
	the sphere)
	pair: returned with position of the 2 markers. e.g. pair[0] = [1234,293], pair[1] = [1234,2382]
	marker_info: taken from arDetectMarker function
	marker_num: total number of markers detected
	pattern_num: how many patterns were identified
*/
void get_pair(double** pair, ARMarkerInfo *marker_info, int marker_num, int pattern_num)
{
	double max = 0;
	double curr = 0;
	for (int i = 0; i < marker_num; i++) {
		for (int j = 0; j < marker_num, j != i; j++) {
			if (marker_info[i].id != -1 && marker_info[j].id != -1) {
				curr = get_dist(marker_info[i].pos, marker_info[j].pos);
				if (curr > max) {
					max = curr;
					pair[0] = marker_info[i].pos;
					pair[1] = marker_info[j].pos;
				}
			}
		}
	}
}

/*
	Calculate the midpoint between two x,y, points
	midpoint: array to fill with the x,y position
	pair: the pair of x,y points to use for the calculation
*/
void get_midpoint(double* midpoint, double** pair)
{
	midpoint[0] = (pair[0][0] + pair[1][0]) / 2.0;
	midpoint[1] = (pair[0][1] + pair[1][1]) / 2.0;
}

/*
	Calculate the average sphere location using x,y data from 'centers' array
	Writes the result to the output.csv file
	num: number of images in centers array
*/
void calculateAverage(int num)
{
	int rx = 0;
	int ry = 0;
	for (int i = 0; i < num; i++) // Calculate average center for all images
	{
		if (centers[i][0] != 0 && centers[i][1] != 0) // Ignores images that pattern detection failed on
		{
			rx += centers[i][0];
			ry += centers[i][1];
		}
		else num--;
	}
	rx /= num; ry /= num;
	printf("Average location: %i, %i\n", rx, ry);
	writeLine("Average", rx, ry, 0,0,0,0,0,0,0,0);
}

/*
	Main loop for ARToolkit. Runs until program exits
	img_name: The name of the image file. This is used when writing results to csv file
*/

static void mainLoop(char* img_name)
{
	ARMarkerInfo    *marker_info;
	int             marker_num;

	// detect the markers in the video frame
	if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0) 
	{		
		printf("Marker detection (arDetectMarker) failed on %s. Skipping this image...\n", img_name);
		return;
	}

	printf("%d potential markers found\n", marker_num);
	printf("The following patterns have been identified: ");
	int pattern_count = 0;
	int marker_loc[8]; // For saving x,y positions of each marker
	for (int i = 0; i < 8; i++) marker_loc[i] = 0;

	for (int i = 0; i < marker_num; i++)
	{
		if (marker_info[i].id != -1) // If this marker matches a pattern
		{
			pattern_count++; // Keep count of recognized patterns
			for (int j = 0; j < objectNum; j++) // Check which pattern this is
			{
				if (marker_info[i].id == object[j].id)
				{
					marker_loc[marker_info[i].id * 2] = marker_info[i].pos[0]; // Save x position
					marker_loc[marker_info[i].id * 2 + 1] = marker_info[i].pos[1]; // Save y position
					printf("%s ", object[j].name);
					break;
				}
			}
		}
	}
	if (pattern_count == 0) printf("No patterns could be identified...\n");
	else printf("\n");

	if (pattern_count > 2) // If at least 2 known patterns have been found.
	{
		double midpoint[2];
		double pair[2][2];
		get_pair(&pair, marker_info, marker_num, pattern_count);
		get_midpoint(&midpoint, &pair);
		printf("Center is: %5i,%5i\n", (int)midpoint[0], (int)midpoint[1]);
		writeLine(img_name, (int)midpoint[0], (int)midpoint[1],
			marker_loc[0], marker_loc[1],
			marker_loc[2], marker_loc[3],
			marker_loc[4], marker_loc[5],
			marker_loc[6], marker_loc[7]);
		centers[img_num][0] = (int)midpoint[0]; // Save the x position
		centers[img_num++][1] = (int)midpoint[1]; // Save the y position
	}
	else
	{
		printf("%i patterns isnt enough to find a center point!\n", marker_num);
		writeLine(img_name, 0, 0,
			marker_loc[0], marker_loc[1],
			marker_loc[2], marker_loc[3],
			marker_loc[4], marker_loc[5],
			marker_loc[6], marker_loc[7]);
		centers[img_num][0] = 0; // Save the x position
		centers[img_num++][1] = 0; // Save the y position
	}
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

	centers = calloc((argc - 1), sizeof(int)); // Allocate space to hold sphere location for each image
	for (int i = 0; i < argc - 1; i++) centers[i] = calloc(2, sizeof(int));

	createOutputFile(); // Delete any old output.csv file and create a fresh one
	for (int i = 1; i < argc; i++) // For each input image
	{
		dataPtr = loadImage(argv[i], &img_width, &img_height);
		if (dataPtr == NULL)
		{
			printf("Failed to load image: '%s' It will be skipped...\n", argv[i]);
			continue;
		}
		init(); // Initialize ARToolkit for this size of image
		printf("Running on %s:\n", argv[i]);
		mainLoop(argv[i]); // Detect sphere location on this image
		free(dataPtr);
		printf("=================== Done! ===================\n\n");
	}

	calculateAverage(argc - 1);

	printf("============================\n");
	printf("See output.csv for results\n");
	printf("============================\n");
	return 0;
}
