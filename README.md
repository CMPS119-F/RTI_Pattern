Automatic sphere-finder for RTIBuilder
===========

This program is designed to automate the process of locating the highlight sphere in Cultural Heritage Imaging's RTIBuilder software. This project is a proof-of-concept. It demonstrates that it is indeed possible to completely automate the sphere-finding process.

### How it works
This program will recognize a special pattern, found in **Pattern.pdf**, in an image that you provide. You simply glue the sphere onto the center of the pattern (where the X is) and take your pictures as you normally would.

### Instructions
- Print out the pattern in Pattern.pdf
	- Feel free to scale the picture down appropriately for smaller sphere sizes
- Glue this pattern onto something solid such as a piece of cardboard
- Attach the sphere directly on the X in the image.
- Take your pictures
- If needed, convert your images to .jpg format

### Running the program
1. Download program from "Releases" section
2. There are two ways to run the program:
	- Drag-and-drop method:
		- Select your images and drag-and-drop them onto the executable (**AR_simple.exe**)
	- Command-line method:
		- Run as: **AR_Sample.exe image1.jpg image2.jpg image3.jpg**
- You will now see a file **output.csv**. This file will contain the detected location (center) of the sphere for each image.

### Results
The program detects patterns consistently. As long as at least 3 patterns are visible, the sphere's center is usually located within 1-2 pixels.

### Issues
Bugs, issues, etc. will go here...

### The Team
- Skylar Brown
- Michael Grijalva
- Wallace Luk
