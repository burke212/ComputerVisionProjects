#include "MeanShift.h"

MeanShift::MeanShift(void)
{
}

MeanShift::~MeanShift(void)
{
}

using namespace cv;
using namespace std;



Mat MeanShift::segmentUsingLAB( const Mat f_img, const unsigned f_winSize_ui )
{
	Mat imgBGR, imgLAB, imgSeg;
	Mat LAB[3];
	Mat seg[3];

	f_img.convertTo(imgBGR, CV_32FC3);
	imgBGR *= 1./255;
	cvtColor(imgBGR, imgLAB, CV_BGR2Lab);

	split(imgLAB, LAB);

	normalize(LAB[0], seg[0], 0, 255, NORM_MINMAX);
	normalize(LAB[1], seg[1], 0, 255, NORM_MINMAX);
	normalize(LAB[2], seg[2], 0, 255, NORM_MINMAX);

	merge(seg, 3, imgSeg); 

	cout << "Convert into LAB" << endl;
	return segmentUsingRGB(imgSeg, f_winSize_ui);
}

Mat MeanShift::segmentUsingHLS( const Mat f_img, const unsigned f_winSize_ui )
{
	Mat imgBGR, imgHLS, imgSeg;
	Mat HLS[3];
	Mat seg[3];

	f_img.convertTo(imgBGR, CV_32FC3);
	imgBGR *= 1./255;
	cvtColor(imgBGR, imgHLS, CV_BGR2HLS);

	split(imgHLS, HLS);

	normalize(HLS[0], seg[0], 0, 255, NORM_MINMAX);
	normalize(HLS[1], seg[1], 0, 255, NORM_MINMAX);
	normalize(HLS[2], seg[2], 0, 255, NORM_MINMAX);

	merge(seg, 3, imgSeg); 

	cout << "Convert into LAB" << endl;
	return segmentUsingRGB(imgSeg, f_winSize_ui);
}

Mat MeanShift::segmentUsingRGB( const Mat f_img, const unsigned f_winSize_ui ) // f_img: CV_32FC3, [0,255]
{
	// Set aside some space to keep the dimensions of the scale space
	SImgDims imgDims;

	// Get the feature space
	Mat featureSpace = getFeatureSpace( f_img, imgDims );
	//Mat featureSpace0 = getFeatureSpace( f_img, imgDims );
	//Mat featureSpace = smoothFeatureSpace(featureSpace0, imgDims);

	// Print some data about the dimensions of the three dim feature space
	imgDims.print();

	// The active pixels in the feature space image are those with positive value
	cout << "Active pixels = " << countNonZero( featureSpace) << endl;

	// Partition the feature space into windows of size f_winSize_ui
	int activeWindows_i = 0;
	vector<SWin> partition_v = generatePartition( imgDims, activeWindows_i, f_winSize_ui );
	cout << "Total win = " <<  partition_v.size() << endl;

	// Set the number of pix from the anchor point to the edge of the window
	const unsigned sizeFromAnchor_ui = int( f_winSize_ui / 2 );

	// Iterate until there are no active windows.
	int counter_i = 0;
	while( activeWindows_i > 0 )
	{
		counter_i++;
		cout << "Iteration = " << counter_i << "..." << flush;
		iterate( partition_v, featureSpace, imgDims,
			sizeFromAnchor_ui, activeWindows_i );
	}
	cout << "\n";
	countNonDiscardedWindows( partition_v );

	clusterFinalMeans(partition_v);
	cout << "AFTER CLUSTER" << endl;
	countNonDiscardedWindows( partition_v );

	// Create the equivalence classes
	vector< vector<Point3i> > classes_v;
	createClasses( partition_v, 
		classes_v );

	cout << "Labels = " << classes_v.size() << endl;

	// Generate the segmented image. In here we do not work anymor
	// with the feature space, only with the original image and
	// the label/classes that we already have generated.
	Mat out = segmentFromFeatureSpace( f_img,
		f_winSize_ui,
		partition_v,
		classes_v     );

	//cout << out << endl;
	//cout << "-----------------------class size----- " << classes_v.size() << endl;

	Mat outN;
	// The normalization is required for visualization, as there are just
	// a few labels (i.e intensity values) in the image
	normalize( out, outN, 0, 255, NORM_MINMAX );
	//showImage( outN, "Scaled and segmented" );
	////show2Images( out, outN, "Segmented Image" );
	//imwrite("Seg.jpg", outN);
	return outN;

	//Mat imgBGR = f_img * (1./255);
	//Mat imgGray, L;
	//cvtColor(imgBGR, imgGray, CV_BGR2GRAY);
	//normalize(imgGray, imgGray, 0, 255, NORM_MINMAX );
	//imgGray.convertTo(L, CV_8UC1);

	//Mat D = resetColor(out, L);

	//return D;
}

