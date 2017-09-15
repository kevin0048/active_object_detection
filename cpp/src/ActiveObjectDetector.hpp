#ifndef ACTIVEOBJECTDETECTOR_H
#define ACTIVEOBJECTDETECTOR_H
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <string>

using namespace std;
using namespace cv;
using namespace raspicam;

class ActiveObjectDetector{
    public:
        /**Default constructor.
         */
        //ActiveObjectDetector(){};
        /**Constructor.
         */
        ActiveObjectDetector(int connectedStyle=8, int threshold=8000, 
                int binaryThreshold=10, int recordingTime=30, 
                string videoSavePath="video/");
     
        /**Process to the frame.
         */
        Mat processFrame(Mat& image, Ptr<BackgroundSubtractorMOG2>& fgbg, Mat kernel);
        /**Detect.
         */
        bool detect(Mat& fgmask);
        /**Recoding video.
         */
        void recording(RaspiCam_Cv& camera);
        /**Upload video file through ssh.
         */
        void upload();
        /**Main function for active object detect.
         */
        void objectDetector();

        //Accessors
    private:
        RaspiCam_Cv camera;
        int connectedStyle;
        int threshold;
        int binaryThreshold;
        int recordingTime;
        string videoSavePath;

};
#endif
