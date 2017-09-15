#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv;
using namespace raspicam;

int main(int argc,const char **argv)
{
    Mat image1;
    Mat image2;
    Mat fgmask;
    
    Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    pMOG2 = createBackgroundSubtractorMOG2();

    RaspiCam_Cv camera; //Camera object
    cout<<"opening camear..."<<endl;
    if (!camera.open()){cerr<<"Error opening"<<endl; return -1;}

    camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    //wait a while until camera stabilize
    cout<<"Sleeping for 3 secs"<<endl;
    usleep(3*1000);
    
    camera.grab();
    //capture
    camera.retrieve(image1);
    
    camera.grab();
    camera.retrieve(image2);
    
    cvtColor(image1, image1, CV_BGR2GRAY);
    cvtColor(image2, image2, CV_BGR2GRAY);
    
    //imshow("image1",image1);
    waitKey(0);
    destroyAllWindows();
    //imshow("image2",image2);
    waitKey(0);
    destroyAllWindows();


    equalizeHist(image1, image1);
    equalizeHist(image2, image2);

    GaussianBlur(image1, image1, Size(11,11), 0, 0);
    GaussianBlur(image2, image2, Size(11,11), 0, 0);



    pMOG2->apply(image1,fgmask);
    morphologyEx(fgmask, fgmask, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(3,3)));


    pMOG2->apply(image2,fgmask);    
    morphologyEx(fgmask, fgmask, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(3,3)));

    Mat sub;

    absdiff(image2, image1, sub);
    
    threshold(sub, sub, 10, 1, THRESH_BINARY);

    fgmask = fgmask.mul(sub);
    
    //imshow("RPI Camera", fgmask);
    
    Mat labels;
    Mat stats;
    Mat centroids;

    connectedComponentsWithStats(fgmask, labels, stats, centroids, 8);
    
    //cout << "labels=" <<endl <<" "<< labels << endl;
    //cout << "stats=" << endl << " "<< stats << endl;
    //cout << "centroids=" << endl << " "<< centroids << endl;
    //cout << "stats rows = " << stats.rows << endl; 
    for(int i=1;i<stats.rows;i++){
        int count = 0;
        for(int r=0;r<labels.rows;r++){
            for(int c=0;c<labels.cols;c++){
                if(labels.at<int>(r,c)==i){
                    count++;
                }
            }
        }
        //cout<<i<<endl;

       
            cout<<count<<" ";
       
    }
    

    //waitKey(0); 
    camera.release();
    
    return 0;
}
