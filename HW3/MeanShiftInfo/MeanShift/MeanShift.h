#pragma once
#include <opencv2/opencv.hpp>

#include <math.h>
#include <vector>
#include <iostream>

#include "defs.h"

static int MERGE = 2;
static double CLUSTER = 150.f;

class MeanShift
{
public:
	MeanShift(void);
	~MeanShift(void);

public:
	cv::Mat segmentUsingRGB( const cv::Mat f_img, const unsigned f_winSize_ui );
	cv::Mat segmentUsingLAB( const cv::Mat f_img, const unsigned f_winSize_ui );
	cv::Mat segmentUsingHLS( const cv::Mat f_img, const unsigned f_winSize_ui );

	cv::Mat resetColor2(const cv::Mat seg_img, const cv::Mat Original_img);



protected:
	cv::Mat standardization(const cv::Mat& f_img);
	cv::Mat segmentFromFeatureSpace( const cv::Mat& f_img,
		const unsigned f_winSize_ui,
		const vector<SWin> &f_partition_v,
		const vector<vector<cv::Point3i> > &f_classes_v );
	cv::Mat getFeatureSpace( const cv::Mat f_img, SImgDims &f_imgDims );
	cv::Mat smoothFeatureSpace( const cv::Mat f_space, const SImgDims &f_imgDims); // smooth the feature space before mean-shift, using Gauss kernal

	cv::Mat resetColor(const cv::Mat seg_img, const cv::Mat L_img); // using average color to label a segment
	

	// Get the range of each of the channels of the input image
	// 1. Split the color image
	// 2. Get the range of each of the channels
	void getChannelsRanges( const cv::Mat f_img, int f_ranges [],
		SImgDims &f_imgDims );

	vector<SWin> generatePartition( const SImgDims &f_imgDims,
		int &f_activeWindows_i, 
		const unsigned size_u = 1 );

	void iterate( vector<SWin> &f_partition_v, const cv::Mat f_featureSpace,
		const SImgDims &f_imgDims, const unsigned f_winSize_ui,
		int &f_activeWindows_i     );

	void updateMean( SWin &f_win, const cv::Mat f_featureSpace, const SImgDims &f_imgDims,
		const unsigned f_winSize_ui,
		int &f_activeWindows_i );

	void updateType( vector<SWin> &partition_v, int &f_activeWindows_i );

	inline void mergeWindows( const  SWin &f_win2Merge, SWin &f_winTarget );

	void createClasses( const vector<SWin> &f_partition_v, 
		vector<vector<cv::Point3i> > &f_classes_v );

	uchar findLabel( const cv::Point3i f_anchor,
		const vector<vector<cv::Point3i> > &f_classes_v );

	cv::Point3i findAnchorPoint( const cv::Point3i f_point, 
		const vector<SWin> & f_partition_v,
		const unsigned f_winSize_ui );

	void countNonDiscardedWindows( const vector<SWin> & f_partition_v );

	void clusterFinalMeans( vector<SWin> &f_partition_v );

	inline  float norm( const cv::Point3i f_point1, const cv::Point3i f_point2 );

	int myRound(float num);

	void showImage( cv::Mat f_img, const std::string &f_name_s );

	cv::Mat makeIt8bit( cv::Mat f_img );

	inline float E_distance(int x1, int y1, int z1, int x2, int y2, int z2)//Euclidean distance
	{
		return sqrt((float)((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1)));
	}

	inline float Gaussian(float x, float sigma)
	{
		return (float)exp(-(x*x)/(sigma*sigma));
	}
};
