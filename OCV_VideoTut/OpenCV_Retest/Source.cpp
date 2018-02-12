// OpenCVTest.cpp : Defines the entry point for the console application.
//Opens/ plays a video.
#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

/// Global Variables
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;
String window_name = "My First Video";
VideoCapture vCap_Original("barriers.avi");
Mat frame;
Mat newFrame;



void on_trackbar()
{
	alpha = (double)(alpha_slider + alpha_slider) / alpha_slider_max;
	beta = (1.0 - alpha);
	
	frame.convertTo(newFrame, -1, alpha, beta);
}


int main(int argc, char argv[])
{
	// Check for video.
	if (vCap_Original.isOpened() == false) {
		cout << "Cannot open the video file" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	// Initialize values
	alpha_slider = 50;

	//get the frames rate of the video
	double fps = vCap_Original.get(CAP_PROP_FPS);
	cout << "Frames per seconds : " << fps << endl;

	namedWindow(window_name, WINDOW_AUTOSIZE); //create a window

	// Create Trackbars
	char TrackbarName[50];
	sprintf_s(TrackbarName, "Alpha x %d", alpha_slider_max);

	createTrackbar(TrackbarName, window_name, &alpha_slider, alpha_slider_max);

	while (true)
	{
		bool bSuccess = vCap_Original.read(frame); // read a new frame from video 

		// Adjust the brightness of the video using trackbar.
		on_trackbar();

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			cout << "Found the end of the video" << endl;
			vCap_Original.set(CAP_PROP_POS_AVI_RATIO, 0);// Loops video 
			continue;
			//break;
		}

		//show the frame in the created window
		imshow(window_name, frame);
		imshow("new frame", newFrame);
		
		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}

	return 0;
}