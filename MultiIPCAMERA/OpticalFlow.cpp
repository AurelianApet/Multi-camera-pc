#include "stdafx.h"
#include "OpticalFlow.h"

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2\gpu\gpu.hpp>



using namespace cv;
using namespace std;





static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, double, const Scalar& color)
{
	for (int y = 0; y < cflowmap.rows; y += step)
		for (int x = 0; x < cflowmap.cols; x += step)
		{
			const Point2f& fxy = flow.at<Point2f>(y, x);
			line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
				color);
			circle(cflowmap, Point(x, y), 2, color, -1);
		}
}


OpticalFlow::OpticalFlow()
{
}


OpticalFlow::~OpticalFlow()
{
}

void OpticalFlow::initOpticalFlow(vector<CString> LabelList)
{
	m_vectorNameList = LabelList;
	Scale_MotionFrm = 0.5;
	cflow_factor = 7;
	optMag = cflow_factor * Scale_MotionFrm;
}

void OpticalFlow::drawOptFlowMap_gpu(const Mat& flow_x, const Mat& flow_y, Mat& cflowmap, int step, const Scalar& color) {
	for (int y = 0; y < cflowmap.rows; y += step)
		for (int x = 0; x < cflowmap.cols; x += step)
		{
			Point2f fxy;
			fxy.x = cvRound(flow_x.at< float >(y, x) + x);
			fxy.y = cvRound(flow_y.at< float >(y, x) + y);

			line(cflowmap, Point(x, y), Point(fxy.x, fxy.y), color);
			circle(cflowmap, Point(fxy.x, fxy.y), 1, color, -1);
		}
}

TriggerArg OpticalFlow::getVectorFlow(Detector* pDetector, TriggerArg arg )
{

	Mat frame = arg.image.clone();
	if (frame.size().width >= 1080)
		cflow_factor = 20;
	if (frame.size().width < 1080)
		cflow_factor = 15;
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	if (!check_f)
	{
		
		frame_size = arg.image.size();
		h = frame_size.height;
		w = frame_size.width;
		videoSize = Size(w*Scale_MotionFrm, h*Scale_MotionFrm);
		seg_mask = Mat(h, w, CV_32FC1, Scalar(0, 0, 0));
		check_f = true;
	}
	
	resize(frame, frame, videoSize, 0, 0, INTER_CUBIC);
	cvtColor(frame, gray, COLOR_BGR2GRAY);

	if (!prevgray.empty())
	{

		Mat xy[2];

		calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
		cvtColor(prevgray, cflow, COLOR_GRAY2BGR);

		uflow.copyTo(flow);

		drawOptFlowMap(flow, cflow, cflow_factor * Scale_MotionFrm, 1.5, Scalar(0, 255, 0));
		//cv::imshow("flow", cflow);
		split(flow, xy);

		//calculate angle and magnitude
		Mat magnitude, angle;
		cv::cartToPolar(xy[0], xy[1], magnitude, angle, true);


		cv::threshold(magnitude, res, optMag, 100, CV_THRESH_BINARY);
		//imshow("flow magnitude", res);

		Mat res2;
		res.convertTo(res2, CV_8UC1, 255);

		// Create a structuring element (SE) and do some morphological operation in order to close holes, get unified connected components
		int morph_size = 2;
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
		morphologyEx(res2, res2, MORPH_CLOSE, element, Point(-1, -1), 12);

		arg = findBlobs(res2, frame, frameCount, arg, pDetector);
		
	}

	std::swap(prevgray, gray);
	//std::swap(lastFrame, frame);
	//frameCount++;
	

	return arg;
}

