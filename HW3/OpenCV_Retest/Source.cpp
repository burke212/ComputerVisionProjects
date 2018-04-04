// OpenCVTest.cpp : Defines the entry point for the console application.
//Opens/ plays a video.
#include <opencv2/opencv.hpp>
//#include <stdint.h>

using namespace std;
using namespace cv;

int main(int argv, char** argc)
{
	// Load the image
	Mat src = imread("coloredImage1.tif", CV_LOAD_IMAGE_COLOR);
	//Mat src = imread("coloredImage2.png", CV_LOAD_IMAGE_COLOR);
	//Mat src = imread("blackWhite1.tif", CV_LOAD_IMAGE_COLOR);
	//Mat src = imread("blackWhite2.png", CV_LOAD_IMAGE_COLOR);
	

	// Check if everything was fine
	if (src.empty()) {
		cout << "Couldn't load image" << endl;
		return -1;
	}
	
	// Show source image
	imshow("Source Image", src);

	// Create binary image from source image
	Mat bw;
	cvtColor(src, bw, CV_BGR2GRAY);
	//threshold(bw, bw, 40, 255, CV_THRESH_OTSU);// CV_THRESH_BINARY | CV_THRESH_OTSU
	threshold(bw, bw, 157, 225, CV_THRESH_OTSU);// CV_THRESH_BINARY | CV_THRESH_OTSU
	imshow("Binary Image small", bw);
	
	// Perform the distance transform algorithm
	Mat dist;
	distanceTransform(bw, dist, CV_DIST_L2, 3);// DIST_LABEL_PIXEL	CV_DIST_L2
	// Normalize the distance image for range = {0.0, 1.0}
	// so we can visualize and threshold it
	normalize(dist, dist, 0, 157, NORM_MINMAX); //<<=========================================================================
	imshow("Distance Transform Image", dist);

	// Threshold to obtain the peaks
	// This will be the markers for the foreground objects
	threshold(dist, dist, .4, 1., CV_THRESH_BINARY);
	// Dilate a bit the dist image
	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	dilate(dist, dist, kernel1);
	imshow("Peaks", dist);

	// Create the CV_8U version of the distance image
	// It is needed for findContours()
	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);
	// Find total markers
	vector<vector<Point> > contours;
	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); 
	// Create the marker image for the watershed algorithm
	Mat markers = Mat::zeros(dist.size(), CV_32SC1);
	// Draw the foreground markers
	for (size_t i = 0; i < contours.size(); i++)
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1);
	// Draw the background marker
	circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);
	imshow("Markers", markers * 10000);

	// Perform the watershed algorithm
	//===============================================================================================
	watershed(src, markers);
	//imshow("Markers_post_watershed", markers);
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	//imshow("Markers_post_convertTo", markers);
	//imshow("Mark_post_convertTo", mark);
	bitwise_not(mark, mark);
	imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
	// image looks like at that point
	// Generate random colors
	//===============================================================================================
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	// Create the result image
	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= static_cast<int>(contours.size()))
				dst.at<Vec3b>(i, j) = colors[index - 1];
			else
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
		}
	}
	// Visualize the final image
	imshow("Final Result", dst);
	

	waitKey(0);
	return 0;
}