Mat MeanShift::segmentFromFeatureSpace( const Mat& f_img,
									   const unsigned f_winSize_ui,
									   const vector<SWin> &f_partition_v,
									   const vector<vector<Point3i> > &f_classes_v )
{
	// Split the 3 channel input image into three one channel images
	Mat buff[ 3 ];
	split( f_img, buff );

	cout << "Segmenting image  ";

	// Allocate space for the output image
	Mat out( f_img.rows, f_img.cols, CV_8UC1 );

	for( unsigned y = 0; y < (unsigned)f_img.rows; ++y )
	{
		// Just to make sure that this is still working
		if( y % 10 == 0 )
		{
			cout << "." << flush;
		}

		for( unsigned x = 0; x < (unsigned)f_img.cols; ++x )
		{
			// Get the coordinate of the curr pix in the feature space
			Point3i point( (unsigned)buff[0].ptr<float>( y )[ x ], 
				(unsigned)buff[1].ptr<float>( y )[ x ],
				(unsigned)buff[2].ptr<float>( y )[ x ] );

			// Find to which window of the partition belongs the (blue,green,red) point
			// We iterate through all the anchor points and find its position
			Point3i anchor = findAnchorPoint( point, f_partition_v,
				f_winSize_ui );

			//cout << point << "-->" << anchor << endl;

			// The anchor point defines the final label
			const uchar finalLabel_f = findLabel( anchor, f_classes_v );

			out.ptr<uchar>( y )[ x ] = finalLabel_f;

		} // end for x
	} // end for y

	cout << "done :) \n";

	return out;
}


// Get the range of each of the channels of the input image
// 1. Split the color image
// 2. Get the range of each of the channels
void MeanShift::getChannelsRanges( const Mat f_img, int f_ranges [],
								  SImgDims &f_imgDims )
{
	// Show input image?
	//showImage( f_img );

	// Number of channels in the image
	const unsigned channels_ui = f_img.channels();

	// Allocate space for the one channel images
	Mat images[ 3 ];

	// Get the 1 channel images
	split( f_img, images );

	for( unsigned i = 0; i < channels_ui; ++i )
	{
		double min_d = 99999999;
		double max_d = -99999999;

		showImage( images[ i ], "1-channel image" );

		// Get the min and max values in each channel
		minMaxLoc( images[i], &min_d, &max_d );

		// The size of each dimension is determined by the size
		// of the corresponding channel
		const unsigned size_ui = (unsigned)max_d + 1; 

		// Output the range of the i channel
		//cout << "Channel = " << i << ": Min = " << min_d << "  Max = " << max_d 
		// << "  Size = " << size_ui << endl; 

		f_ranges[ i ] = size_ui;

		// Put the size of the images in here
		if( i == 0 )
		{
			f_imgDims.blue = size_ui;
		}
		else if( i == 1 )
		{
			f_imgDims.green = size_ui;
		}
		else if( i == 2 )
		{
			f_imgDims.red = size_ui;
		}
	}
}


