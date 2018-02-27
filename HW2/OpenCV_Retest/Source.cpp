// OpenCVTest.cpp : Defines the entry point for the console application.
//

/*
	Program displays an image of a dog & what it looks like after applying a Box (Mean) Filter. 

	Also displays an image of a bicycle & what it looks like after applying a Sobel Filter, and after applying a Gaussian Filter. 
*/
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

String win_DogOriginal = "Dog Original Image";
String win_DogBox = "Dog Box Filter Image";
String win_BicycleOriginal = "Bicycle Original Image";
String win_BicycleSobel = "Bicycle Sobel Filter Image";

Size kSize = Size(3, 3);

Mat BicycleInputImage = imread("bicycle.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.
Mat DogInputImage = imread("dog.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.


void boxFilter() {
	Mat DogOutputImage;

	// Initialize & define Size and Point objects.
	Size kSize_Box = Size(45, 45);
	Point point = Point(-1, -1);

	imshow(win_DogOriginal, DogInputImage);// Show original Dog image.

	// Apply a Box Filter effect.
	boxFilter(DogInputImage, DogOutputImage, 50, kSize_Box, point, true, BORDER_DEFAULT);

	convertScaleAbs(DogOutputImage, DogOutputImage);// Converts image back into 8-bit format.

	imshow(win_DogBox, DogOutputImage);// Show the Dog image after applying a box filter.
}

int sobelFilter() {
	int kSize_Sobel = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	
	Mat BicycleOutputImage, BicycleGradient, BicycleGradX, BicycleGradY, ABS_BicycleGradX, ABS_BicycleGradY;

	// Stop prog if image doesn't load.
	if (BicycleInputImage.empty()) {
		cout << "Couldn't load image." << endl;
		return -1;
	}
	
	imshow(win_BicycleOriginal, BicycleInputImage);// Show original bicycle image.
	
	GaussianBlur(BicycleInputImage, BicycleOutputImage, kSize, 0, 0, BORDER_DEFAULT); // Apply gaussian blur filter.

	cvtColor(BicycleInputImage, BicycleOutputImage, COLOR_BGR2GRAY); // Convert image to greyscale. 

	Sobel(BicycleOutputImage, BicycleGradX, ddepth, 1, 0, kSize_Sobel, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for X derivative.
	Sobel(BicycleOutputImage, BicycleGradY, ddepth, 0, 1, kSize_Sobel, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for Y derivative.

	convertScaleAbs(BicycleGradX, ABS_BicycleGradX);// Convert sobel filter image to 8-bit for X derivatives.
	convertScaleAbs(BicycleGradY, ABS_BicycleGradY);// Convert sobel filter image to 8-bit for Y derivatives.

	addWeighted(ABS_BicycleGradX, 0.5, ABS_BicycleGradY, 0.5, 0, BicycleGradient);// Combine X & Y derivatives calculations into 1 Mat.

	imshow(win_BicycleSobel, BicycleGradient);// Show the output image with sobel filter applied. 

	waitKey();
	return 0;
}

int gaussFilter() {	
	Mat gaussBicycleOutput;

	// Stop prog if image doesn't load.
	if (BicycleInputImage.empty()) {
		cout << "Couldn't load image." << endl;
		return -1;
	}

	GaussianBlur(BicycleInputImage, gaussBicycleOutput, kSize, 0, 0, BORDER_DEFAULT);// Apply gaussian blur to image. 

	imshow("Gauss Filter", gaussBicycleOutput);// Show original bicycle image.

	waitKey();

	return 0;
}


int main(int argv, char** argc)
{
	boxFilter(); // Perform box filter on the Dog image.

	// Check if 'Enter' is pressed. Triggers next filter output to show.
	if (waitKey() == 13) {
		sobelFilter();		
	}

	// Check if 'Enter' is pressed. Triggers next filter output to show.
	if (waitKey() == 13){
		gaussFilter();
	}

	return 0;
}