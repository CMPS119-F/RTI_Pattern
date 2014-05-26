Automatic sphere-finder for RTIBuilder
===========

This program is designed to automate the process of locating the highlight sphere in Cultural Heritage Imaging's RTIBuilder software. This project is a proof-of-concept. It demonstrates that it is indeed possible to completely automate the sphere-finding process.

### How it works
This program will recognize a special pattern, found in **Pattern.pdf**, in an image that you provide. You simply glue the sphere onto the center of the pattern (where the X is) and take your pictures as you normally would.

### Instructions
- Print out the pattern in **Pattern.pdf**
	- Feel free to scale the picture down appropriately for smaller sphere sizes. The program is capable of detecting the pattern even when it's very small.
- Glue this pattern onto something solid such as a piece of cardboard
- Attach the sphere directly on the X in the image.
- Take your pictures (At least 3 of the patterns need to visible in an image to locate the sphere)
- If needed, convert your images to .jpg format

### Running the program
1. Download program from "Releases" section
2. There are two ways to run the program:
	- Drag-and-drop method:
		- Select your images and drag-and-drop them onto the executable (**detect_sphere.exe**). The images must be in the same directory as **detect_sphere.exe**
		- A window should open, run the pattern detection, and close
	- Command-line method:
		- cd into the directory containing the program
		- Run as: *detect_sphere.exe image1.jpg image2.jpg image3.jpg*
3. You will now see a file **output.csv**. This file will contain the detected location (center) of the sphere for each image. The last line contains the average center if multiple images were used.
	- If the program failed to detect patterns in an image the location will show up as 0,0

### Results
The program detects patterns consistently. As long as at least 3 patterns are visible, the sphere's center is usually located within 1-2 pixels.

### Issues
- Pattern detection may fail on large images with the message "Marker detection (arDetectMarker) failed on example_image.jpg. Skipping this image...". The exact cause has not been identified, but it may be due to embedded color profiles. It is recommended to simply try opening the image with Photoshop or GIMP, discard the embedded color profile, and re-save the image.

### Compiling
This project is 'self-contained'. The libs, dll's, and headers for the ARToolkit the SDL libraries are included. You should be able to 'git clone' this repository, open the .sln file with Visual Studio (we used 2013), and compile without any issues.

### The Team
- Skylar Brown
- Michael Grijalva
- Wallace Luk
