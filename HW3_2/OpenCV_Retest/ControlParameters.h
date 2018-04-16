#pragma once

#include <opencv2/opencv.hpp>
#include <string>


class ControlParameters
{
public:
	std::string seg_img_filename;
	std::string seg_out_filename;
	int segment_method;
	int segment_window;
};