// OpenCVTest.cpp : Defines the entry point for the console application.
//Opens/ plays a video.
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <stdio.h>
#include <string>

using namespace std;
using namespace cv;

// Global Variables
String original_img_win = "Original Image";

Size kern3x3 = Size(3,3);

Mat original_img = imread("blocks_L-150x150.png", CV_LOAD_IMAGE_COLOR);



void showOriginalImg() {
	imshow(original_img_win, original_img);

	waitKey(0);
}


void applyGuassianBlur(Mat source, Size kern) {
	Mat dest;

	GaussianBlur(source, dest, kern, 0, 0, BORDER_DEFAULT);// Apply gaussian blur to image. 
	imshow("Gaussian", dest);
	waitKey(0);
}

void calculateOctave1(Mat source) {
	Mat dest=source;
	Size kern = Size(3, 3);

	// 4 Octaves
	for (int i = 1; i < 5; i++) {
		String octave = "Octave ";
		octave = octave + to_string(i);// make string for each Octave, i.e. "Octave 1", "Octave 2"...
		
		// 5 layers/ images per octave
		for (int j = 1; j < 6; j++) {
			String layer = "Layer ";
			layer = octave + layer + to_string(j);// make string for each Octave, i.e. "layer 1", "layer 2"...

			GaussianBlur(dest, dest, kern, 0, 0, BORDER_DEFAULT);
			cout << "width: " << dest.rows << endl;
			cout << "height: " << dest.cols << endl;
			//namedWindow(layer, WINDOW_NORMAL);// Keeps window sizes the same as the image. Stretches image size to user controlled size.
			imshow(layer, dest);

			kern = kern + Size(2,2);// Increase the kernel size to 3 5 7 9 11
		}

		resize(dest, dest, Size(dest.cols*0.5, dest.rows*0.5), 0.5, 0.5); // Resize the image | half the image size. 
		cout << "Itration " << i << endl;
		cout << "	Image width: " << dest.rows << endl;
		cout << "	Image height: " << dest.cols << endl;
	}


	waitKey(0);
}


int main(int argc, char argv[])
{
	// Show the original image.
	showOriginalImg();
	
	// Apply 3x3 gauss on original image
	//applyGuassianBlur(original_img, kern3x3);

	calculateOctave1(original_img);

	return 0;
}