// Get a three dimensional histogram, one dimension for each of the channels in the input image
Mat MeanShift::getFeatureSpace( const Mat f_img, SImgDims &f_imgDims )
{
	const unsigned dims_ui = f_img.channels();

	// Allocate space for the range (starting from zero)  of each of the channels.
	int ranges[ 3 ];

	// Get the range of each of the channels in the image
	getChannelsRanges( f_img, ranges, f_imgDims );

	// Initialize to 0.f the  output image
	// It is a three dimensional image, each dimension
	// would have the size of the B, G and R channels of the input
	// image.
	Mat hist( dims_ui, ranges, CV_32FC1, Scalar( 0.0 ) );

	// Gets the 3D histogram
	for( unsigned y = 0; y < (unsigned)f_img.rows; ++y )
	{
		// Access the y row of the input image
		const float* img_p = f_img.ptr<float>( y );

		for( unsigned x = 0; x < (unsigned)f_img.cols; ++x )
		{
			// x coordinate for the color image
			const unsigned xCoordColor_ui = dims_ui * x;

			// NOTE: The default OpenCV color space is BGR
			const unsigned blue_ui  = unsigned( img_p[ xCoordColor_ui ]     );
			const unsigned green_ui = unsigned( img_p[ xCoordColor_ui + 1 ] );
			const unsigned red_ui   = unsigned( img_p[ xCoordColor_ui + 2 ] );

			// Need to know how to access this with pointers. No time now
			hist.at<float>( blue_ui, green_ui, red_ui )++;

		} // end for x
	} // end for y

	return hist;
}

uchar MeanShift::findLabel( const Point3i f_anchor,
						   const vector<vector<Point3i> > &f_classes_v )
{
	// Return -1 if there is no label for that anchor point.
	// THAT SHOULD NEVER HAPPENS, If IT DOES THERE IS SOMETHING
	// REALLY REALLY WRONG WITH THE CODE

	for( unsigned i = 0; i < f_classes_v.size(); ++i )
	{
		for( unsigned j = 0; j < f_classes_v[ i ].size(); ++j )
		{
			if( f_anchor == f_classes_v[ i ][ j ] )
			{
				return uchar( i );
			}

		}  // end for j
	} //end for i

	//cout << "Wrong!" << endl;
	return f_classes_v.size();
}


Point3i MeanShift::findAnchorPoint( const Point3i f_point, 
									   const vector<SWin> &f_partition_v,
									   const unsigned f_winSize_ui )
{
	// In here we compute the number of pix from the anchor 
	// that defined the window. 
	const int anchorY_i = int( f_winSize_ui / 2 ); 
	const int anchorX_i = int( f_winSize_ui / 2 ); 
	const int anchorZ_i = int( f_winSize_ui / 2 ); 

	// ALlocate space for the anchor point
	Point3i out;

	for( unsigned i = 0; i < f_partition_v.size(); ++i )
	{
		// Current anchor point
		const Point3i buffer = f_partition_v[i].position[0];

		if( f_point.y >= buffer.y - anchorY_i &&
			f_point.y <= buffer.y + anchorY_i    )
		{  
			if( f_point.x >= buffer.x - anchorX_i &&
				f_point.x <= buffer.x + anchorX_i     )
			{
				if( f_point.z >= buffer.z - anchorZ_i &&
					f_point.z <= buffer.z + anchorZ_i     )
				{
					out = buffer;
				} // end if z coord
			} // end if x coord
		} // end if y coord

	} //end for i

	return out;

}


