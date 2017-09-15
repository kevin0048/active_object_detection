#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv;
using namespace raspicam;

int main(int argc,const char **argv)
{
    Mat image;
    RaspiCam_Cv camera; //Camera object
    cout<<"opening camear..."<<endl;
    if (!camera.open()){cerr<<"Error opening"<<endl; return -1;}

    camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    //wait a while until camera stabilize
    cout<<"Sleeping for 3 secs"<<endl;
    usleep(3*1000);
    while(1){
        camera.grab();
        //capture
        camera.retrieve(image);
        cout<<"The size of the image:"<<image.size<<endl;
        cout<<"The dims of the image:"<<image.dims<<endl;
        imshow("RPI Camera", image);
        if(waitKey(25)>=0){break;}
    }
    camera.release();
    
    return 0;
}
