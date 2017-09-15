#include <iostream>
#include <raspicam/raspicam.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace raspicam;


int main(){
    
    RaspiCam camera;
    camera.setWidth(640);
    camera.setHeight(480);
    camera.setFrameRate(16);
    camera.setFormat(RASPICAM_FORMAT_BGR);

    if(!camera.open()){cerr<<"Error opening camera"<<endl;return -1;}



    
    unsigned char *data=new unsigned char[ camera.getImageTypeSize( RASPICAM_FORMAT_BGR)];
    
    while(true){
     
        camera.grab();
        camera.retrieve(data);

        Mat image(Size(camera.getWidth(), camera.getHeight()), CV_8UC3, data);
        imshow("image", image);
        if(waitKey(25)>=0){
            break;
    } 
    }
    delete data;
    return 0;
}