vector<SWin> MeanShift::generatePartition( const SImgDims &f_imgDims,
										  int &f_activeWindows_i,
										  const unsigned size_ui  )
{
	// This is going to be returned. It is a vector that contains
	// SWin objects, that contained all the information about a
	// window, its initila position, its type and its position
	// throug the iterations. Note that now it is only set the
	// initial position (the coordinates of the anchor point).
	// All the other properties are set to be ready for the
	// iterative process.
	vector<SWin> partition_v;

	// In here we compute the number of pixles from the achor point to the edge of the
	// window (note that it is a cube).
	const unsigned anchorY_ui = int( size_ui / 2 ); 
	const unsigned anchorX_ui = int( size_ui / 2 );
	const unsigned anchorZ_ui = int( size_ui / 2 );

	cout << "Pix to win edge from anchor = " << anchorX_ui << endl;

	// Define the anchor points and the initial mean of the windows in the partition
	// Note that a window is defined by its anchor point (the center of the window).
	// It is not important if the whole window would be in the feature space, as only
	// the elements within the features space would be considered later
	for( unsigned b = 0; b < f_imgDims.blue; ++b )
	{
		// Skip non anchor points
		if( b % size_ui != anchorY_ui )
		{
			continue;
		}

		for( unsigned g = 0; g < f_imgDims.green; ++g )
		{
			// Skip non anchor points
			if( g % size_ui != anchorX_ui )
			{
				continue;
			}

			for( unsigned r = 0; r < f_imgDims.red; ++r )
			{
				// Skip non anchor points
				if( r % size_ui != anchorZ_ui )
				{
					continue;
				}

				// Allocate temporary space for the window.
				// This will be pushed back into the final vector
				SWin buffer;
				// Allocate space for the anchor point of the window
				Point3i buffPoint;

				// Set the coordinates of the init anchor point and the current
				// anchor point. It is the same as we are initilizing
				buffPoint.x = b;
				buffPoint.y = g;
				buffPoint.z = r;

				// Set the anchor point as the first position of the window
				buffer.position.push_back( buffPoint );

				// Set the window ready for the next iteration
				buffer.active_b = true;

				// At the begining, set all the windows as LEADERS (i.e. all
				// of them, potentially, would define a label by itself).
				buffer.type_e = LEADER_S;

				// Set as non clustered. (i.e. all
				// of them, potentially, would define a label by itself)
				buffer.cluster_b = false;

				// Insert the new window into the vector 
				partition_v.push_back( buffer );

				// Increase the counter of active windows
				f_activeWindows_i++;

			} // end for r

		} // end for g

	} // end for b

	return partition_v;
}



void MeanShift::iterate( vector<SWin> &f_partition_v, const Mat f_featureSpace,
						const SImgDims &f_imgDims, const unsigned f_winSize_ui,
						int &f_activeWindows_i  )
{

	// Iterate trough all the active windows to update its mean.
	// The update is necesary as each iteration the windows change
	// its position.
	for( unsigned int w = 0; w < f_partition_v.size(); ++w )
	{
		// Compute the mean of the values in the window. Do it only for
		// active windows
		if( f_partition_v[w].active_b )
		{
			updateMean( f_partition_v[w], f_featureSpace, f_imgDims,
				f_winSize_ui, f_activeWindows_i  );
		}
		//cout << "." << flush;
	} // end for w

	// Identify leaders and followers. Note that GRAY windows, i.e.
	// the ones that all of its elements have the value zero in the
	// scale space are identified in the updateMean process.
	updateType( f_partition_v, f_activeWindows_i );

}


