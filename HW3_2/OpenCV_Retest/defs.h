#ifndef FILE_DEFS_h
#define FILE_DEFS_h

#include <vector>
#include <iostream>

#include <opencv2/core/core.hpp>

using std::vector;
using std::cout;

inline float sq( const float a)
{
    return a * a;
}

enum winType
{
  GRAY_S, // irrelevant window
  FOLLOWER_S,
  LEADER_S
};

struct SWin // Window in feature space
{
  // History of positions
  vector<cv::Point3i> position;

  // Following someone?
  bool follow_b;
  
  // Clustered? i.e. it belong to another class
  bool cluster_b;
    
  // Put here the initial point
  // of the window that it merges or follows
  vector<cv::Point3i> merge;
    
  bool active_b;

  winType type_e;
};

struct SImgDims  //Dimension of feature space. The features are R, G, and B. Normally a 8bit RGB is 256x256x256. 
{
  unsigned blue;
  unsigned green;
  unsigned red;

  void print()
  {
    cout << blue << " (blue) x " << green << " (green) x "
	 << red << " (red)\n";
  }


};

struct SClass
{
  vector<cv::Point3i> points_v;
  bool cluster_b;
};



#endif /* FILE_DEFS_h */
