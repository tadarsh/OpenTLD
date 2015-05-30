#include "FaceDetection.h"

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

FaceDetection::~FaceDetection()
{
}

FaceDetection::FaceDetection()
{
    string haarcascade_path = "/usr/local/Cellar/opencv/2.4.10.1/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
    face_cascade.load(haarcascade_path);

}

FaceDetection::FaceDetection(string haarcascade_path)
{
    face_cascade.load(haarcascade_path);

}
vector<Rect> FaceDetection::detectFace(Mat frame)
{
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 4, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    
    return faces;    
}


