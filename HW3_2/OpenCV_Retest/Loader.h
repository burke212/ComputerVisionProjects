#pragma once

#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>

#include "ControlParameters.h"

class Loader
{
public:
	Loader(void);
	~Loader(void);

public:
	bool loadControlParameters(ControlParameters& cp);
	cv::Mat loadOriginalImage(const ControlParameters& cp);
};
