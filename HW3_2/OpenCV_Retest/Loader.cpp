#include "Loader.h"

using namespace cv;
using namespace std;

Loader::Loader(void)
{
}

Loader::~Loader(void)
{
}

bool Loader::loadControlParameters(ControlParameters& cp)
{
	FileStorage fsp("stereo.xml", FileStorage::READ);
	if( !fsp.isOpened() )
	{
		cout << "Cannot open stereo.xml" << endl;
		waitKey();
		return false;
	}

	fsp["seg_img_filename"] >> cp.seg_img_filename;
	fsp["seg_out_filename"] >> cp.seg_out_filename;
	fsp["segment_method"] >> cp.segment_method;
	fsp["segment_window"] >> cp.segment_window;

	return true;
}


Mat Loader::loadOriginalImage(const ControlParameters& cp)
{
	Mat output_img = imread(cp.seg_img_filename, CV_LOAD_IMAGE_COLOR);
	if(output_img.empty())
		cout << "Failed to read fog_input image" << endl;
	return output_img;
}

