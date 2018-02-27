// OpenCVTest.cpp : Defines the entry point for the console application.
//Opens/ plays a video.
#include <opencv2/opencv.hpp>
#include <stdint.h>

using namespace std;
using namespace cv;

/// Global Variables
const int alpha_slider_max = 100;
const int beta_slider_max = 100;
int alpha_slider;
int beta_slider;
double alpha;
double beta;
bool altAccepted = false;
String window_name = "My First Video";
String other_window_name = "New Frame";
VideoCapture vCap_Original("barriers.avi");
Mat frame;
Mat newFrame;
Mat alteredFrame;


// Adjust alpha & beta values (brightness & contrast)
void on_trackbar()
{
	alpha = (double)(alpha_slider + alpha_slider) / alpha_slider_max;
	//beta = (1.0 - alpha);
	beta = (beta_slider - alpha);

	if (altAccepted == false) {// While still editing brightness/ contrast.
		frame.convertTo(newFrame, -1, alpha, beta);
	}
	else {// Once user has accepted the changed. 
		frame.convertTo(frame, -1, alpha, beta);
	}
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
	beta_slider = 0;

	//get the frames rate of the video
	double fps = vCap_Original.get(CAP_PROP_FPS);
	cout << "Frames per seconds : " << fps << endl;

	namedWindow(window_name, WINDOW_AUTOSIZE); //create a window

	// Create Trackbars
	char TrackbarName[50];
	char otherTrackbar[50];
	sprintf_s(TrackbarName, "Alpha == %d", alpha_slider_max);
	sprintf_s(otherTrackbar, "Beta == %d", beta_slider_max);

	createTrackbar(TrackbarName, window_name, &alpha_slider, alpha_slider_max);
	createTrackbar(otherTrackbar, window_name, &beta_slider, beta_slider_max);

	while (true)
	{
		bool bSuccess = vCap_Original.read(frame); // read a new frame from video 

		// Loop the video when last frame is found.
		if (bSuccess == false)
		{
			cout << "Found the end of the video" << endl;
			vCap_Original.set(CAP_PROP_POS_AVI_RATIO, 0);// Loops video 
			continue;
		}

		// If user hasn't accepted the changes...
		if (altAccepted == false) 
		{
			// Adjust the brightness of the video using trackbar.
			on_trackbar();

			//show the frame in the created window
			imshow(window_name, frame);
			imshow(other_window_name, newFrame);
		}

		// If user has accepted the changes...
		if (altAccepted == true) 
		{
			// Apply adjustments to video based on changes.
			on_trackbar();

			destroyWindow(other_window_name);
			
			imshow(window_name, frame);
		}

		// Check if 'Enter' is pressed.
		if (waitKey(1) == 13) {
			altAccepted = true;
			cout << "Change to original accepted.";
			destroyWindow(window_name);
		}


		//wait for for 1 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		if (waitKey(1) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}

	return 0;
}