TriggerArg OpticalFlow::findBlobs(Mat src, Mat original, int frameCount, TriggerArg arg, Detector* pDetector)
{

	//Mat Classic_Img(Old_frame.size().width, Old_frame.size().height, CV_8UC3, Scalar(0, 0, 0));
	
	Mat dst;
	cvtColor(src, dst, CV_GRAY2BGR);

	//wifi passwd - c10c5b03

	vector<vector<Point>> contours; // storing contour
	vector<Vec4i> hierarchy;

	findContours(src.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<double> areas(contours.size());
	

	for (int i = 0; i < contours.size(); i++) {
		areas[i] = contourArea(Mat(contours[i]));
		if (areas[i] > 600 * Scale_MotionFrm * Scale_MotionFrm)
		{
			Rect rect1 = boundingRect(contours[i]);
			

			//Below was to store the ROI in the project directory !
			//Mat ROI_Motion = original(rect1);
			Rect scale_rect;
			Rect label_bg;
			scale_rect.x = int(rect1.x / Scale_MotionFrm);
			scale_rect.y = int(rect1.y / Scale_MotionFrm);
			
			scale_rect.width = int(rect1.width / Scale_MotionFrm);
			scale_rect.height = int(rect1.height / Scale_MotionFrm);
			//comment 7-18
			
			
			
			/////////////////////////
			if (scale_rect.x > arg.image.size().width)
			{
				scale_rect.x = arg.image.size().width;
				scale_rect.width = scale_rect.width - (scale_rect.x - arg.image.size().width);
			}
			if (scale_rect.y > arg.image.size().height)
			{
				scale_rect.y = arg.image.size().height;
				scale_rect.height = scale_rect.height - (scale_rect.x - arg.image.size().height);
			}
			Rect forCrop;
			if (scale_rect.x > 50)
			{
				forCrop.x = scale_rect.x - 50;

			}
			else
			{
				forCrop.x = 0;
			}
			if (scale_rect.y > 50)
			{
				forCrop.y = scale_rect.y - 50;
			}
			else
			{
				forCrop.y = 0;
			}

			forCrop.height = scale_rect.height + 90;
			forCrop.width = scale_rect.width + 90;
			if (forCrop.width + forCrop.x > arg.image.size().width)
			{
				forCrop.width = scale_rect.width + (forCrop.x + forCrop.width - arg.image.size().width);
			}
			
			if (forCrop.y + forCrop.height > arg.image.size().height)
			{
				forCrop.height = scale_rect.height + (forCrop.y + forCrop.height - arg.image.size().height);
			}
			try {
				Mat Crop_Img(arg.image, forCrop);
				//scale_rect = forCrop;
				/*arg = getYOLOMat(Crop_Img, arg, pDetector, scale_rect);*/
				arg = getYOLOMat(Crop_Img, arg, pDetector, scale_rect);
				rectangle(arg.image, scale_rect, CV_RGB(255, 255, 0), 2, 5);
			}
			catch (Exception e)
			{
				
			}
			
			
		}
	}
	
	//imshow("Blobs Rect", dst);
	return arg;

}

TriggerArg OpticalFlow::getYOLOMat(Mat CropImg, TriggerArg arg, Detector* pDetector, Rect scale_rect)
{
	try
	{
		vector<bbox_t> result;
		/*int zoom = 3;*/
		/*Mat ZoomInImg;
		resize(CropImg, ZoomInImg, Size((CropImg.size().width * zoom), (CropImg.size().height * zoom)));*/
		result = pDetector->detect(CropImg);
		int object_id;
		float max_prob = 0.00;
		CString Label;
		string lab;
		Point p(scale_rect.x + 1, scale_rect.y - 5);
		Rect label_bg;
		/*String fileName = to_string(frameCount) + ".jpg";
		imwrite(fileName, arg.image);*/
		for (int j = 0; j<result.size(); j++)
		{
			if (result[j].obj_id == 0 || result[j].obj_id == 2 || result[j].obj_id == 7)
			{
				if (result[j].prob >= 0.1)
				{
					arg.trigger = 1;
					if (max_prob < result[j].prob)
					{
						max_prob = result[j].prob;
						object_id = result[j].obj_id;
						Label = m_vectorNameList[object_id];
						const WCHAR * pszFoo = (const WCHAR *)Label;
						char	strMultibyte[256] = { 0, };
						int len = WideCharToMultiByte(CP_ACP, 0, pszFoo, -1, NULL, 0, NULL, NULL);
						WideCharToMultiByte(CP_ACP, 0, pszFoo, -1, strMultibyte, len, NULL, NULL);
						string object_label(strMultibyte);
						lab = object_label;
					}

				}

			}

		}
		int label_len = lab.length();
		label_bg.width = 10 * label_len;
		label_bg.height = 15;
		label_bg.x = scale_rect.x;
		label_bg.y = scale_rect.y - label_bg.height;
		rectangle(arg.image, label_bg, CV_RGB(255, 255, 0), -1, CV_AA, 0);
		putText(arg.image, lab,p, FONT_HERSHEY_COMPLEX_SMALL, 0.5, CV_RGB(0, 0, 0), 1, CV_AA);


	}
	catch (exception e)
	{
		String fileName = to_string(frameCount) + ".jpg";
		/*imwrite(fileName, Classic_Img);*/
	}
	
	return arg;
}
