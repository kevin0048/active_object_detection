#include <iostream>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <string>

#include "ActiveObjectDetector.hpp"

using namespace std;
using namespace cv;
using namespace raspicam;


ActiveObjectDetector::ActiveObjectDetector(int connectedStyle, int threshold, int binaryThreshold,
        int recordingTime, string videoSavePath){
    this->connectedStyle=connectedStyle;
    this->threshold=threshold;
    this->binaryThreshold=binaryThreshold;
    this->recordingTime=recordingTime;
    this->videoSavePath=videoSavePath;
}

// Mat ActiveObjectDetector::getFrame(){}


//Main function for active object detect.
//void ActiveObjectDetect::objectDetector(){
//    
//}

//Process frame.
Mat ActiveObjectDetector::processFrame(Mat& image, Ptr<BackgroundSubtractorMOG2>& fgbg, 
        Mat kernel){
    Mat gray;
    Mat fgmask;
    cvtColor(image, gray, CV_BGR2GRAY);
    equalizeHist(gray, gray);
    GaussianBlur(gray, gray, Size(11,11), 0, 0);
    fgbg->apply(gray, fgmask);
    morphologyEx(fgmask, fgmask, MORPH_OPEN, kernel);
    
    return fgmask;
}

//Detect.
bool ActiveObjectDetector::detect(Mat& fgmask){
    Mat labels;
    Mat stats;
    Mat centroids;

    connectedComponentsWithStats(fgmask, labels, stats, centroids, 
            this->connectedStyle);

    for(int i=1; i<stats.rows; i++){
        int count=0;
        for(int r=0; r<labels.rows; r++){
            for(int c=0; c<labels.rows; c++){
                if(labels.at<int>(r,c)==i){
                    count ++;
                }
            }
        }
        if(count>this->threshold){
            return true;
        }
    }

    return false;
}



//Recording and imshowing
void ActiveObjectDetector::recording(RaspiCam_Cv& camera){
   Mat image;
   
   cout<<camera.get(CV_CAP_PROP_FRAME_WIDTH)<<" "<<camera.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;
   cout<<camera.get(CV_CAP_PROP_FPS)<<endl;
   while(true){
       camera.grab();
       camera.retrieve(image);
       if (image.empty()){
           break;
       }

       imshow("image", image);
       if(waitKey(25)>=0){
           break;
       }
   }
}

