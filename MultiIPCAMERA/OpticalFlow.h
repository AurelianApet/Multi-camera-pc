#include <stdio.h>
#include <iostream>
#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "yolo_v2_class.h"

using namespace cv;

#pragma once

struct TriggerArg
{
	Mat image;
	int trigger;
};
class OpticalFlow
{
private:
	int s = 1;
	bool check_f;

	Mat frame1, ret, motion_mask;

	Size frame_size;
	int h, w;
	Mat seg_mask;
	vector<Rect> seg_bounds;

	Mat flow, cflow, frame, lastFrame;
	Mat gray, prevgray, uflow;

	Mat res;
	int frameCount = 0;

	int optMag;
	int cflow_factor;

	Size videoSize;
	float Scale_MotionFrm;

	vector<CString> m_vectorNameList;

public:



	//resize scale
	

	OpticalFlow();
	~OpticalFlow();

	void initOpticalFlow(vector<CString> LabelList);

	void drawOptFlowMap_gpu(const Mat& flow_x, const Mat& flow_y, Mat& cflowmap, int step, const Scalar& color);

	TriggerArg getVectorFlow(Detector* pDetector, TriggerArg arg);

	//cv::Mat findBlobs(Mat src, Mat original, int frameCount, Mat Old_frame, Detector *pDetector);
	TriggerArg findBlobs(Mat src, Mat original, int frameCount, TriggerArg arg, Detector* pDetector);

	TriggerArg getYOLOMat(Mat CropImg, TriggerArg arg, Detector* pDetector, Rect scale_rect);

	
};

