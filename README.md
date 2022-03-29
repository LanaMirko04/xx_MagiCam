# MagiCam 
## Table of Contents
- [General info](#general-info)
- [Build instruction](#build-instruction)
- [License](#license)

## General info
`magicam` is a face detection program powered by `OpenCV` and written in C/C++.
The purpose of this program is to communicate with an Arduino board connected to 2 servos, a series of angles for moving the X and Y axis.
<!-- GIF Image -->

## Build instruction
**Prerequisite**
List of build-time dependencies:
 - C++ compiler
 - `CMake`
 - `OpenCV`

**Note about `OpenCV`:**
> See the [OpenCV](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html) documentation to installation it on Linux

### Compile from source:
- Clone this repository using `git`
	```bash
	git clone https://github.com/ItalianDudes/MagiCam
	```
- Create build directory
	```bash
	mkdir build && cd build
	```
- Configure - generate build scripts for the preferred build system:
	```bash
	cmake ../MagiCam
	```
- Build - run actual compilation process:
	```bash
	make
	```

## License
Do What the Fuck You Want to Public License (WTFPL)
