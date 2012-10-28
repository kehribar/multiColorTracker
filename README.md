## multiColorTracker

Computationally optimal color based multiple object tracker implementation with OpenCV 2.X

Idea of classifiyng upto 32 different objects using logical AND operations, comes from this paper:

"Fast and Cheap Color Image Segmentation for Interactive Robots"
http://www.cs.cmu.edu/~mmv/papers/wirevision00.pdf

Color tracking is implemented in YUV space.

Someday in future, I hope I will implement this algorithm on FPGA based embedded environment. 

## Usage

I tested this program with OpenCV 2.4 and Ubuntu 12.10 based system. You should easily compile the program by 'make main' if you have OpenCV installed.

After you compiled the program, you can select an object color by clicking on the image.

Left mouse click and right mouse click define two seperate colors for two objects.

You can chose the color tolerance values via trackbars.

I'll write more details in near future soon.

ihsan.