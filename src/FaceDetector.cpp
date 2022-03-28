#include <iostream>
#include <stdlib.h>
#include "include/FaceDetector.h"

FaceDetector::FaceDetector( const std::string &path ) {

  if ( !faceCascade.load( path ) ) {
    std::cout << "--(!) Error opening XML file -- Exit!\n";
    exit( EXIT_FAILURE );
  }

  cv::namedWindow( "Face detection" );
}

cv::Point FaceDetector::getCenter() {
  return center;
}

void FaceDetector::display( cv::Mat &frame ) {
  cv::Mat frameGray;
	cvtColor( frame, frameGray, cv::COLOR_BGR2GRAY );
	equalizeHist( frameGray, frameGray );

  std::vector<cv::Rect> faces;
  faceCascade.detectMultiScale( frame, faces );

  cv::Rect face;
  for ( auto tmp: faces )
    if ( face.area() < tmp.area() )
      face = tmp;

  center = cv::Point ( face.x + face.width/2, face.y+face.height/2 );
	ellipse( frame, center, cv::Size( face.width/2, face.height/2 ), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4 );

	cv::Mat faceROI = frameGray( face );
	
  imshow( "Face detection", frame );
}

FaceDetector::~FaceDetector() {
  cv::destroyWindow( "Face detection" );
}