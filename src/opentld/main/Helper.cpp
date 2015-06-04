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

/**
 * Return frame number to go to for the next face
 * returns -1 if there are no more faces to track
 */
int nextFace(vector<vector<int> > faces)
{
    for (int i = 0; i < faces.size(); ++i)
        if (faces[i].size() > 0)
            return i;
    return -1;
}

/**
 * Update faces to track
 */

void updateFacesToTrack(Rect r1, vector<Rect> faces, vector<vector<int> > &facesID_at_frames, int id)
{
    int max_id = 0;
    double max_iou = 0;
    for (int i = 0; i < faces.size(); ++i)
    {
       if (findIOU(r1, faces[i]) > max_iou) 
       {
            max_id = i;
            max_iou = findIOU(r1, faces[i]);
       }
    }
    cout << max_iou << ":" << id << endl;
    if (max_iou > 0)
    {
        for (int i = 0; i < facesID_at_frames[id].size(); ++i)
        {
            if (facesID_at_frames[id][i] == max_id)
            {
                cout << "Removed at" << id * 10 << endl;

                facesID_at_frames[id].erase(facesID_at_frames[id].begin() + i);
            }
        }       
    }
}