void MeanShift::updateMean( SWin &f_win, const Mat f_featureSpace, const SImgDims &f_imgDims,
						   const unsigned f_winSize_ui, int &f_activeWindows_i )
{
	// Update the mean of the window defined by the last
	// element in the position vector
	const unsigned iter_ui = f_win.position.size() - 1;


	unsigned blueAdd_ui  = 0;  
	unsigned greenAdd_ui = 0;
	unsigned redAdd_ui   = 0;
	unsigned counter_ui  = 0;

	for( int b  = int( f_win.position[iter_ui].x - f_winSize_ui ); 
		b <= int( f_win.position[iter_ui].x + f_winSize_ui );  ++b )

	{
		// Check that we are inside the image
		if( b < 0 || b >= (int)f_imgDims.blue )
		{ 
			continue;
		}

		for( int g  = int( f_win.position[iter_ui].y - f_winSize_ui ); 
			g <= int( f_win.position[iter_ui].y + f_winSize_ui );  ++g )
		{
			// Check that we are inside the image
			if( g < 0 || g >= (int)f_imgDims.green )
			{ 
				continue;
			}

			for( int r  = int( f_win.position[iter_ui].z - f_winSize_ui ); 
				r <= int( f_win.position[iter_ui].z + f_winSize_ui );  ++r )
			{
				// Check that we are inside the image
				if( r < 0 || r >= (int)f_imgDims.red )
				{ 
					continue;
				}


				// Add as many times b,r and sd.dev g as indicated by the
				// value in the feature space

				const unsigned maxOccur_ui = (unsigned)f_featureSpace.at<float>( b, g, r );
				//cout << maxOccur_ui << endl;

				blueAdd_ui  += maxOccur_ui * (unsigned)b;
				greenAdd_ui += maxOccur_ui * (unsigned)g;
				redAdd_ui   += maxOccur_ui * (unsigned)r;
				counter_ui  += maxOccur_ui;

			} // end for r
		} // end for g
	} // end for b

	//cout << "counter_ui = " << counter_ui << endl;;

	// Set as inactive, and as GRAY the windows that all its occurencies are 0;
	// They won't be considered as cadidates to generate a label.
	if( counter_ui == 0 )
	{
		//cout << "GRAY!!!\n";
		f_win.active_b = false;
		f_activeWindows_i--;
		f_win.type_e = GRAY_S;
		return;
	}

	// Compute the new mean, i.e. the new position in the scale space
	Point3i buffer;	   
	buffer.x = myRound( (float)blueAdd_ui  / (float)counter_ui );
	buffer.y = myRound( (float)greenAdd_ui / (float)counter_ui );
	buffer.z = myRound( (float)redAdd_ui   / (float)counter_ui );

	//cout << buffer << endl;

	// If the current mean equals to the old one, mark it as not
	// candidate for the next iteration. Note that its win type does
	// not changes
	if( buffer  ==  f_win.position[iter_ui] )
	{
		f_win.active_b = false;

		// Decrease the counter of active windows
		f_activeWindows_i--;

		//cout << buffer << endl;

		return;
	}

	// Add to the position vector the current position
	f_win.position.push_back( buffer );
}


void MeanShift::updateType( vector<SWin> &f_partition_v, int &f_activeWindows_i )
{
	//cout << "Updating windows types\n";

	// Check each window (the base widows bW) to update its sate
	for( unsigned bW = 0; bW < f_partition_v.size(); ++bW )
	{
		// Only check active windows
		if( !f_partition_v[bW].active_b )
		{
			continue;
		}

		// Define the current interation (i.e. the last position)
		const unsigned   current_ui = f_partition_v[bW].position.size() - 1;
		// Define the current position of the anchor point of the window
		const Point3i basePoint = f_partition_v[bW].position[ current_ui ];

		// Compare the current position with the position of all 
		// the other windows
		for( unsigned cW = 0; cW < f_partition_v.size(); ++cW )
		{
			// Do not compare the window with itself
			if( bW == cW )
			{
				continue;
			}

			// Compare with the whole history of locations
			for( unsigned h = 0; h < f_partition_v[cW].position.size(); ++h )
			{
				// Define the comparison position
				const Point3i compPoint = f_partition_v[cW].position[ h ];

				// If the base window in the current position (i.e. the last entry in the 
				// position vector) occupies the same position that the comp window
				//if( basePoint == compPoint  )
				//[Dongwei] If the current position of the base window is 8-connectedness with another window,
				if( abs(basePoint.x - compPoint.x) < MERGE
					&& abs(basePoint.y - compPoint.y) < MERGE
					&& abs(basePoint.z - compPoint.z) < MERGE)
				{
					// Mark the comp point as LEADER. It does not matter
					// who the leader is, but one of them has to be the leader.
					f_partition_v[cW].type_e = LEADER_S;

					// Mark the base win as non active
					f_partition_v[bW].active_b = false;
					// Decrease the counter of active windows
					f_activeWindows_i--;

					// Decide if the base win becomes a merger or a follower.
					// If the comp window and the base window have the same position 
					// in the current iteration (i.e. comparing the last entries), then 
					// the base window becomes  a merger. It really does not matters
					//  if it is a merger or a follower, is it actually the same stuff
					if( h  == current_ui )  
					{
						f_partition_v[bW].type_e   = FOLLOWER_S;
						//cout << "MERGER!!!!!!!\n";
					}
					else
					{
						f_partition_v[bW].type_e = FOLLOWER_S;
						//cout << "FOLLOWER!!!!!!\n";
					}

					// If a window becomes a follower, then it will be part of the same
					// class of the leader window. We keep track of that with this function
					mergeWindows( f_partition_v[bW], f_partition_v[cW] );

					break;
				} // end if
			} // end for h

		} // end for cW
	} // end for bw
}

