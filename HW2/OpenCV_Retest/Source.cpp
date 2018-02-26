// OpenCVTest.cpp : Defines the entry point for the console application.
//
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argv, char** argc)
{
	Mat testColour = imread("2980_2560x1440.jpg", CV_LOAD_IMAGE_COLOR);// Load image w/ color.
	Mat testGrey = imread("2980_2560x1440.jpg", CV_LOAD_IMAGE_GRAYSCALE);// Load image as grayscale.

	imshow("color", testColour);// Open window to show testColour image.
	imshow("grey", testGrey);// Open window to show testGrey image.

	imwrite("outputGrey.jpg", testGrey);// Save testGrey image as outputGrey.jpg.

	waitKey();// Have system wait until key is pressed. This is so we can see what the output is before the program terminates.

	return 0;
}