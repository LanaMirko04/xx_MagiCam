#ifndef __FACEDETECTOR_H__
#define __FACEDETECTOR_H__

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>

class FaceDetector {
public:
  /* Methods */
  FaceDetector( const std::string &path );
  cv::Point getCenter();
  void display( cv::Mat &frame );
  ~FaceDetector();
private:
  /* Attributes */
  cv::CascadeClassifier faceCascade;
  cv::Point center;
};

#endif