inline void MeanShift::mergeWindows( const SWin &f_win2Merge, SWin &f_winTarget )
{
	// Put the initial position of the f_win2Merge in the 
	// list of merged points of the f_winTarget
	f_winTarget.merge.push_back( f_win2Merge.position[0] );

}


// This method will cluster windows that their finall position
void MeanShift::clusterFinalMeans( vector<SWin> &f_partition_v )
{
	for( unsigned i = 0; i < f_partition_v.size(); ++i )
	{
		// Only leader or single type of vectors have final means
		if( f_partition_v[ i ].type_e    == FOLLOWER_S || 
			f_partition_v[ i ].type_e    == GRAY_S     ||
			f_partition_v[ i ].cluster_b                  )
		{
			continue;
		}

		// Get the last position, where the final mean is
		const unsigned lastPos_ui = f_partition_v[ i ].position.size() - 1;

		Point3i base = f_partition_v[ i ].position[ lastPos_ui ];

		for( unsigned j = 0; j < f_partition_v.size(); ++j )
		{
			// Do not cluster with a label with itself
			if( j == i )
			{
				continue;
			}

			// Only leader or single type of vectors have final means
			if( f_partition_v[ j ].type_e    == FOLLOWER_S || 
				f_partition_v[ j ].type_e    == GRAY_S     ||
				f_partition_v[ j ].cluster_b                  )
			{
				continue;
			}

			// Get the last position
			const unsigned lastPosC_ui = f_partition_v[ j ].position.size() - 1;

			Point3i comp = f_partition_v[ j ].position[ lastPosC_ui ];

			// Put the pos 0 of the j vector and all its followers as followers of the 
			// i vector
			if( norm( base, comp ) <= CLUSTER )
			{
				f_partition_v[ i ].merge.push_back( f_partition_v[ j ].position[ 0 ] );
				for( unsigned k = 0; k < f_partition_v[ j ].merge.size( ); ++k )
				{
					f_partition_v[ i ].merge.push_back( f_partition_v[ j ].merge[ k ] );
				}

				cout << "Cluster!!!" << endl;

				f_partition_v[ j ].cluster_b = true;
				break;
			}

		} // end for j
	} //end for i;

}


void MeanShift::createClasses( const vector<SWin> &f_partition_v, 
							  vector< vector<Point3i> > &f_classes_v )
{
	for( unsigned i = 0; i < f_partition_v.size(); ++i )
	{
		// If it is already clustered, don't mind to create a new class
		if( f_partition_v[ i ].cluster_b )
		{
			continue;
		}

		// Only leader type windows will define a class.
		// Gray type windows do not define classes.
		// Follower type windows do not define classes. All of them
		// should be considered in the classes of their respective
		// leaders
		if( f_partition_v[ i ].type_e == LEADER_S )
		{
			// Set some space aside to store the locations
			// of the leader win and all of its followers
			vector<Point3i> buffer;

			// Include the point itself in the class
			buffer.push_back( f_partition_v[ i ].position[ 0 ] );

			// Also include all of its followers
			for( unsigned f = 0; f < f_partition_v[ i ].merge.size(); ++f )
			{
				buffer.push_back( f_partition_v[i].merge[f] );
			}
			f_classes_v.push_back( buffer );

		}  // end if

	} // end for i
}


