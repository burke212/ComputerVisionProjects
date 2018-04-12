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


Mat applyGuassianBlur(Mat source, Size kern) {
	Mat temp;

	GaussianBlur(source, temp, kern, 0, 0, BORDER_DEFAULT);// Apply gaussian blur to image. 
	//imshow("Gaussian", dest);
	//waitKey(0);
	return temp;
}

void calculateOctave1(Mat source) {
	Mat dest=source;
	Mat temp;
	Size kern = Size(3, 3);

	// 4 Octaves
	for (int i = 1; i < 5; i++) {
		String octave = "Octave";
		octave = octave + to_string(i);// make string for each Octave, i.e. "Octave 1", "Octave 2"...
		
		// 5 layers/ images per octave
		for (int j = 1; j < 6; j++) {
			String layer = "Layer";
			layer = octave + "_"+ layer + to_string(j);// make string for each Octave, i.e. "layer 1", "layer 2"...

			temp = applyGuassianBlur(dest, kern);
			//GaussianBlur(dest, dest, kern, 0, 0, BORDER_DEFAULT);
			//cout << "Kernel: " << kern << endl;
			//namedWindow(layer, WINDOW_NORMAL);// Keeps window sizes the same as the image. Stretches image size to user controlled size.
			
			//imshow(layer, dest);
			imshow(layer, temp);
			imwrite("Octaves/" + layer + ".png", temp);

			kern = kern + Size(2,2);// Increase the kernel size to 3 5 7 9 11
		}

		kern = Size(3, 3);// Reset kernel size @ beginning of each octave.

		resize(dest, dest, Size(dest.cols*0.5, dest.rows*0.5), 0.5, 0.5); // Resize the image | half the image size. 
	}

	waitKey(0);
}


int calulateDoG() {
	String DOG;
	
	Mat DoG;

	for (int i = 1; i < 5; i++) {
		for (int j = 1; j < 5; j++) {
			String octave = "Octave" + to_string(i);// make string for each Octave, i.e. "Octave 1", "Octave 2"...
			String layer = octave + "_" + "Layer";// +to_string(j);// make string for each Octave, i.e. "layer 1", "layer 2"...

			Mat img1 = imread("Octaves/" + layer + to_string(j) + ".png", CV_LOAD_IMAGE_COLOR);// i.e. "Octave"i_"Layer"j
			Mat img2 = imread("Octaves/" + layer + to_string(j + 1) + ".png", CV_LOAD_IMAGE_COLOR);// i.e. "Octave"i_"Layer"j+1
			
			// Validate image isn't empty
			if (img1.empty()) {
				cout << "Can't load img1";
				waitKey(0);
				return -1;
			}

			// Validate image isn't empty
			if (img2.empty()) {
				cout << "Can't load img2";
				waitKey(0);
				return -1;
			}

			DoG = img1 - img2;
			
			DOG = "DoG/DOG" + to_string(i) + "_" + layer + to_string(j) + ".png";// "DoG"i"_Octave"i"_Layer"j
			
			//imshow(layer, DoG); // Uncomment to view DoG output.
			imwrite(DOG, DoG);
		}
	}
	waitKey(0);
	return 0;
}

string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}
void findMinMax() {
	String DOG; // Text to hold DoG picture name
	double minVal; double maxVal; Point minLoc; Point maxLoc; // Values for Max/ Min
	int count = 1;//TEST
	vector<Point> max_points;// Hold max val locations
	vector<Point> min_points;// Hold min val locations

	for (int i = 1; i < 5; i++) {// Iterate thru Octaves
		for (int j = 1; j < 5; j++) {// Iterate thru (DoG) layers in Octaves

			String octave = "Octave" + to_string(i);// make string for each Octave, i.e. "Octave 1", "Octave 2"...
			String layer = octave + "_" + "Layer";// +to_string(j);// make string for each Octave, i.e. "layer 1", "layer 2"...

			DOG = "DoG/DOG" + to_string(i) + "_" + layer + to_string(j) + ".png";// "DoG"i"_Octave"i"_Layer"j

			Mat img1 = imread(DOG, CV_LOAD_IMAGE_COLOR);// i.e. "Octave"i_"Layer"j
			Mat max_img1 = Mat::zeros(img1.size(), CV_64FC1);
			Mat min_img1 = Mat::zeros(img1.size(), CV_64FC1);
			
			cvtColor(img1, img1, COLOR_RGB2GRAY);// Convert img from RGB -> grayscale

			// Validate image loaded
			if (img1.empty()) {
				cout << "Can't load img1";
				waitKey(0);
				//	return -1;
			}

			minMaxLoc(img1, &minVal, &maxVal, &minLoc, &maxLoc);// Find max & min value in image

			//TEST
			cout << "iteration: " << count << endl
				<< "	minval: " << minVal << endl
				<< "	maxval: " << maxVal << endl
				<< "	minLoc: " << minLoc << endl
				<< "	maxLoc: " << maxLoc << endl;
			count++;
		}
	}
	waitKey(0);
}






int main(int argc, char argv[])
{
	// Show the original image.
	//showOriginalImg();
	
	// Apply 3x3 gauss on original image
	//applyGuassianBlur(original_img, kern3x3);

	calculateOctave1(original_img);

	calulateDoG();

	findMinMax();

	return 0;
}