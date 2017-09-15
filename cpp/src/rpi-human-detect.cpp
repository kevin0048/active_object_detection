#include <iostream>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv;
using namespace raspicam;

HOGDescriptor hog; 

void detect(Mat& image)
{
  vector<Rect> found;
  hog.detectMultiScale(image, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
  size_t i, j;
  for(i = 0; i < found.size(); i++)
  {
    Rect rect = found[i];
    rectangle(image, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2);
  }
}

void init()
{
  hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

int main(int argc, const char **argv)
{
  init();

  RaspiCam_Cv camera;
  Mat image;

  camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
  if(!camera.open())
  {
    cerr << "error opening the camera!" << endl;
    return -1;
  }

  while(1)
  {
    camera.grab();
    camera.retrieve(image);

    //detect(image);

    imshow("RPI Camera", image);
    if(waitKey(25) >= 0)
    {
      break;
    }
  } 

  camera.release();

  return 0;
}
