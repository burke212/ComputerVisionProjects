#include <opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "ControlParameters.h"
#include "Loader.h"
#include "MeanShift.h"



using namespace cv;
using namespace std;



void main()
{
//Initialize-----------------------------------------------------------------
	cout << "Init. Mean-Shift image segmentation." << endl;
	cout << "Segment image using # features:" << endl;
	cout << "#1: R G B" << endl;
	cout << "#2: L a b" << endl;
	cout << "#3: H L S" << endl;
	ControlParameters cp;
	Loader loader;

	if(!loader.loadControlParameters(cp))
	{
		cout << "Cannot load control parameters." << endl;
		waitKey();
		return;
	}

	Mat original_image = loader.loadOriginalImage(cp);

	cout << "Load images and parameters... DONE." << endl;

//Image Segmentation-----------------------------------------------------------------
	MeanShift ms;
	Mat segment_temp;
	Mat segment_out;

	Mat float_image;
	original_image.convertTo(float_image, CV_32FC3);

	switch(cp.segment_method)
	{
	case 1:
		segment_out = ms.segmentUsingRGB(float_image,cp.segment_window);
		break;

	case 2:
		segment_out = ms.segmentUsingLAB(float_image,cp.segment_window);
		break;

	case 3:
		segment_out = ms.segmentUsingHLS(float_image,cp.segment_window);
		break;
		break;

	default:
		segment_out = ms.segmentUsingRGB(float_image,cp.segment_window);
	}

	Mat seg_color = ms.resetColor2(segment_out, original_image);

	imwrite(cp.seg_out_filename, seg_color);
	namedWindow("Segmentation", WINDOW_AUTOSIZE);
	imshow("Segmentation", seg_color);
	waitKey();

	cout << "Image segmentation... DONE." << endl;

}