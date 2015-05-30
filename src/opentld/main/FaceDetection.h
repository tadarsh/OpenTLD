#ifndef FACE_DETECT_H_
#define FACE_DETECT_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;

/**
 * Detecting faces in an image. Should return the bounding box (x, y, width, height) of that face whose trajectory hasn't been computed yet (TODO). For now, returning the first of the detected faces.
 */

class FaceDetection
{
public:
    FaceDetection();
    FaceDetection (string haarcascade_path);
    Rect detectFace(Mat frame);
    ~FaceDetection();

private:
    CascadeClassifier face_cascade;
};

#endif
