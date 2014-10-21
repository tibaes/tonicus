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

#define SearchSize		2
#define MEM_OFFSET_U	0
#define MEM_OFFSET_V 	1

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

void drawFlow(Mat& flow, const char *window, int idWrite) {
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
			ph[j*3+1] = 100; //(uchar)cvtVec2Value(pt,norm);
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

void drawFlowMask(const char *window) {
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

#define EPS 0.1
Point2f meanFlow(Mat& flow) {
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

Point direction(Point2f& mf, Point2f& threshold) {
	Point d(0,0);
	if (mf.x > threshold.x)
		d.x = 1;
	if (mf.x < -threshold.x)
		d.x = -1;
	if (mf.y > threshold.y)
		d.y = 1;
	if (mf.y < -threshold.y)
		d.y = -1;
	return d;
}

Point2f speed(Point2f& mf, Point& resolution, Point2f& realSize) {
	Point2f spd(0.0f,0.0f);	
 	spd.x = ((mf.x/resolution.x)*realSize.x);
	spd.y = ((mf.y/resolution.y)*realSize.y);
	return spd;
}

int main(int argc, char **argv) {
	drawFlowMask("legenda");

	// Mat i0,i1;
	Mat i0 = imread(argv[1],0);
	Mat i1 = imread(argv[2],0);
    
	//GaussianBlur(s0, i0, cvSize(5,5), 0);
    //GaussianBlur(s1, i1, cvSize(5,5), 0);
    
	Mat flow;
	calcOpticalFlowFarneback(i0, i1, flow, 0.5, 4, 9, 5, 7, 1.5,
                           OPTFLOW_FARNEBACK_GAUSSIAN);
	Point2f mf = meanFlow(flow);
	printf("Mean Flow: %f %f\n",mf.x,mf.y);
	
	Point res(i0.rows,i0.cols);
	Point2f realSize(1.0,1.0);
	Point2f spd = speed(mf,res,realSize);
	printf("Velocidade relativa: %f\n",fabsf(spd.x));

	Point2f thr(0.002f,0.002f); // % relativa a imagem * tamanho real
	Point dir = direction(spd,thr);
	switch (dir.x) {
		case  1: printf("Direita\n"); break;
		case -1: printf("Esquerda\n"); break;
		case  0: printf("Parado\n"); break;
	}
	
	drawFlow(flow, "flow", 1);
	imshow("i0",i0);
	imwrite("i0.png",i0);
	imshow("i1",i1);
	imwrite("i1.png",i0);
	waitKey(100);

	return 0;
}
