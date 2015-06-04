#ifndef HELPER_H_
#define HELPER_H_ 

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

/**
 * Finding intersection over union of 
 * two rectangles
 */
double findIOU(Rect r1, Rect r2);

#endif
