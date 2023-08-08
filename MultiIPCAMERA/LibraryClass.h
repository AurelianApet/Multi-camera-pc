#pragma once
#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "yolo_v2_class.h"
using namespace cv;
class LibraryClass
{
public:
	LibraryClass();
	~LibraryClass();
	VideoWriter videowriter;
	string setVideoWriter(int width, int height, string w); //set videowriter object such as video file name, extension, width, height
	void inputFrmToVideo(Mat image);	//save image to video file frame by frames
	/*void FileSubmit(string filepath, string dest, string ftpserver);*/
};

