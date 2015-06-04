#include "Helper.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/**
 * Finding intersection over union of 
 * two rectangles
 */
double findIOU(Rect r1, Rect r2)
{
    Rect intersection = r1 & r2;
    double un = r1.area() + r2.area() - intersection.area();
    return intersection.area()/un;
}

