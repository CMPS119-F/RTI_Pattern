#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "FileWriter.h"

void createOutputFile()
{
	FILE* file = fopen("output.csv", "w"); // This will delete any existing file
	if (file != NULL)
	{
		fputs("image,x,y\n", file);
		fclose(file);
	}
}

void writeLine(char* img_name, int x, int y)
{
	FILE* file = fopen("output.csv", "a");
	char* line = calloc(strlen(img_name) + 8 + 3, sizeof(char));

	strcpy(line, img_name);
	strcat(line, ",");
	char buffer[10];
	sprintf(buffer, "%d", x);
	strcat(line, buffer);
	strcat(line, ",");
	sprintf(buffer, "%d", y);
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
