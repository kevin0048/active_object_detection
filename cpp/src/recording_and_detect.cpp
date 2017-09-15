#include <iostream>
#include <unistd.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv;
using namespace raspicam;


class ActiveObjectDetect{
    public:
        void process_frame(Mat& image, Mat& fgmask, Ptr<BackgroundSubtractorMOG2> fgbg, Mat kernel){
           Mat gray;
           cvtColor(image, gray, CV_BGR2GRAY);
           equalizeHist(gray, gray);
           GaussianBlur(gray, gray, Size(11, 11), 0, 0);
           fgbg->apply(gray, fgmask);
           morphologyEx(fgmask, fgmask, MORPH_OPEN, kernel);
        }
        // RaspiCam_Cv camera'
        // recording(&camera);
        // void recording(RaspiCam_Cv *camera){
        //   camera->xxxx
        // }
        void recording(RaspiCam_Cv& camera, Mat& image1, Mat& image2,string){
            
            Mat image;

        }


        
        bool detect(Mat& fgmask, int threshold){
            return false;        
        }




};





int main(int argc, const char **argv)
{
    Mat image1;
    Mat image2;
    Mat gray1;
    Mat gray2;
    Mat fgmask;
    Mat sub;
    RaspiCam_Cv camera; //Camera object
    if (!camera.open()){cerr << "Error opening camera" << endl; return -1;}

    // some setting for camera
    camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    
    // wait a while until camera stabilize
    cout << "Sleeping camera for 3 secs" << endl;
    usleep(3*1000);
    
    // prepare
    Ptr<BackgroundSubtractorMOG2> fgbg;
    fgbg=createBackgroundSubtractorMOG2();
    Mat kernel=getStructuringElement(MORPH_ELLIPSE, Size(3,3));

    /*
    Mat image;
    camera.grab();
    camera.retrieve(image);
    cvtColor(image, image, CV_BGR2GRAY);
    bool isColor = (image.type() == CV_8UC3);
    VideoWriter writer;
    int codec = CV_FOURCC('H','2','6','4');
    double fps = 15.0;
    string filename = "output.h264";
    
    writer.open(filename, codec, fps, image.size(), isColor); //VideoWriter object
    */

    while(true){
        // get frame1
        camera.grab();
        camera.retrieve(image1);
        // process frame1
        cvtColor(image1, gray1, CV_BGR2GRAY);
        equalizeHist(gray1, gray1);
        GaussianBlur(gray1, gray1, Size(11, 11), 0, 0);
        // apply frame1 to fgbg
        fgbg->apply(gray1, fgmask);
        morphologyEx(fgmask, fgmask, MORPH_OPEN, kernel);
        

        // get frame2
        camera.grab();
        camera.retrieve(image2);
        // process frame2
        cvtColor(image2, gray2, CV_BGR2GRAY);
        equalizeHist(gray2, gray2);
        GaussianBlur(gray2, gray2, Size(11, 11), 0, 0);
        // apply frame2 to fgbg
        fgbg->apply(gray2, fgmask);
        morphologyEx(fgmask, fgmask, MORPH_OPEN, kernel);

        absdiff(gray1, gray2, sub);
        threshold(sub, sub, 10, 1, THRESH_BINARY);
        fgmask=fgmask.mul(sub);
        
        imshow("fgmask", fgmask);
        //
        //writer.write(fgmask);
        if(waitKey(25)>=0){
            break;
        }
        
        //usleep(200);        

        
        // detect
        Mat labels;
        Mat stats;
        Mat centroids;

        connectedComponentsWithStats(fgmask, labels, stats, centroids, 8);

        for(int i=1; i<stats.rows; i++){
            int count = 0;
            for(int r=0; r<labels.rows; r++){
                for(int c=0; c<labels.rows; c++){
                    if(labels.at<int>(r, c)==i){
                        count ++;
                    }
                }
            }

            if(count>8000){
                cout<<"Detect"<<endl;
            }
        }
        
    }
    camera.release();
    // writer.release();
    destroyAllWindows();
    return 0;

}
