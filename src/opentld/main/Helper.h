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

/**
 * Return frame number to go to for the next face
 */
int nextFace(vector<vector<int> > faces);

/**
 * Update faces to track
 */

void updateFacesToTrack(Rect r1, vector<Rect> faces, vector<vector<int> > &facesID_at_frames, int id);

#endif
