#include "stdafx.h"
#include "LibraryClass.h"
#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include <wininet.h> 
#include "yolo_v2_class.h"

#pragma comment(lib, "Wininet")
using namespace cv;
using namespace std;

LibraryClass::LibraryClass()
{
}


LibraryClass::~LibraryClass()
{
}



string LibraryClass::setVideoWriter(int width, int height, string w)
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d-%X", &tstruct);
	for (int i = 0; i < strlen(buf); i++) {
		if (buf[i] == ':') {
			buf[i] = '_';
		}
	}
	string Video_FilePath = "upload\\"+ w + std::string (buf)  + ".mp4";
	//fourcc integer
	//int fcc = CV_FOURCC('D', 'I', 'V', '3');
	int fcc = CV_FOURCC('X', 'V', 'I', 'D');
	

	//frames per sec integer
	int fps = 6;
	videowriter = VideoWriter(Video_FilePath, fcc, fps, Size(width, height), true);
	return Video_FilePath;

}



void LibraryClass::inputFrmToVideo(Mat image)
{
	videowriter << image;
}


