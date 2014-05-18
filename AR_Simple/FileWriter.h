#ifndef _FILEWRITER_H_
#define _FILEWRITER_H_

/*
	Create the output.csv file and
	write the headers to the file
*/
void createOutputFile();

/*
	Write a new line to the output.csv file
	This function will create the output.csv file (with headers) if 
	it does not already exist
	img_name: name of the image file
	x: x position of the sphere for this image
	y: y position of the sphere for this image
*/
void writeLine(char* img_name, int x, int y);

#endif
