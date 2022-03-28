cmake_minimum_required(VERSION 3.18)
project(MagiCam VERSION 0.1.0)
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable( magicam ${SRC_FILES} )
target_include_directories( magicam PUBLIC src/include )
target_link_libraries( magicam ${OpenCV_LIBS} )
