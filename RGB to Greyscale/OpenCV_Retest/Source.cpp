// OpenCVTest.cpp : Defines the entry point for the console application.
//

/*
	Program displays & converts colored image --> greyscale.

	\m/>.<\m/
*/
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

String OriginalImg = "Original Image";
String editedImg = "Edited Image";

Mat inputImage = imread("00_RECTIFY.tif", CV_LOAD_IMAGE_COLOR);// Load dog image.
Mat outputImage;


int main(int argv, char** argc)
{
	if (inputImage.empty()) {
		waitKey(0);
		cout << "Couldn't load image" << endl;
		
		return -1;
	}

	imshow(OriginalImg, inputImage);

	cvtColor(inputImage, outputImage, cv::COLOR_BGR2GRAY);

	imshow(editedImg, outputImage);
	
	imwrite("RECTIFY_GREY.tif", outputImage);

	waitKey(0);
	return 0;
}