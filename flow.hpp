#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <climits>
#include <cv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

using namespace std;
using namespace cv;

class OpticalFlow {
  Mat i0,i1;
	Mat flow;
  
  Point2f meanFlow;
  Point2f speed;
  Point direction;
  
  Point2f realSize;
  Point2f thresholdDirection;
  float fps;

public:
	OpticalFlow(Point2f& _realSize, float _fps, Point2f& _thresholdDirection,
              const char* imagePath0, const char* imagePath1);
  void recompute(const char* imagePath0, const char* imagePath1);
  
  // return less than zero if left; greater if right; zero otherwise.
	int getDirectionX(void);
  
  // return the absolut speed in the unit (realSize/fps) [e.g. m/s, pxl/s]
	float getSpeedX(void);
  
  void draw(void);
  void write(void);
};
