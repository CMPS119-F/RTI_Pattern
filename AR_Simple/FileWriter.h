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
	ax & ay: Location of marker A
	bx & by: Location of marker B
	dx & dy: Location of marker D
	gx & gy: Location of marker G
*/
void writeLine(char* img_name, int x, int y, 
	int ax, int ay, 
	int bx, int by, 
	int dx, int dy, 
	int gx, int gy);

#endif