void MeanShift::countNonDiscardedWindows( const vector<SWin> & f_partition_v )
{
	int countGray_i = 0;
	for( unsigned i = 0; i < f_partition_v.size(); ++i )
	{
		if( f_partition_v[ i ].type_e == GRAY_S )
		{
			countGray_i++;
		}
	}

	cout << "Windows that could define a class = " << f_partition_v.size() - countGray_i 
		<< "  Gray win = " << countGray_i << endl;

}


inline  float MeanShift::norm( const Point3i f_point1, const Point3i f_point2 )
{
	const float value_f = sq( (float)f_point1.x - (float)f_point2.x ) +
		sq( (float)f_point1.y - (float)f_point2.y ) +
		sq( (float)f_point1.z - (float)f_point2.z );

	return value_f;
}

int MeanShift::myRound(float num)
{
	double temp = (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
	return (int)temp;
}

void MeanShift::showImage( Mat f_img, const string &f_name_s )
{
	// Create the window for displing the image           
	// The second argument is set to zero to allow resizing of the window
	namedWindow( f_name_s, CV_WINDOW_AUTOSIZE );

	// Make the input image an 8bit one if necessary
	Mat img8bit = makeIt8bit( f_img );

	// Show image
	imshow( f_name_s, img8bit );
	//Wait for a key
	int key_i = waitKey( 0 );

	//Clean data
	destroyWindow( f_name_s );
}


Mat MeanShift::makeIt8bit( Mat f_img )
{
	if( f_img.depth() > CV_8S )
	{
		// Create a target image
		Mat img8bit;
		// Convert the input image into an 8 bit one
		f_img.convertTo( img8bit, CV_8U ); 

		return img8bit;             
	} 
	else
	{
		return f_img;
	}
}

Mat MeanShift::standardization(const Mat& f_img)
{
	Mat output;

	double max = -9999;
	double min = 9999;
	minMaxLoc(f_img, &min, &max);

	output = (f_img - (min + 0.1)) * (255./(max - min));

	return output;
}

Mat MeanShift::resetColor(const Mat seg_img, const Mat L_img) // here seg_img & L_img are CV_8UC1, in [0, 255].
{
//register segments
	vector<uchar> seg_v;
	for(int y = 0; y < seg_img.rows; ++y)
	{
		for(int x = 0; x < seg_img.cols; ++x)
		{
			uchar thisSeg = seg_img.at<uchar>(y, x);
			vector<uchar>::iterator it = find(seg_v.begin(), seg_v.end(), thisSeg);
			if(it == seg_v.end())
				seg_v.push_back(thisSeg);
		} // x
	} // y

//count average color
	vector<double> sum_v(seg_v.size(), 0);
	vector<int> counter_v(seg_v.size(), 0);
	vector<uchar> average_v(seg_v.size(), 0);
	for(int y = 0; y < seg_img.rows; ++y)
	{
		for(int x = 0; x < seg_img.cols; ++x)
		{
			uchar thisSeg = seg_img.at<uchar>(y, x);
			double thisColor = (double)L_img.at<uchar>(y, x);
			for(int i = 0; i < seg_v.size(); ++i)
			{
				if(thisSeg == seg_v[i])
				{
					
					sum_v[i] += thisColor;
					counter_v[i] += 1;
					break;
				}
			}
		} // x
	} // y

	for(int i = 0; i < counter_v.size(); ++i)
	{
		if(counter_v[i] > 0)
		{
			average_v[i] = (uchar)(sum_v[i] / counter_v[i]);
		}
	}

//reset color
	Mat out_img(seg_img.rows, seg_img.cols, CV_8UC1);

	for(int y = 0; y < seg_img.rows; ++y)
	{
		for(int x = 0; x < seg_img.cols; ++x)
		{
			uchar thisSeg = seg_img.at<uchar>(y, x);
			for(int i = 0; i < seg_v.size(); ++i)
			{
				if(thisSeg == seg_v[i])
				{
					out_img.at<uchar>(y, x) = average_v[i];
					break;
				}
			}
		} // x
	} // y

	return out_img;
}

Mat MeanShift::resetColor2(const Mat seg_img, const Mat Original_img)
{
	//register segments
	vector<uchar> seg_v;
	for(int y = 0; y < seg_img.rows; ++y)
	{
		for(int x = 0; x < seg_img.cols; ++x)
		{
			uchar thisSeg = seg_img.at<uchar>(y, x);
			vector<uchar>::iterator it = find(seg_v.begin(), seg_v.end(), thisSeg);
			if(it == seg_v.end())
				seg_v.push_back(thisSeg);
		} // x
	} // y

//count average color
	vector<double> sum_B_v(seg_v.size(), 0);
	vector<double> sum_G_v(seg_v.size(), 0);
	vector<double> sum_R_v(seg_v.size(), 0);
	vector<int> counter_v(seg_v.size(), 0);
	vector<Vec3b> average_v(seg_v.size(), 0);

	for(int y = 0; y < seg_img.rows; ++y)
	{
		for(int x = 0; x < seg_img.cols; ++x)
		{
			uchar thisSeg = seg_img.at<uchar>(y, x);
			Vec3b thisColor = Original_img.at<Vec3b>(y, x);
			double thisB = (double)thisColor.val[0];
			double thisG = (double)thisColor.val[1];
			double thisR = (double)thisColor.val[2];
			for(int i = 0; i < seg_v.size(); ++i)
			{
				if(thisSeg == seg_v[i])
				{
					
					sum_B_v[i] += thisB;
					sum_G_v[i] += thisG;
					sum_R_v[i] += thisR;
					counter_v[i] += 1;
					break;
				}
			}
		} // x
	} // y

	for(int i = 0; i < counter_v.size(); ++i)
	{
		if(counter_v[i] > 0)
		{
			average_v[i].val[0] = (uchar)(sum_B_v[i] / counter_v[i]);
			average_v[i].val[1] = (uchar)(sum_G_v[i] / counter_v[i]);
			average_v[i].val[2] = (uchar)(sum_R_v[i] / counter_v[i]);
		}
	}

//reset color
	Mat out_img(seg_img.rows, seg_img.cols, CV_8UC3);

	for(int y = 0; y < seg_img.rows; ++y)
	{
		for(int x = 0; x < seg_img.cols; ++x)
		{
			uchar thisSeg = seg_img.at<uchar>(y, x);
			for(int i = 0; i < seg_v.size(); ++i)
			{
				if(thisSeg == seg_v[i])
				{
					out_img.at<Vec3b>(y, x) = average_v[i];
					break;
				}
			}
		} // x
	} // y

	return out_img;
}

Mat MeanShift::smoothFeatureSpace( const Mat f_space, const SImgDims &f_imgDims)
{
	int window_radius = 4;
	int sigma = 2;
	int dim[3];
	dim[0] = f_imgDims.blue;
	dim[1] = f_imgDims.green;
	dim[2] = f_imgDims.red;
	Mat output(3, dim, CV_32FC1);
	cout << "Start smoothing feature space  ";
	for(int z = 0; z < dim[0]; ++z)
	{
		if( z%10 == 0 )
			cout << ".";
		for(int y = 0; y < dim[1]; ++ y)
		{
			for(int x = 0; x < dim[2]; ++x)
			{
				double sum_value = 0;
				double sum_weight = 0;
				for(int i = z - window_radius; i < z + window_radius; ++i) // go over the smooth window
				{
					for(int j = y - window_radius; j < y + window_radius; ++j)
					{
						for(int k = x - window_radius; k < x + window_radius; ++k)
						{
							if(i < 0 || j < 0 || k < 0 || i >= dim[0] || j >= dim[1] || k >= dim[2]) // keep in image range
								continue;
							double e_distance = E_distance(z, y, x, i, j, k);
							double this_weight = Gaussian(e_distance, sigma);
							double this_value = f_space.at<float>(i,j,k);
							sum_value += this_value*this_weight;
							sum_weight += this_weight;
						} //k
					} // j
				} // i
				output.at<float>(z,y,x) = sum_value/sum_weight;
			} // x
		} // y
	} // z
	cout << "done." << endl;
	
	return output;
}
