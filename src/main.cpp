#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <getopt.h>

#include "include/FaceDetector.h"
#include "include/serialsetup.h"

/*** CONSTANTS ***/
const int ESC_KEY = 27;
const int F_DELAY = 5;
const std::string XML_PATH = "./resources/haarcascade_frontalface_alt2.xml";

/*** VARIABLES ***/
cv::VideoCapture webcam;
int serial;

/*** FUNCTIONS ***/
static void printHelpFlag() {

  fputs("MagiCam v0.1.0 - (C) La Corte Alessio, Lana Mirko\n"
        "Usage: magicam [options] [arguments]\n\n"
        "Options:\n"
        "  -h --help        \t print help screen\n"
        "  -w --webcam=PATH \t path to webcam file\n"
        "  -s --serial=PATH \t path to serial port\n\n"
        ,stdout);
    
  exit( EXIT_SUCCESS );

}

// -----------------------------------
//

void parseArguments( int argc, char *argv[] );
std::string createPacket( cv::Point center, cv::Mat frame );

int main( int argc, char *argv[] ) {
  
  parseArguments( argc, argv );

  cv::Mat frame;
  FaceDetector faceDetector( XML_PATH );
  int frameCounter = 0;
  
  while ( webcam.read( frame ) ) {

    if ( frame.empty() ) {
			std::cout << "--(!) No captured frame -- Break!\n";
			break;
    }

    faceDetector.display( frame );

    if ( frameCounter == 0 ) {
      std::string pack = createPacket( faceDetector.getCenter(), frame );
      write ( serial, pack.c_str(), pack.length() );
    }

    if ( cv::waitKey(20) == ESC_KEY )
			break;

    frameCounter = ( ++frameCounter > F_DELAY ) ? frameCounter = 0 : frameCounter;

  }

  write( serial, "*0;0@", 6 );
  close(serial);

  return EXIT_SUCCESS;
}

void parseArguments( int argc, char *argv[] ) {

  int useCatFace = 0;

  static struct option long_opts[] = {
    {"help",      no_argument,        0,          'h'},
    {"webcam",    required_argument,  0,          'w'},
    {"serial",    required_argument,  0,          's'},
  };

  if ( argc < 2 )
    printHelpFlag();

  int opt, opti = 0;

  while ( (opt = getopt_long( argc, argv, "hw:s:", long_opts, &opti )) != -1) {

    if ( opt == EOF )
      break;

    switch ( opt ) {
    case 'h':
      printHelpFlag();
      break;
    
    case 'w':
      if ( !webcam.open(  std::string( optarg ) ) ) {
        std::cout << "--(!) Error opening video capture -- Exit!\n";
        exit( EXIT_FAILURE);
      }
      break;

    case 's':
      if ( (serial = init( optarg, O_WRONLY )) <= 0 ) {
        std::cout << "--(!) Error opening serial port -- Exit!\n";
        exit( EXIT_FAILURE );
      }
      break;

    case '?':
      printHelpFlag();
      break;

    default:
      printHelpFlag();
      exit( EXIT_FAILURE );
    }

  }

}

int getXAngle( cv::Point center, cv::Mat frame ) {

	if ( center.x != 0)
		return -(((double) (center.x-frame.cols/2)/(frame.cols/2))*30);
	return 0;

}

int getYAngle( cv::Point center, cv::Mat frame ) 
{
	if ( center.y != 0)
		return (((double) (center.y-frame.rows/2)/(frame.rows/2))*30);
	return 0;

}

std::string createPacket( cv::Point center, cv::Mat frame ) {

	std::ostringstream out;
	out << "*" << getXAngle( center, frame ) << ";" << getYAngle( center, frame ) << "#";
	
  return out.str();

}
