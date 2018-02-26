// OpenCVTest.cpp : Defines the entry point for the console application.
//
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

String win_DogOriginal = "Dog Original Image";
String win_DogBox = "Dog Box Filter Image";
String win_BicycleOriginal = "Bicycle Original Image";
String win_BicycleSobel = "Bicycle Sobel Filter Image";


void boxFilter() {
	Mat DogInputImage = imread("dog.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.
	Mat DogOutputImage;

	// Initialize & define Size and Point objects.
	Size size = Size(45, 45);
	Point point = Point(-1, -1);

	imshow(win_DogOriginal, DogInputImage);// Show original Dog image.

										   // Apply a Box Filter effect.
	boxFilter(DogInputImage, DogOutputImage, 50, size, point, true, BORDER_DEFAULT);

	imshow(win_DogBox, DogOutputImage);// Show the Dog image after applying a box filter.
	imwrite("DogBoxFilterImage.bmp", DogOutputImage);// Save the Dog image after applying a box filter.
}

int sobelFilter() {
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	
	Mat BicycleInputImage = imread("bicycle.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.
	Mat BicycleOutputImage, BicycleGradient, BicycleGradX, BicycleGradY, ABS_BicycleGradX, ABS_BicycleGradY;

	if (BicycleInputImage.empty()) {// Stop prog if image doesn't load.
		cout << "Couldn't load image." << endl;
		return -1;
	}
	
	imshow(win_BicycleOriginal, BicycleInputImage);// Show original bicycle image.
	
	GaussianBlur(BicycleInputImage, BicycleOutputImage, Size(3, 3), 0, 0, BORDER_DEFAULT);

	cvtColor(BicycleInputImage, BicycleOutputImage, COLOR_BGR2GRAY);

	Sobel(BicycleOutputImage, BicycleGradX, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	Sobel(BicycleOutputImage, BicycleGradY, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(BicycleGradX, ABS_BicycleGradX);
	convertScaleAbs(BicycleGradY, ABS_BicycleGradY);

	addWeighted(ABS_BicycleGradX, 0.5, ABS_BicycleGradY, 0.5, 0, BicycleGradient);

	imshow(win_BicycleSobel, BicycleGradient);

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



	return 0;
}