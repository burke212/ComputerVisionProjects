// OpenCVTest.cpp : Defines the entry point for the console application.
//
#include <opencv2/opencv.hpp>

using namespace cv;

String win_DogOriginal = "Dog Original Image";
String win_DogBox = "Dog Box Filter Image";


int main(int argv, char** argc)
{
	Mat inputImage = imread("dog.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.
	Mat outputImage;

	// Initialize & define Size and Point objects.
	Size size = Size(45, 45);
	Point point = Point(-1, -1);

	imshow(win_DogOriginal, inputImage);// Show original Dog image.

	// Apply a Box Filter effect.
	boxFilter(inputImage, outputImage, 50, size, point, true, BORDER_DEFAULT);
	
	imshow(win_DogBox, outputImage);// Show the Dog image after applying a box filter.
	imwrite("DogBoxFilterImage.bmp", outputImage);// Save the Dog image after applying a box filter.

	waitKey();

	return 0;
}