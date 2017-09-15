#include <iostream>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <string>
#include "ActiveObjectDetector.hpp"

using namespace std;
using namespace cv;
using namespace raspicam;

int main(){
    //Camera object
    RaspiCam_Cv camera;
    camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    if(!camera.open()){cerr<<"Error opening the camera!"<<endl;return -1;}
    
    //Active object detector object
    ActiveObjectDetector obj;
    //Recording
    obj.recording(camera);
    
    camera.release();
    return 0;
}
