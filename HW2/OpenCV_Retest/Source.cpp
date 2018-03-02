// OpenCVTest.cpp : Defines the entry point for the console application.
//

/*
	Program displays an image of a dog & what it looks like after applying a Box (Mean) Filter. 
	 
	Also displays an image of a bicycle & what it looks like after applying a Sobel Filter, and after applying a Gaussian Filter. 

	\m/>.<\m/
*/
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

String win_DogOriginal = "Dog Original Image";
String win_DogBox1 = "Dog Box Filter Image | Window Size 45";
String win_DogBox2 = "Dog Box Filter Image | Window Size 15";
String win_DogBox3 = "Dog Box Filter Image | Window Size 75";

String win_BicycleOriginal = "Bicycle Original Image";
String win_BicycleSobel1 = "Bicycle Sobel Filter Image | Window Size 3x3";
String win_BicycleSobel2 = "Bicycle Sobel Filter Image | Window Size 9x9";
String win_BicycleSobel3 = "Bicycle Sobel Filter Image | Window Size 1x1";

String win_BicycleGauss1 = "Gauss Filter, Window Size 45";
String win_BicycleGauss2 = "Gauss Filter, Window Size 15";
String win_BicycleGauss3 = "Gauss Filter, Window Size 75";

Mat BicycleInputImage = imread("bicycle.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.
Mat DogInputImage = imread("dog.bmp", CV_LOAD_IMAGE_COLOR);// Load dog image.


int boxFilter() {
	Mat DogOutputImage;

	// Initialize & define Size and Point objects.
	Size kSize_Box1 = Size(45, 45);
	Size kSize_Box2 = Size(15, 15);
	Size kSize_Box3 = Size(75, 75);
	Point point = Point(-1, -1);

	// Stop prog if image doesn't load.
	if (DogInputImage.empty()) {
		cout << "Couldn't load image." << endl;
		return -1;
	}

	imshow(win_DogOriginal, DogInputImage);// Show original Dog image.

	/* 
	The following 3 sections of code apply a Box Filter with 3 different kernel sizes.
	The first section uses a window size of 45x45, the second 15x15, and the third 75x75.
	*/

	// Apply a Box Filter effect.
	boxFilter(DogInputImage, DogOutputImage, 50, kSize_Box1, point, true, BORDER_DEFAULT);

	convertScaleAbs(DogOutputImage, DogOutputImage);// Converts image back into 8-bit format.

	imshow(win_DogBox1, DogOutputImage);// Show the Dog image after applying a box filter.

	//======================================================================================================================

	// Apply a Box Filter effect.
	boxFilter(DogInputImage, DogOutputImage, 50, kSize_Box2, point, true, BORDER_DEFAULT);

	convertScaleAbs(DogOutputImage, DogOutputImage);// Converts image back into 8-bit format.

	imshow(win_DogBox2, DogOutputImage);// Show the Dog image after applying a box filter.

	//======================================================================================================================

	// Apply a Box Filter effect.
	boxFilter(DogInputImage, DogOutputImage, 50, kSize_Box3, point, true, BORDER_DEFAULT);

	convertScaleAbs(DogOutputImage, DogOutputImage);// Converts image back into 8-bit format.

	imshow(win_DogBox3, DogOutputImage);// Show the Dog image after applying a box filter.


	waitKey();

	return 0;
}

int sobelFilter() {
	int kSize_Sobel1 = 3;
	int kSize_Sobel2 = 9;
	int kSize_Sobel3 = 1;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	Size kSize = Size(3, 3);
	
	Mat BicycleOutputImage, BicycleGradient, BicycleGradX, BicycleGradY, ABS_BicycleGradX, ABS_BicycleGradY;

	// Stop prog if image doesn't load.
	if (BicycleInputImage.empty()) {
		cout << "Couldn't load image." << endl;
		return -1;
	}
	

	imshow(win_BicycleOriginal, BicycleInputImage);// Show original bicycle image.

	/*
	The following 3 sections of code apply a Box Filter with 3 different kernel sizes.
	The first section uses a window size of 3x3, the second 9x9, and the third 1x1.
	*/
	
	GaussianBlur(BicycleInputImage, BicycleOutputImage, kSize, 0, 0, BORDER_DEFAULT); // Apply gaussian blur filter.

	cvtColor(BicycleInputImage, BicycleOutputImage, COLOR_BGR2GRAY); // Convert image to greyscale. 

	Sobel(BicycleOutputImage, BicycleGradX, ddepth, 1, 0, kSize_Sobel1, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for X derivative.
	Sobel(BicycleOutputImage, BicycleGradY, ddepth, 0, 1, kSize_Sobel1, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for Y derivative.

	convertScaleAbs(BicycleGradX, ABS_BicycleGradX);// Convert sobel filter image to 8-bit for X derivatives.
	convertScaleAbs(BicycleGradY, ABS_BicycleGradY);// Convert sobel filter image to 8-bit for Y derivatives.

	addWeighted(ABS_BicycleGradX, 0.5, ABS_BicycleGradY, 0.5, 0, BicycleGradient);// Combine X & Y derivatives calculations into 1 Mat.

	imshow(win_BicycleSobel1, BicycleGradient);// Show the output image with sobel filter applied. 
	
	//======================================================================================================================

	Sobel(BicycleOutputImage, BicycleGradX, ddepth, 1, 0, kSize_Sobel2, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for X derivative.
	Sobel(BicycleOutputImage, BicycleGradY, ddepth, 0, 1, kSize_Sobel2, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for Y derivative.

	convertScaleAbs(BicycleGradX, ABS_BicycleGradX);// Convert sobel filter image to 8-bit for X derivatives.
	convertScaleAbs(BicycleGradY, ABS_BicycleGradY);// Convert sobel filter image to 8-bit for Y derivatives.

	addWeighted(ABS_BicycleGradX, 0.5, ABS_BicycleGradY, 0.5, 0, BicycleGradient);// Combine X & Y derivatives calculations into 1 Mat.

	imshow(win_BicycleSobel2, BicycleGradient);// Show the output image with sobel filter applied. 

	//======================================================================================================================

	Sobel(BicycleOutputImage, BicycleGradX, ddepth, 1, 0, kSize_Sobel3, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for X derivative.
	Sobel(BicycleOutputImage, BicycleGradY, ddepth, 0, 1, kSize_Sobel3, scale, delta, BORDER_DEFAULT);// Calculate sobel filter for Y derivative.

	convertScaleAbs(BicycleGradX, ABS_BicycleGradX);// Convert sobel filter image to 8-bit for X derivatives.
	convertScaleAbs(BicycleGradY, ABS_BicycleGradY);// Convert sobel filter image to 8-bit for Y derivatives.

	addWeighted(ABS_BicycleGradX, 0.5, ABS_BicycleGradY, 0.5, 0, BicycleGradient);// Combine X & Y derivatives calculations into 1 Mat.

	imshow(win_BicycleSobel3, BicycleGradient);// Show the output image with sobel filter applied. 



	waitKey();
	return 0;
}

int gaussFilter() {	
	Size kSize_Gauss1 = Size(45, 45);
	Size kSize_Gauss2 = Size(15, 15);
	Size kSize_Gauss3 = Size(75, 75);
	
	Mat gaussBicycleOutput;

	// Stop prog if image doesn't load.
	if (BicycleInputImage.empty()) {
		cout << "Couldn't load image." << endl;
		return -1;
	}

	/*
	The following 3 sections of code apply a Box Filter with 3 different kernel sizes.
	The first section uses a window size of 45x45, the second 15x15, and the third 75x75.
	*/

	GaussianBlur(BicycleInputImage, gaussBicycleOutput, kSize_Gauss1, 0, 0, BORDER_DEFAULT);// Apply gaussian blur to image. 

	imshow(win_BicycleGauss1, gaussBicycleOutput);// Show original bicycle image.

	//======================================================================================================================

	GaussianBlur(BicycleInputImage, gaussBicycleOutput, kSize_Gauss2, 0, 0, BORDER_DEFAULT);// Apply gaussian blur to image. 

	imshow(win_BicycleGauss2, gaussBicycleOutput);// Show original bicycle image.

	//======================================================================================================================

	GaussianBlur(BicycleInputImage, gaussBicycleOutput, kSize_Gauss3, 0, 0, BORDER_DEFAULT);// Apply gaussian blur to image. 

	imshow(win_BicycleGauss3, gaussBicycleOutput);// Show original bicycle image.


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