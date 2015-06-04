/*  Copyright 2011 AIT Austrian Institute of Technology
 *
 *   This file is part of OpenTLD.
 *
 *   OpenTLD is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   OpenTLD is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
/*
 * MainX.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#include "Main.h"

#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "TLDUtil.h"
#include "Trajectory.h"
#include "FaceDetection.h"
#include "Helper.h"

using namespace tld;
using namespace cv;

void Main::doWork()
{
    Trajectory trajectory;
    IplImage *img = imAcqGetImg(imAcq);
    Mat grey(img->height, img->width, CV_8UC1);
    cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);

    tld->detectorCascade->imgWidth = grey.cols;
    tld->detectorCascade->imgHeight = grey.rows;
    tld->detectorCascade->imgWidthStep = grey.step;


    // Initializing face detector TODO: Add into config
    FaceDetection fd(std::string("/usr/local/Cellar/opencv/2.4.10.1/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml"));    


    if(showTrajectory)
    {
        trajectory.init(trajectoryLength);
    }

    if(selectManually)
    {

        CvRect box;

        if(getBBFromUser(img, box, gui) == PROGRAM_EXIT)
        {
            return;
        }

        if(initialBB == NULL)
        {
            initialBB = new int[4];
        }

        initialBB[0] = box.x;
        initialBB[1] = box.y;
        initialBB[2] = box.width;
        initialBB[3] = box.height;
    }

    FILE *resultsFile = NULL;

    if(printResults != NULL)
    {
        resultsFile = fopen(printResults, "w");
        if(!resultsFile)
        {
            fprintf(stderr, "Error: Unable to create results-file \"%s\"\n", printResults);
            exit(-1);
        }
    }

    bool reuseFrameOnce = false;
    bool skipProcessingOnce = false;

    if(loadModel && modelPath != NULL)
    {
        tld->readFromFile(modelPath);
        reuseFrameOnce = true;
    }
    else if(initialBB != NULL)
    {
        Rect bb = tldArrayToRect(initialBB);

        printf("Starting at %d %d %d %d\n", bb.x, bb.y, bb.width, bb.height);

        tld->selectObject(grey, &bb);
        skipProcessingOnce = true;
        reuseFrameOnce = true;
    }

    int frame_number = imAcqVidGetNextFrameNumber(imAcq) - 3;

    // Reading all frames and detecting all faces in every 10th frame
    vector<vector<int> > faceID_at_frames;
    vector<int> frameNo_at_frames;
    int flag1 = 0;

    while(imAcqHasMoreFrames(imAcq))
    {
        cvReleaseImage(&img);
        img = imAcqGetImg(imAcq);
        if (flag1 % 10 == 0)  
        {
            vector<int> face_vec;
            faceID_at_frames.push_back(face_vec);
            frameNo_at_frames.push_back(imAcqVidGetNextFrameNumber(imAcq));
            vector<Rect> faces = fd.detectFace(Mat(img));
            cout << frameNo_at_frames[flag1/10] << ":" << faces.size() << endl;
            for (int j = 0; j < faces.size() ; ++j)
                faceID_at_frames[flag1/10].push_back(j);     
        }
        flag1++;
    }
    cout << frameNo_at_frames[nextFace(faceID_at_frames)] << endl;

    while(nextFace(faceID_at_frames) != -1)
    {   
        // Getting the first non-zero faceID
        int next_id = nextFace(faceID_at_frames);
        int next_frame = frameNo_at_frames[nextFace(faceID_at_frames)];
        
        cout << next_id << ":" << next_frame << ":" << faceID_at_frames[next_id].size() << endl;

        // Moving the pointer to the one with the first non-tracked face
        imAcqVidSetNextFrameNumber(imAcq, next_frame);
        int flag1 = 0;
        while(imAcqVidGetNextFrameNumber(imAcq) < imAcq->lastFrame)  
        { 

            // Moving the pointer forward to the required frame
            while(imAcqVidGetNextFrameNumber(imAcq) < next_frame - 1)
            {
                cvReleaseImage(&img);
                img = imAcqGetImg(imAcq);
            }

            double tic = cvGetTickCount();
            if(!reuseFrameOnce)
            {
                cvReleaseImage(&img);
                img = imAcqGetImg(imAcq);
                if (flag1 == 0)
                {
                    vector<Rect> faces = fd.detectFace(Mat(img));
                    cout << faces.size() << endl;
                    if (1)
                    {
                        cout << faceID_at_frames[next_id].size() << endl;
                        tld->selectObject(grey, &faces[faceID_at_frames[next_id][0]]);
                        faceID_at_frames[next_id].erase(faceID_at_frames[next_id].begin());
                        cout << faceID_at_frames[next_id].size() << endl;
                    }
 
                }

                if (flag1 > 0 && (flag1%10 == 0) && (tld->currBB != NULL))
                {
                    vector<Rect> faces = fd.detectFace(Mat(img));
                    Rect r1(tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height);
                    updateFacesToTrack(r1, faces, faceID_at_frames, next_id + flag1/10);
                }
                flag1++;
                
                
                /*
                if (flag1 % 10 == 0)
                {
                    vector<Rect> faces = fd.detectFace(Mat(img));
                    if (tld->currBB == NULL)
                    {
                        tld->selectObject(grey, &faces[faceID_at_frames[next_id][0]]);
                        faceID_at_frames[next_id].erase(faceID_at_frames[next_id].begin(), faceID_at_frames[next_id].begin() + 1);
                    }
                    if ( && tld->currBB != NULL)
                    {
                        Rect r1(tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height);
                        for (int j = 0; j < faces.size() ; ++j)
                        {
                            Rect r2 = faces[j];
                            cout << findIOU(r1, r2) << ", ";
                        }
                        cout << endl;
                    }
                    if (faces.size() > 0 && tld->currBB == NULL)
                        tld->selectObject(grey, &faces[0]);
                }
                */

                /*
                   if (tld->currBB == NULL)
                   {
                   vector<Rect> faces = fd.detectFace(Mat(img));
                //cout << faces.size()  << endl; 
                if (faces.size() > 0)
                tld->selectObject(grey, &faces[0]);
                }
                */
                if(img == NULL)
                {
                    printf("current image is NULL, assuming end of input.\n");
                    break;
                }

                cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
            }

            if(!skipProcessingOnce)
            {
                tld->processImage(cvarrToMat(img));
            }
            else
            {
                skipProcessingOnce = false;
            }

            if(printResults != NULL)
            {
                if(tld->currBB != NULL)
                {
                    fprintf(resultsFile, "%d %.2d %.2d %.2d %.2d %f\n", imAcq->currentFrame - 1, tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height, tld->currConf);
                }
                else
                {
                    fprintf(resultsFile, "%d NaN NaN NaN NaN NaN\n", imAcq->currentFrame - 1);
                }
            }

            double toc = (cvGetTickCount() - tic) / cvGetTickFrequency();

            toc = toc / 1000000;

            float fps = 1 / toc;

            int confident = (tld->currConf >= threshold) ? 1 : 0;

            if(showOutput || saveDir != NULL)
            {
                char string[128];

                char learningString[10] = "";

                if(tld->learning)
                {
                    strcpy(learningString, "Learning");
                }

                sprintf(string, "#%d,Posterior %.2f; fps: %.2f, #numwindows:%d, %s", imAcq->currentFrame - 1, tld->currConf, fps, tld->detectorCascade->numWindows, learningString);
                CvScalar yellow = CV_RGB(255, 255, 0);
                CvScalar blue = CV_RGB(0, 0, 255);
                CvScalar black = CV_RGB(0, 0, 0);
                CvScalar white = CV_RGB(255, 255, 255);

                if(tld->currBB != NULL)
                {
                    CvScalar rectangleColor = (confident) ? blue : yellow;
                    cvRectangle(img, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 8, 8, 0);

                    if(showTrajectory)
                    {
                        CvPoint center = cvPoint(tld->currBB->x+tld->currBB->width/2, tld->currBB->y+tld->currBB->height/2);
                        cvLine(img, cvPoint(center.x-2, center.y-2), cvPoint(center.x+2, center.y+2), rectangleColor, 2);
                        cvLine(img, cvPoint(center.x-2, center.y+2), cvPoint(center.x+2, center.y-2), rectangleColor, 2);
                        trajectory.addPoint(center, rectangleColor);
                    }
                }
                else if(showTrajectory)
                {
                    trajectory.addPoint(cvPoint(-1, -1), cvScalar(-1, -1, -1));
                }

                if(showTrajectory)
                {
                    trajectory.drawTrajectory(img);
                }

                CvFont font;
                cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
                cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, 50), black, CV_FILLED, 8, 0);
                cvPutText(img, string, cvPoint(25, 25), &font, white);

                if(showForeground)
                {

                    for(size_t i = 0; i < tld->detectorCascade->detectionResult->fgList->size(); i++)
                    {
                        Rect r = tld->detectorCascade->detectionResult->fgList->at(i);
                        cvRectangle(img, r.tl(), r.br(), white, 1);
                    }

                }


                if(showOutput)
                {
                    gui->showImage(img);
                    char key = gui->getKey();

                    if(key == 'q') break;

                    if(key == 'b')
                    {

                        ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;

                        if(fg->bgImg.empty())
                        {
                            fg->bgImg = grey.clone();
                        }
                        else
                        {
                            fg->bgImg.release();
                        }
                    }

                    if(key == 'c')
                    {
                        //clear everything
                        tld->release();
                    }

                    if(key == 'l')
                    {
                        tld->learningEnabled = !tld->learningEnabled;
                        printf("LearningEnabled: %d\n", tld->learningEnabled);
                    }

                    if(key == 'a')
                    {
                        tld->alternating = !tld->alternating;
                        printf("alternating: %d\n", tld->alternating);
                    }

                    if(key == 'e')
                    {
                        tld->writeToFile(modelExportFile);
                    }

                    if(key == 'i')
                    {
                        tld->readFromFile(modelPath);
                    }

                    if(key == 'r')
                    {
                        CvRect box;

                        if(getBBFromUser(img, box, gui) == PROGRAM_EXIT)
                        {
                            break;
                        }

                        Rect r = Rect(box);

                        tld->selectObject(grey, &r);
                    }
                }

                if(saveDir != NULL)
                {
                    char fileName[256];
                    sprintf(fileName, "%s/%.5d.png", saveDir, imAcq->currentFrame - 1);

                    cvSaveImage(fileName, img);
                }
            }

            if(reuseFrameOnce)
            {
                reuseFrameOnce = false;
            }
        }
    }

    cvReleaseImage(&img);
    img = NULL;

    if(exportModelAfterRun)
    {
        tld->writeToFile(modelExportFile);
    }

    if(resultsFile)
    {
        fclose(resultsFile);
    }
}
