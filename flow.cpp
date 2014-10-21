#include "flow.hpp"

#define SearchSize		2
#define MEM_OFFSET_U	0
#define MEM_OFFSET_V 	1

// -- Draw Flow --

#define PI 3.141592653589793238462643
static inline float cvtVec2Hue(Point& v) {
	float h = atan(fabsf((float)v.y / (float)v.x));
	if (v.x <  0.0 && v.y >= 0.0) h = PI - h;
	if (v.x >= 0.0 && v.y <  0.0) h = 2.0 * PI - h;
	if (v.x <  0.0 && v.y <  0.0) h = h + PI;
	h = 90.0 * h / PI;
	return h;
}

static inline float cvtVec2Value(Point& v, float norm) {
	return (255.0 * (sqrt((float)(v.y*v.y + v.x*v.x)) / norm));
}

static void drawFlow(Mat& flow, const char *window, int idWrite) {
	Mat rgb, hsv(flow.rows,flow.cols,CV_8UC3);
	float norm = sqrt(2*SearchSize*SearchSize);
	int pc = flow.channels();
	for (int i = 0; i < flow.rows; ++i) {
		float *pf = flow.ptr<float>(i);
		uchar *ph = hsv.ptr<uchar>(i);
		for (int j = 0; j < flow.cols; ++j) {
			int u = pf[j*pc+MEM_OFFSET_U];
			int v = pf[j*pc+MEM_OFFSET_V];
			Point pt(u,v);
			ph[j*3] = (uchar)cvtVec2Hue(pt);
			ph[j*3+1] = 100;
			ph[j*3+2] = (uchar)cvtVec2Value(pt,norm);
		}
	}
	cvtColor(hsv,rgb,CV_HSV2BGR);
	imshow(window,rgb);

	if (idWrite > -1) {
		char fn[1024];
		sprintf(fn,"%s-%d.jpg",window,idWrite);
		imwrite(fn,rgb);
	}
}

static void drawFlowMask(const char *window) {
	Mat rgb, hsv(401,401,CV_8UC3);
	Point c(hsv.cols/2,hsv.rows/2);
	float norm = sqrt(c.x*c.x + c.y*c.y);
	for (int i = 0; i < hsv.rows; ++i) {
		uchar *p = hsv.ptr<uchar>(i);
		for (int j = 0; j < hsv.cols; ++j) {
			Point v(j-c.x,c.y-i);
			p[j*3]   = (uchar)cvtVec2Hue(v);
			p[j*3+1] = (uchar)cvtVec2Value(v,norm);
			p[j*3+2] = p[j*3+1];
		}
	}
	cvtColor(hsv,rgb,CV_HSV2BGR);
	imshow(window,rgb);
}

void OpticalFlow::draw(void) {
  drawFlowMask("legend");
  drawFlow(flow, "flow", -1);
	imshow("i0",i0);
	imshow("i1",i1);
	waitKey(0);
}

void OpticalFlow::write(void) {
  drawFlow(flow, "flow", 1);
	imwrite("i0.png",i0);
	imwrite("i1.png",i0);
}

// -- Optical Flow --

#define EPS 0.1
static Point2f getMeanFlow(Mat& flow) {
	float su=0.0f, sv=0.0f;
	int qu=0, qv=0;
	int pc=flow.channels();
	for (int i=0; i < flow.rows; ++i)
	{
		float *pf = flow.ptr<float>(i);
		for (int j=0; j < flow.cols; ++j)
		{
			float u = pf[j*pc+MEM_OFFSET_U];
			float v = pf[j*pc+MEM_OFFSET_V];
			if (fabsf(u) > EPS) {
				su += u;
				qu++;
			}
			if (fabsf(v) > EPS) {
				sv += v;
				qv++;
			}
		}
	}
	float mu = (qu)? (su / (float)qu): 0;
	float mv = (qv)? (sv / (float)qv): 0;
	Point2f m(mu,mv);
	return m;
}

void OpticalFlow::recompute(const char* imagePath0, const char* imagePath1) {
	i0 = imread(imagePath0,0);
	i1 = imread(imagePath1,0);
  if (!i0.data || !i1.data) {
    printf("Invalid image path\n");
    exit(-1);
  }
  Point resolution(i0.rows,i0.cols);

  // raw optical flow
	//calcOpticalFlowFarneback(i0, i1, flow, 0.5, 4, 9, 5, 7, 1.5,
  //                         OPTFLOW_FARNEBACK_GAUSSIAN);
	calcOpticalFlowFarneback(i0, i1, flow, 0.5, 6, 15, 25, 7, 1.5,
	                         OPTFLOW_FARNEBACK_GAUSSIAN);
	meanFlow = getMeanFlow(flow);

  // real speed
  speed.x = ((meanFlow.x/resolution.x)*realSize.x)*fps;
  speed.y = ((meanFlow.y/resolution.y)*realSize.y)*fps;

  // real direction
  direction.x = 0;
  direction.y = 0;
  if (speed.x > thresholdDirection.x)
    direction.x = 1;
  if (speed.x < -thresholdDirection.x)
    direction.x = -1;
  if (speed.y > thresholdDirection.y)
    direction.y = 1;
  if (speed.y < -thresholdDirection.y)
    direction.y = -1;
}

OpticalFlow::OpticalFlow(Point2f& _realSize, float _fps,
                         Point2f& _thresholdDirection,
                         const char* imagePath0, const char* imagePath1)
{
  realSize = _realSize;
  fps = _fps;
  thresholdDirection = _thresholdDirection;
  recompute(imagePath0,imagePath1);
}

int OpticalFlow::getDirectionX(void) {
  return direction.x;
}

float OpticalFlow::getSpeedX(void) {
  return fabsf(speed.x);
}
