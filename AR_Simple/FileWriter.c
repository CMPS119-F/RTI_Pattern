#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "FileWriter.h"

void createOutputFile()
{
	FILE* file = fopen("output.csv", "w"); // This will delete any existing file
	if (file != NULL)
	{
		fputs("image,center_x,center_y,A_x,A_y,B_x,B_y,D_x,D_y,G_x,G_y\n", file);
		fclose(file);
	}
}

void writeLine(char* img_name, int x, int y,
	int ax, int ay,
	int bx, int by,
	int dx, int dy,
	int gx, int gy)
{
	FILE* file = fopen("output.csv", "a");
	char* line = calloc(strlen(img_name) + 40 + 3, sizeof(char));

	strcpy(line, img_name);
	strcat(line, ",");
	char buffer[256];
	sprintf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", x, y, 
		ax, ay, 
		bx, by, 
		dx, dy, 
		gx, gy);
	strcat(line, buffer);
	strcat(line, "\n");

	if (file != NULL)
	{
		fputs(line, file);
		fclose(file);
	}
	else
	{
		printf("output.csv could not be opened!\n");
		exit(1);
	}
}
