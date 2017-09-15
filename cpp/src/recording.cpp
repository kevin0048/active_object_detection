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
    //camera.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    //camera.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
    //wait a while until camera stabilize
    cout<<"Sleeping for 3 secs"<<endl;
    usleep(3*1000);
    camera.grab();
    camera.retrieve(image);

    bool isColor = (image.type() == CV_8UC3);
    VideoWriter writer;
    int codec = CV_FOURCC('H','2','6','4');
    double fps = 25.0;
    string filename = "video/output.h264";
    
    writer.open(filename, codec, fps, image.size(), isColor); //VideoWriter object

    if(!writer.isOpened()){
        cerr << "Could not open the output video file for write\n";
        return -1;
    }
    
    while(1){
        camera.grab();
        //capture
        camera.retrieve(image);
        
        if(image.empty()){
            break;
        }

        writer.write(image);
        cout<<"The size of the image:"<<image.size<<endl;
        //cout<<"The dims of the image:"<<image.dims<<endl;
        
        
        
        imshow("RPI Camera", image);
        if(waitKey(25)>=0){break;}
    }
    camera.release();
    writer.release();
    return 0;
}
