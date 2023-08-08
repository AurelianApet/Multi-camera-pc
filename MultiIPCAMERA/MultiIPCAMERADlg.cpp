
// MultiIPCAMERADlg.cpp : implementation file
//
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <io.h>
#include <regex>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>
#include "stdafx.h"
#include "MultiIPCAMERA.h"
#include "MultiIPCAMERADlg.h"
#include "afxdialogex.h"
#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\video\background_segm.hpp"
#include "opencv2\gpu\gpu.hpp"
#include "opencv\cv.h"
#include "opencv2\legacy\legacy.hpp"
#include "AddCameraDlg.h"
#include "yolo_v2_class.h"
#include "LibraryClass.h"
#include "MSettingsDlg.h"
#include "ReportDlg.h"
#include "SummaryDlg.h"
#include "OpticalFlow.h"
#include <Windows.h>
#include <iomanip>
#pragma warning(disable : 4996)
#include<conio.h>  // remove this line if not using Windows OS
#define SHOW_STEPS // un-comment | comment this line to show steps or not
// const global variables
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

#include <wininet.h> 
#pragma comment(lib, "Wininet")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace cv;
using namespace std;
#include "Blob.h"

// function prototypes
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
bool checkIfBlobsCrossedTheLineRight(int cameraNum, SYSTEMTIME old_time, float rate, std::vector<Blob> &blobs, int &carCountRight, int param, int &tmpPrevx1, int &tmpPrevy1, int &tmpCurx1, int &tmpCury1, float w_ratio, float h_ratio, Rect rect);
bool checkIfBlobsCrossedTheLineLeft(int cameraNum, SYSTEMTIME old_time, float rate, std::vector<Blob> &blobs, int &carCountLeft, int param, int &tmpPrevx, int &tmpPrevy, int &tmpCurx, int &tmpCury, float w_ratio, float h_ratio, Rect rect);
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
void drawCarCountOnImage(cv::Mat &imgFrame2Copy, int param);

using namespace std::chrono;
// global variables
std::stringstream date;
int carCountLeft[4] = { 0, 0, 0, 0 }, intVerticalLinePosition, carCountRight[4] = { 0, 0, 0, 0 }, 
	tmpPrevx[4] = { 0, 0, 0, 0 }, tmpPrevy[4] = { 0, 0, 0, 0 }, tmpCurx[4] = { 0, 0, 0, 0 }, tmpCury[4] = { 0, 0, 0, 0 }, // left
	tmpPrevx1[4] = { 0, 0, 0, 0 }, tmpPrevy1[4] = { 0, 0, 0, 0 }, tmpCurx1[4] = { 0, 0, 0, 0 }, tmpCury1[4] = { 0, 0, 0, 0 };	//right

ofstream myfile;
ofstream testfile;

CString g_curState = _T("");
vector<CString> m_vectorNameList;
// CAboutDlg dialog used for App About

string cfg_file = "yolo.cfg";				  //yolo model cfg file 
string weight_file = "yolo.weights";	      //yolo model 	weights file 	

int		RtspCount; //rtsp thread count;
bool	g_bThreadStop[9];
bool	g_bThreadStop_full;
float colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
//Detector *pDetector = new Detector(cfg_file, weight_file);
ThreadRTSPArg RTSPPARAM[9];												//camera of properties array
ThreadRTSPArg RTSPPARAM_FULL;											//full mode rtsp param
bool RTSPThread_USED;
CRITICAL_SECTION g_cs;

int conf_threshold1;							//threshold of mask
int conf_threshold2;							//threshold of mask
int conf_threshold3;							//threshold of mask
int conf_threshold4;							//threshold of mask

int motion_fps1;
int motion_fps2;
int motion_fps3;
int motion_fps4;
CString conf_ftpserver;						//ftp server of settings
CString conf_ftpuser;						//ftp user of settings
CString conf_ftppass;						//ftp password of settings

int skip_frames1;
int skip_frames2;
int skip_frames3;
int skip_frames4;

int rate1;
int rate2;
int rate3;
int rate4;
FileUploadArg g_FPARAM[24];                 //file upload param for upload thread
int g_FPARAM_index;
bool g_own_file = false;

//Form rect
RECT	m_rectCurHist; // form h,y x, y
int		full_width;
int		full_height;

//full screen camera variables
int g_selected_idx;
bool g_full_check = false;
bool g_selection_tool = false;
bool sel_rightline = false;
bool sel_leftline = false;

//ratio of resize image
float ratio_mask = 0.3;

//for selection tools variables
bool destroy;
CvRect box;

cv::Point leftLine[4][2];
cv::Point rightLine[4][2];

IplImage* image;
bool drawing_box[4];
bool drawing_left_line[4];
bool drawing_right_line[4];

class CAboutDlg : public CDialogEx
{
public:
//	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//Destory all threads of cameras
void kill_All_Cams()
{
	for (int i = 0; i < RtspCount; i++)
	{
		g_bThreadStop[i] = true;
	}
	Sleep(500);
}

void draw_box(IplImage* img, CvRect rect)
{
	cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height),
		cvScalar(0, 0, 255), 2);

	CvRect rect2 = cvRect(box.x, box.y, box.width, box.height);
	cvSetImageROI(image, rect2);   //here I wanted to set the drawn rect as ROI
}

void fullScreen(int param)
{
	CMultiIPCAMERADlg* pthisDlg = (CMultiIPCAMERADlg*)theApp.m_pMainWnd;
	kill_All_Cams();
	if (!g_full_check)
		g_selected_idx = (int)param;
	g_full_check = !g_full_check;
	if (g_full_check)
	{
		g_bThreadStop_full = false;
	}
	else
	{
		g_bThreadStop_full = true;
		pthisDlg->overwriteFile();
	}
		
	if (RTSPPARAM[g_selected_idx].ROI_rect_check)
	{
		box = RTSPPARAM[g_selected_idx].ROI_rect;
	}
	else 
	{
		box = Rect(0, 0, 0, 0);
	}
	
	if (pthisDlg)
		pthisDlg->DisplayFull(param);
}

void refreshFullScreen()
{
	CMultiIPCAMERADlg* pthisDlg = (CMultiIPCAMERADlg*)theApp.m_pMainWnd;
	g_bThreadStop_full = true;
	RTSPPARAM[g_selected_idx].leftPoint[0] = leftLine[g_selected_idx][0];
	RTSPPARAM[g_selected_idx].leftPoint[1] = leftLine[g_selected_idx][1];
	RTSPPARAM[g_selected_idx].rightPoint[0] = rightLine[g_selected_idx][0];
	RTSPPARAM[g_selected_idx].rightPoint[1] = rightLine[g_selected_idx][1];

	Sleep(500);
	g_bThreadStop_full = false;
	::AfxBeginThread(pthisDlg->displayRTSP, (LPVOID)&RTSPPARAM_FULL);
}
//multi cameras have mouse click event
void CallBackCamsFunc(int event, int x, int y, int flags, void* param)
{	
	switch (event) {
	case EVENT_LBUTTONDBLCLK:
	{
		fullScreen(int(param));
		break;
	}
	case CV_EVENT_MOUSEMOVE:
	{
		if (g_full_check && g_selection_tool)
		{
			if (drawing_box[g_selected_idx])
			{
				box.width = x - box.x;
				box.height = y - box.y;
			}
		}
		if (sel_leftline && g_full_check) {
			if (drawing_left_line[g_selected_idx])
			{
				leftLine[g_selected_idx][1].x = x;
				if (leftLine[g_selected_idx][0].x < 0)
					leftLine[g_selected_idx][0].x = 0;
				leftLine[g_selected_idx][1].y = y;
				if (leftLine[g_selected_idx][0].y < 0)
					leftLine[g_selected_idx][0].y = 0;
			}
		}
		if (sel_rightline && g_full_check) {
			if (drawing_right_line[g_selected_idx]) {
				rightLine[g_selected_idx][1].x = x;
				if (rightLine[g_selected_idx][0].x < 0)
					rightLine[g_selected_idx][0].x = 0;
				rightLine[g_selected_idx][1].y = y;
				if (rightLine[g_selected_idx][0].y < 0)
					rightLine[g_selected_idx][0].y = 0;
			}
		}
		break;
	}
	case CV_EVENT_LBUTTONDOWN: {
//		OutputDebugString(str);
		if (g_full_check && g_selection_tool)
		{
			drawing_box[g_selected_idx] = true;
			box = cvRect(x, y, 0, 0);
		}
		if (g_full_check && sel_leftline) {
			drawing_left_line[g_selected_idx] = true;
			leftLine[g_selected_idx][0].x = x;
			if (leftLine[g_selected_idx][0].x < 0)
				leftLine[g_selected_idx][0].x = 0;
			leftLine[g_selected_idx][0].y = y;
			if (leftLine[g_selected_idx][0].y < 0)
				leftLine[g_selected_idx][0].y = 0;
		}
		if (g_full_check && sel_rightline) {
			drawing_right_line[g_selected_idx] = true;
			rightLine[g_selected_idx][0].x = x;
			if (rightLine[g_selected_idx][0].x < 0)
				rightLine[g_selected_idx][0].x = 0;
			rightLine[g_selected_idx][0].y = y;
			if (rightLine[g_selected_idx][0].y < 0)
				rightLine[g_selected_idx][0].y = 0;
		}
		break;
	}
	case CV_EVENT_LBUTTONUP:
	{
		if (g_full_check && g_selection_tool)
		{
			drawing_box[g_selected_idx] = false;
			if (box.width < 0)
			{
				box.x += box.width;
				box.width *= -1;
			}
			if (box.height < 0)
			{
				box.y += box.height;
				box.height *= -1;
			}
			RTSPPARAM[g_selected_idx].ROI_rect = box;
			RTSPPARAM[g_selected_idx].ROI_rect_check = true;
			RTSPPARAM_FULL.ROI_rect = box;
			RTSPPARAM_FULL.ROI_rect_check = true;
			g_selection_tool = false;
			sel_leftline = false;
			sel_rightline = false;
			//AfxMessageBox(_T("sss"));
			//draw_box(image, box);
			refreshFullScreen();
		}
		if (g_full_check && sel_leftline)
		{
			RTSPPARAM[g_selected_idx].leftline_check = true;
			RTSPPARAM_FULL.leftline_check = true;
			drawing_left_line[g_selected_idx] = false;
			g_selection_tool = false;
			sel_leftline = false;
			sel_rightline = false;
			refreshFullScreen();
		}
		if (g_full_check && sel_rightline)
		{
			RTSPPARAM[g_selected_idx].rightline_check = true;
			RTSPPARAM_FULL.rightline_check = true;
			drawing_right_line[g_selected_idx] = false;
			g_selection_tool = false;
			sel_leftline = false;
			sel_rightline = false;
			refreshFullScreen();
		}
		break;
	}
	case CV_EVENT_RBUTTONUP:
		destroy = true;
		break;
	}
}
//get file name from path
std::vector<std::string> splitpath(const std::string& str, const std::set<char> delimiters)
{
	std::vector<std::string> result;

	char const* pch = str.c_str();
	char const* start = pch;
	for (; *pch; ++pch)
	{
		if (delimiters.find(*pch) != delimiters.end())
		{
			if (start != pch)
			{
				std::string str(start, pch);
				result.push_back(str);
			}
			else
			{
				result.push_back("");
			}
			start = pch + 1;
		}
	}
	result.push_back(start);

	return result;
}

//extact ip from rstp url
std::string CMultiIPCAMERADlg::extractIP(cv::String rtspURL)
{
	char szIP[128];
	const char* ipStart = strstr(rtspURL.c_str(), "rtsp://");
	if (ipStart == NULL)
	{
		return "";
	}
	else
	{
		ipStart += 7;
		const char* ipEnd = strstr(ipStart, "/");
		if (ipEnd == NULL)
			strcpy(szIP, ipStart);
		else
			strncpy(szIP, ipStart, ipEnd - ipStart);

		return szIP;
	}
}

void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {
	for (auto &existingBlob : existingBlobs) {
		existingBlob.blnCurrentMatchFoundOrNewBlob = false;
		existingBlob.predictNextPosition();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {
		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {

			if (existingBlobs[i].blnStillBeingTracked == true) {
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}
	}

	for (auto &existingBlob : existingBlobs) {
		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}
		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlob.blnStillBeingTracked = false;
		}
	}
}


void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {
	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}


void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {
	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
	existingBlobs.push_back(currentFrameBlob);
}


double distanceBetweenPoints(cv::Point point1, cv::Point point2) {
	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}


void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
	cv::imshow(strImageName, image);
}


void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);
	std::vector<std::vector<cv::Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);
	cv::imshow(strImageName, image);
}

float leftSpeed[4] = { 0, 0, 0, 0 }, rightSpeed[4] = { 0, 0, 0, 0 };
bool checkIfBlobsCrossedTheLineRight(int cameraNum, SYSTEMTIME old_time, float rate, std::vector<Blob> &blobs, int &carCountRight, int param,
	int &tmpPrevx1, int &tmpPrevy1, int &tmpCurx1, int &tmpCury1, float w_ratio, float h_ratio, Rect rect) {
	if (w_ratio == 0 || h_ratio == 0)
		return false;
	if (param >= 100) {
		param -= 100;
	}
	if (cameraNum >= 100) {
		cameraNum -= 100;
		w_ratio = 1; h_ratio = 1;
	}
	int intHorizontalLinePosition = (rightLine[param][0].y + rightLine[param][1].y) / 2 * h_ratio;
	int intVerticalLinePosition = (rightLine[param][0].x + rightLine[param][1].x) / 2 * w_ratio;
	bool blnAtLeastOneBlobCrossedTheLine = false;
	SYSTEMTIME new_time;
	WORD oldtime;
	for (auto blob : blobs) {
		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;
			if (rect.width != 0 && rect.height != 0)
			{
				int rectxMin = rect.x * w_ratio;
				int rectxMax = (rect.x + rect.width);
				int rectyMin = rect.y * h_ratio;
				int rectyMax = (rect.y + rect.height);
				if (blob.centerPositions[currFrameIndex].y < rectyMin || blob.centerPositions[currFrameIndex].y > rectyMax 
					/*|| blob.centerPositions[currFrameIndex].x > rectxMin || blob.centerPositions[currFrameIndex].x < rectxMax*/) {
					continue;
				}
			}
			// Left way
			if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition/* && blob.centerPositions[currFrameIndex].x > 350*/) {
				if (tmpPrevx1 == blob.centerPositions[prevFrameIndex].x && tmpCurx1 == blob.centerPositions[currFrameIndex].x
					|| tmpPrevy1 == blob.centerPositions[prevFrameIndex].y && tmpCury1 == blob.centerPositions[currFrameIndex].y)
					continue;
				float size_h = (float)(blob.currentBoundingRect.y) / (float)(blob.currentBoundingRect.height + blob.currentBoundingRect.y);
//				if (size_h < 0.1)
//					continue;
				float size_w = (float)(blob.currentBoundingRect.x) / (float)(blob.currentBoundingRect.width + blob.currentBoundingRect.x);
//				if (size_w < 0.5)
//					continue;
				carCountRight++;
				blnAtLeastOneBlobCrossedTheLine = true;
				GetSystemTime(&new_time);
				oldtime = (old_time.wSecond * 1000) + old_time.wMilliseconds;
				WORD newtime = (new_time.wSecond * 1000) + new_time.wMilliseconds;
				float deltaTime = (float)(std::abs(newtime - oldtime));
				if (deltaTime == 0)
					continue;
				float speed = rate / deltaTime * 1000;
				rightSpeed[param] = speed;
				CTime now1 = CTime::GetCurrentTime();
				int y = now1.GetYear();
				int m = now1.GetMonth();
				int d = now1.GetDay();
				int h = now1.GetHour();
				int mm = now1.GetMinute();
				int s = now1.GetSecond();
				myfile << cameraNum + 1 << ", 0, 1, " << speed << ", " << y << ":" << m << ":" << d
					<< "*" << h << ":" << mm << ":" << s << ":" << new_time.wMilliseconds << ", " << size_w << "," << size_h << "," << blob.centerPositions.size() << ",\n";
				/*
				", 0, 1, "
				first param
				0-vehicle
				1-human
				second param
				0-left
				1-right
				*/
				old_time = new_time;
				tmpPrevx1 = blob.centerPositions[prevFrameIndex].x;
				tmpPrevy1 = blob.centerPositions[prevFrameIndex].y;
				tmpCurx1 = blob.centerPositions[currFrameIndex].x;
				tmpCury1 = blob.centerPositions[currFrameIndex].y;
			}
		}
	}
	return blnAtLeastOneBlobCrossedTheLine;
}

bool checkIfBlobsCrossedTheLineLeft(int cameraNum, SYSTEMTIME old_time, float rate, std::vector<Blob> &blobs,int &carCountLeft, int param,
	int &tmpPrevx, int &tmpPrevy, int &tmpCurx, int &tmpCury, float w_ratio, float h_ratio, Rect rect) {
	if (w_ratio == 0 || h_ratio == 0)
		return false;
	if (param >= 100)
		param -= 100;
	if (cameraNum >= 100) {
		cameraNum -= 100;
		w_ratio = 1; h_ratio = 1;
	}
	int intHorizontalLinePosition = (leftLine[param][0].y + leftLine[param][1].y) / 2 * h_ratio;
	int intVerticalLinePosition = (leftLine[param][0].x + leftLine[param][1].x) / 2 * w_ratio;
	bool blnAtLeastOneBlobCrossedTheLineLeft = false;
	SYSTEMTIME new_time;
	WORD oldtime;
	for (auto blob : blobs) {
		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;
			int rectxMin = rect.x;
			int rectxMax = (rect.x + rect.width);
			int rectyMin = rect.y;
			int rectyMax = (rect.y + rect.height);
			if (rect.width != 0 && rect.height != 0)
			{
				if (blob.centerPositions[currFrameIndex].y < rectyMin || blob.centerPositions[currFrameIndex].y > rectyMax 
					/*|| blob.centerPositions[currFrameIndex].x > rectxMin || blob.centerPositions[currFrameIndex].x < rectxMax*/) {
					continue;
				}
			}
			// Left way
			if (blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y > intHorizontalLinePosition
				/* && blob.centerPositions[currFrameIndex].x < 350 && blob.centerPositions[currFrameIndex].x > 0*/) {
				if (tmpPrevx == blob.centerPositions[prevFrameIndex].x && tmpCurx == blob.centerPositions[currFrameIndex].x
					|| tmpPrevy == blob.centerPositions[prevFrameIndex].y && tmpCury == blob.centerPositions[currFrameIndex].y)
					continue;
				float size_h = (float)(blob.currentBoundingRect.y) / (float)(blob.currentBoundingRect.height + blob.currentBoundingRect.y);
//				if (size_h < 0.1)
//					continue;
				float size_w = (float)(blob.currentBoundingRect.x) / (float)(blob.currentBoundingRect.width + blob.currentBoundingRect.x);
//				if (size_w < 0.5)
//					continue;
				carCountLeft++;
				blnAtLeastOneBlobCrossedTheLineLeft = true;
				GetSystemTime(&new_time);
				oldtime = (old_time.wSecond * 1000) + old_time.wMilliseconds;
				WORD newtime = (new_time.wSecond * 1000) + new_time.wMilliseconds;
				float deltaTime = (float)(std::abs(newtime - oldtime));
				if (deltaTime == 0)
					continue;
				float speed = rate / deltaTime * 1000;
				leftSpeed[param] = speed;
				CTime now1 = CTime::GetCurrentTime();
				int y = now1.GetYear();
				int m = now1.GetMonth();
				int d = now1.GetDay();
				int h = now1.GetHour();
				int mm = now1.GetMinute();
				int s = now1.GetSecond();
				myfile << cameraNum + 1 << ", 0, 0, " << speed << ", " << y << ":" << m << ":" << d
					<< "*" << h << ":" << mm << ":" << s << ":" << new_time.wMilliseconds << ", " << size_w << "," << size_h << "," << blob.centerPositions.size() << ",\n";
				old_time = new_time;
				tmpPrevx = blob.centerPositions[prevFrameIndex].x;
				tmpPrevy = blob.centerPositions[prevFrameIndex].y;
				tmpCurx = blob.centerPositions[currFrameIndex].x;
				tmpCury = blob.centerPositions[currFrameIndex].y;
			}
		}
	}
	return blnAtLeastOneBlobCrossedTheLineLeft;
}

void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {
	for (unsigned int i = 0; i < blobs.size(); i++) {
		if (blobs[i].blnStillBeingTracked == true) {
			float size_h = (float)(blobs[i].currentBoundingRect.y) / (float)(blobs[i].currentBoundingRect.height + blobs[i].currentBoundingRect.y);
//			if (size_h < 0.1)
//				continue;
//			float size_w = (float)(blobs[i].currentBoundingRect.x) / (float)(blobs[i].currentBoundingRect.width + blobs[i].currentBoundingRect.x);
//			if (size_w < 0.5)
//				continue;
			cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);
			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
			int intFontThickness = (int)std::round(dblFontScale * 1.0);

			cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}

void drawCarCountOnImage(cv::Mat &imgFrame2Copy, int param) {
	if (param >= 100)
		param -= 100;
	int intFontFace = CV_FONT_HERSHEY_SCRIPT_COMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 450000.0;
	int intFontThickness = (int)std::round(dblFontScale * 2.5);

	// Right way
	cv::Size textSize = cv::getTextSize(std::to_string(carCountRight[param]), intFontFace, dblFontScale, intFontThickness, 0);
	cv::putText(imgFrame2Copy, "Vehicle count:" + std::to_string(carCountRight[param]), cv::Point(imgFrame2Copy.size().width * 0.6, 25), intFontFace, 1, SCALAR_RED, 1);

	if (rightSpeed[param] > 0)
	{
		cv::Size textSize2 = cv::getTextSize(std::to_string(carCountRight[param]), intFontFace, dblFontScale, intFontThickness, 0);
		cv::putText(imgFrame2Copy, "Speed:" + std::to_string(rightSpeed[param]) + "m/s", cv::Point(imgFrame2Copy.size().width* 0.6, 55), intFontFace, 1, SCALAR_GREEN, 1);
	}

	// Left way
	cv::Size textSize1 = cv::getTextSize(std::to_string(carCountLeft[param]), intFontFace, dblFontScale, intFontThickness, 0);
	cv::putText(imgFrame2Copy, "Vehicle count:" + std::to_string(carCountLeft[param]), cv::Point(imgFrame2Copy.size().width * 0.1, 25), intFontFace, 1, SCALAR_YELLOW, 1);

	if (leftSpeed > 0)
	{
		cv::Size textSize3 = cv::getTextSize(std::to_string(carCountLeft[param]), intFontFace, dblFontScale, intFontThickness, 0);
		cv::putText(imgFrame2Copy, "Speed:" + std::to_string(leftSpeed[param]) + "m/s", cv::Point(imgFrame2Copy.size().width * 0.1, 55), intFontFace, 1, SCALAR_GREEN, 1);
	}
}

//Thread of each Cameras process
UINT CMultiIPCAMERADlg::displayRTSP(LPVOID Param)
{
	SYSTEMTIME old_time;
	GetSystemTime(&old_time);
//	showSample();
	TriggerArg trigger_arg;
	trigger_arg.trigger = 0;
	int checkonline = 1;
	ThreadRTSPArg* PARAMS;      //declare structure of pointer
	PARAMS = (ThreadRTSPArg*)Param;
	int skip_frames = skip_frames1;
	float rate = rate1;
	switch (PARAMS->nThreadIndex)
	{
	case 0:
		skip_frames = skip_frames1;
		rate = rate1;
		break;
	case 1:
		skip_frames = skip_frames2;
		rate = rate2;
		break;
	case 2:
		skip_frames = skip_frames3;
		rate = rate3;
		break;
	case 3: 
		skip_frames = skip_frames4;
		rate = rate4;
		break;
	default:
		break;
	}
	// TODO: display rtsp stream
	int saved_frames = 24 * skip_frames;

	VideoCapture capture;
	string saved_videofile;
	string cameraip; // variable of camera ip from rtsp address
	capture.open(PARAMS->RTSP);
	//mouse event listener
	setMouseCallback(PARAMS->W, CallBackCamsFunc, (void*)(PARAMS->nThreadIndex));
	//camera IP
	std::string strIP = extractIP(PARAMS->RTSP);
	char chars[] = ":";	

	for (unsigned int i = 0; i < strlen(chars); ++i)
	{
		// you need include <algorithm> to use general algorithms like std::remove()
		strIP.erase(std::remove(strIP.begin(), strIP.end(), chars[i]), strIP.end());
	}
	if (strIP != "")
		cameraip = strIP;

	//create videowriter instance for save video
	LibraryClass lc;
	saved_videofile = lc.setVideoWriter(PARAMS->Width, PARAMS->Height, cameraip);

	int skipimage = 0;
	int totalframes = 0;
	int night = 0;
	//////////////////////////////////////////////////////////////////
	
	Mat image;
	Mat Show_Frame;
	vector<bbox_t> prev_result;
	vector<bbox_t> prev_result_1;
	vector<bbox_t> prev_result_2;
	bool init_prev_result_check = false;
	vector<bbox_t> result;
	//create BSFREAK Instance
	//BSFREAK BS_FREAK;
	OpticalFlow OP;
	OP.initOpticalFlow(m_vectorNameList);

	//blob_tracking
	cv::Mat imgFrame1;
	cv::Mat imgFrame2;
	std::vector<Blob> blobs;
//	cv::Point crossingLine[2];
//	cv::Point crossingLineLeft[2];
	capture.read(imgFrame1);
	capture.read(imgFrame2);
	//CONTROL LINE FOR CARCOUNT ~AREA1 (RIGHT WAY)

	int param = PARAMS->nThreadIndex;
	if (param >= 100)
		param -= 100;

	int intLeftHorizontalLinePosition = (leftLine[param][0].y + leftLine[param][1].y) / 2;
	int intRightHorizontalLinePosition = (rightLine[param][0].y + rightLine[param][1].y) / 2;

/*
	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);
	intHorizontalLinePosition = intHorizontalLinePosition*1.40;
	intVerticalLinePosition = (int)std::round((double)imgFrame1.cols * 0.35);
	crossingLine[0].x = 515;
	crossingLine[0].y = intHorizontalLinePosition;
	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;

	//CONTROL LINE FOR CARCOUNT ~AREA2 (LEFT WAY)
	crossingLineLeft[0].x = 0;
	crossingLineLeft[0].y = intHorizontalLinePosition;
	crossingLineLeft[1].x = 300;
	crossingLineLeft[1].y = intHorizontalLinePosition;
*/

	char chCheckForEscKey = 0;
	bool blnFirstFrame = true;
	int frameCount = 2;
	
	while (capture.isOpened() && chCheckForEscKey != 27)
	{
		bool process_frame_check = false;
		if (totalframes == 250) totalframes = 0;
		totalframes++;
		try 
		{
			int skipflag = 0;
			//check full screen rtsp stop thread flag
			if (PARAMS->nThreadIndex >= 100)
			{
				if (g_bThreadStop_full)
				{
					capture.release();
					break;
				}
			}
			else {
				//check splits screen rtsp stop thread flag
				if (g_bThreadStop[PARAMS->nThreadIndex])
				{
					capture.release();
					break;
				}
			}

			Mat croppedImage;
			Mat detect_for_image;
			Mat origin_image;
			try {
				capture >> image;
			}
			catch (Exception e)
			{
				capture.open(PARAMS->RTSP);
				continue;
			}
			/*if (image.channels() == 3)
			{
				night = 0;
			}
			else
			{
				night = 1;
			}
				*/
			if (image.empty())
			{
				if (PARAMS->nThreadIndex < 100)
				{
					RTSPPARAM[PARAMS->nThreadIndex].online_offline = false;
				}
				capture.open(PARAMS->RTSP);
				continue;
			}
			if (PARAMS->nThreadIndex < 100)
			{
				RTSPPARAM[PARAMS->nThreadIndex].online_offline = true;
			}
			if (skip_frames == 0)
			{
				process_frame_check = true;
				saved_frames = 24;
			}
			if (skip_frames == 1)
			{
				process_frame_check = true;
			}
			if (skip_frames != 0)
			{
				try 
				{
					if ((totalframes % skip_frames) == 1)
					{
						process_frame_check = true;
					}
				}
				catch(Exception e)
				{
					process_frame_check = true;
				}
			}
			if (process_frame_check)
			{
				capture.read(imgFrame1);
				capture.read(imgFrame2);
				resize(imgFrame1, imgFrame1, Size(PARAMS->Width, PARAMS->Height));
				resize(imgFrame2, imgFrame2, Size(PARAMS->Width, PARAMS->Height));
				float w_ratio = (float)imgFrame2.size().width / full_width;
				float h_ratio = (float)imgFrame2.size().height / full_height;
				//check ROI rec and if exist replace image as ROI image
				Rect ROI_v_rect;
				if (PARAMS->ROI_rect_check)
				{
					ROI_v_rect.width = (int)PARAMS->ROI_rect.width * w_ratio;
					ROI_v_rect.x = (int)PARAMS->ROI_rect.x * w_ratio;
					ROI_v_rect.height = (int)PARAMS->ROI_rect.height * h_ratio;
					ROI_v_rect.y = (int)PARAMS->ROI_rect.y * h_ratio;
				}

				//blob_tracking
				std::vector<Blob> currentFrameBlobs;
				cv::Mat imgFrame1Copy = imgFrame1.clone();
				cv::Mat imgFrame2Copy = imgFrame2.clone();
				cv::Mat imgDifference;
				cv::Mat imgThresh;
				cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
				cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);
				cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
				cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);
				cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);
				cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);
//				cv::imshow("imgThresh", imgThresh);
				cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
				cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
				cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
				cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

				for (unsigned int i = 0; i < 2; i++) {
					cv::dilate(imgThresh, imgThresh, structuringElement5x5);
					cv::dilate(imgThresh, imgThresh, structuringElement5x5);
					cv::erode(imgThresh, imgThresh, structuringElement5x5);
				}

				cv::Mat imgThreshCopy = imgThresh.clone();
				std::vector<std::vector<cv::Point> > contours;
				cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//				drawAndShowContours(imgThresh.size(), contours, "imgContours");
				std::vector<std::vector<cv::Point> > convexHulls(contours.size());
				for (unsigned int i = 0; i < contours.size(); i++) {
					cv::convexHull(contours[i], convexHulls[i]);
				}
//				drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");
				for (auto &convexHull : convexHulls) {
					Blob possibleBlob(convexHull);

					if (possibleBlob.currentBoundingRect.area() > 400 &&
						possibleBlob.dblCurrentAspectRatio > 0.2 &&
						possibleBlob.dblCurrentAspectRatio < 4.0 &&
						possibleBlob.currentBoundingRect.width > 30 &&
						possibleBlob.currentBoundingRect.height > 30 &&
						possibleBlob.dblCurrentDiagonalSize > 60.0 &&
						(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
						currentFrameBlobs.push_back(possibleBlob);
					}
				}
//				drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");
				if (blnFirstFrame == true) {
					for (auto &currentFrameBlob : currentFrameBlobs) {
						blobs.push_back(currentFrameBlob);
					}
				}
				else {
					matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
				}
//				drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");
				drawBlobInfoOnImage(blobs, imgFrame2Copy);
				
				// Check the rightWay
				bool blnAtLeastOneBlobCrossedTheLine = checkIfBlobsCrossedTheLineRight(PARAMS->nThreadIndex, old_time, rate, blobs, 
					carCountRight[param], PARAMS->nThreadIndex, tmpPrevx1[param], tmpPrevy1[param], tmpCurx1[param], tmpCury1[param], w_ratio, h_ratio, ROI_v_rect);

				// Check the leftWay
				bool blnAtLeastOneBlobCrossedTheLineLeft = checkIfBlobsCrossedTheLineLeft(PARAMS->nThreadIndex, old_time, rate, blobs, 
					carCountLeft[param], PARAMS->nThreadIndex, tmpPrevx[param], tmpPrevy[param], tmpCurx[param], tmpCury[param], w_ratio, h_ratio, ROI_v_rect);
				
				//rightWay
				if (blnAtLeastOneBlobCrossedTheLine == true) {
					if (!(rightLine[param][0].x == 0 && rightLine[param][0].y == 0
						&& rightLine[param][1].x == 0 && rightLine[param][1].y == 0))
					{
						if (g_full_check)
						{
							cv::line(imgFrame2, rightLine[param][0], rightLine[param][1], SCALAR_GREEN, 2);
						}
						else
						{
							cv::line(imgFrame2, cv::Point(rightLine[param][0].x * w_ratio, rightLine[param][0].y * h_ratio),
								cv::Point(rightLine[param][1].x * w_ratio, rightLine[param][1].y * h_ratio), SCALAR_GREEN, 2);
						}
					}
				}
				else if (blnAtLeastOneBlobCrossedTheLine == false) {
					if (!(rightLine[param][0].x == 0 && rightLine[param][0].y == 0
						&& rightLine[param][1].x == 0 && rightLine[param][1].y == 0))
					{
						if (g_full_check)
						{
							cv::line(imgFrame2, rightLine[param][0], rightLine[param][1], SCALAR_RED, 2);
						}
						else
						{
							cv::line(imgFrame2, cv::Point(rightLine[param][0].x * w_ratio, rightLine[param][0].y * h_ratio),
								cv::Point(rightLine[param][1].x * w_ratio, rightLine[param][1].y * h_ratio), SCALAR_RED, 2);
						}
					}
				}

				//leftway
				if (blnAtLeastOneBlobCrossedTheLineLeft == true) {
					if (!(leftLine[param][0].x == 0 && leftLine[param][0].y == 0
						&& leftLine[param][1].x == 0 && leftLine[param][1].y == 0))
					{
						if (g_full_check)
						{
							cv::line(imgFrame2, leftLine[param][0], leftLine[param][1], SCALAR_BLACK, 2);
						}
						else
						{
							cv::line(imgFrame2, cv::Point(leftLine[param][0].x * w_ratio, leftLine[param][0].y * h_ratio),
								cv::Point(leftLine[param][1].x * w_ratio, leftLine[param][1].y * h_ratio), SCALAR_BLACK, 2);
						}
					}
				}
				else if (blnAtLeastOneBlobCrossedTheLineLeft == false) {
					if (!(leftLine[param][0].x == 0 && leftLine[param][0].y == 0
						&& leftLine[param][1].x == 0 && leftLine[param][1].y == 0))
					{
						if (g_full_check)
						{
							cv::line(imgFrame2, leftLine[param][0], leftLine[param][1], SCALAR_YELLOW, 2);
						}
						else
						{
							cv::line(imgFrame2, cv::Point(leftLine[param][0].x * w_ratio, leftLine[param][0].y * h_ratio),
								cv::Point(leftLine[param][1].x * w_ratio, leftLine[param][1].y * h_ratio), SCALAR_YELLOW, 2);
						}
					}
				}
				detect_for_image = imgFrame2.clone();
				drawCarCountOnImage(detect_for_image, PARAMS->nThreadIndex);

				//				cv::imshow("imgFrame2Copy", imgFrame2Copy);
				//cv::waitKey(0);	// uncomment this line to go frame by frame for debugging        
				// now we prepare for the next iteration
				currentFrameBlobs.clear();
				imgFrame1 = detect_for_image.clone();	// move frame 1 up to where frame 2 is

				if ((capture.get(CV_CAP_PROP_POS_FRAMES) + 1) <= capture.get(CV_CAP_PROP_FRAME_COUNT)) {
					capture.read(imgFrame2);
				}
//				else {
//					break;
//				}

				blnFirstFrame = false;
				frameCount++;

//deeplearning
/*
				trigger_arg.image = detect_for_image;
				trigger_arg = OP.getVectorFlow(pDetector, trigger_arg);

				try 
				{
					if (PARAMS->ROI_rect_check)
					{
						trigger_arg.image.copyTo(image(ROI_v_rect));
					}
					else
					{
						image = trigger_arg.image;
					}
				}
				catch (Exception e)
				{

				}
*/

				if (g_full_check)
				{
					rectangle(detect_for_image, box, CV_RGB(255, 0, 255), 1);
				}
				else
				{
					rectangle(detect_for_image, ROI_v_rect, CV_RGB(255, 0, 255), 1);
				}
				resize(detect_for_image, detect_for_image, Size(PARAMS->Width, PARAMS->Height));
				imshow(PARAMS->W, detect_for_image);
			}

/*
			if (trigger_arg.trigger == 1)
			{
				if (skipimage != saved_frames)
				{
					skipimage++;
					lc.inputFrmToVideo(image);
				}
				else {
					init_prev_result_check = false;
					USES_CONVERSION;
					g_FPARAM[g_FPARAM_index].ftpserver = PARAMS->FTP_SERVER;
					g_FPARAM[g_FPARAM_index].usr = PARAMS->FTP_USER;
					g_FPARAM[g_FPARAM_index].pass = PARAMS->FTP_PASS;
					g_FPARAM[g_FPARAM_index].src = saved_videofile;
					//get dest filename from src path for upload
					std::set<char> delims{ '\\' };
					std::vector<std::string> path = splitpath(saved_videofile, delims);
					g_FPARAM[g_FPARAM_index].dst = path.back();
					saved_videofile = lc.setVideoWriter(PARAMS->Width, PARAMS->Height, cameraip);
					//upload Thread begings
					::AfxBeginThread(uploadVideoThread, (LPVOID)g_FPARAM_index);
					g_FPARAM_index++;
					if (g_FPARAM_index > 9)
						g_FPARAM_index = 0;

					skipimage = 0;
					trigger_arg.trigger = 0;
				}
			}
*/
			chCheckForEscKey = cv::waitKey(1);
		}
		catch (Exception e)
		{
//			AfxMessageBox(_T("streaming error?"));
		}
		cv::waitKey(30);
	}
	if(PARAMS->nThreadIndex < 100)
		RTSPPARAM[PARAMS->nThreadIndex].online_offline = false;
	return 0;
}

//string to wstring
std::wstring string_to_wstring(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

//upload motion video file Thread
UINT CMultiIPCAMERADlg::uploadVideoThread(LPVOID Param)
{
	//object detection and draw rectangle in image then show windows
	int index = (int)Param;
	//DWORD dwLastError = 0;
	FileUploadArg* FPARAM = &g_FPARAM[index];

	if (FPARAM == NULL)
	{
		g_curState = _T("Upload Process Error");
		return 0;
	}
	//string to wstring
	std::wstring wserver = string_to_wstring(FPARAM->ftpserver);
	std::wstring user = string_to_wstring(FPARAM->usr);
	std::wstring pass = string_to_wstring(FPARAM->pass);
	std::wstring src = string_to_wstring(FPARAM->src);
	std::wstring dst = string_to_wstring(FPARAM->dst);

	HINTERNET hInternet;
	HINTERNET hFtpSession;
	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	try {
		if (hInternet == NULL)
		{
			g_curState = _T("Internet error: Please check cable, hub");
			//AfxMessageBox(_T("Internet error: Please check cable, hub"));
		}
		else
		{
			hFtpSession = InternetConnect(hInternet, wserver.c_str(), INTERNET_DEFAULT_FTP_PORT, user.c_str(), pass.c_str(), INTERNET_SERVICE_FTP, 0, 0);
			if (hFtpSession == NULL)
			{
				g_curState.Format(_T("Ftp server error: Please check server, user, password \n %s : %s : %s"), FPARAM->ftpserver, FPARAM->usr, FPARAM->pass);
				//AfxMessageBox(_T("Ftp server error: Please check server, user, password"));
			}
			else
			{
				if (!FtpPutFile(hFtpSession, src.c_str(), dst.c_str(), FTP_TRANSFER_TYPE_BINARY, 0))
				{
					//AfxMessageBox(_T("File upload error"));	
					g_curState = _T("File upload error");
				}
				else
				{
					g_curState = _T("File upload success");
				}
				InternetCloseHandle(hFtpSession);
			}
			InternetCloseHandle(hInternet);
		}

	}
	catch (Exception e)
	{
		AfxMessageBox(_T("File upload error"));
	}
	return 0;
}

UINT CMultiIPCAMERADlg::threadStateCameralist(LPVOID Param)
{
	while (1)
	{
		CMultiIPCAMERADlg* pthisDlg = (CMultiIPCAMERADlg*)theApp.m_pMainWnd;
		pthisDlg->checkCameraStatue();
		Sleep(1500);
	}
	
	return 0;
}

UINT CMultiIPCAMERADlg::startProcess(LPVOID Param)
{
	while (1)
	{
		CMultiIPCAMERADlg* pthisDlg = (CMultiIPCAMERADlg*)theApp.m_pMainWnd;
		pthisDlg->OnBnClickedButton1();
		Sleep(3600000);
	}
	return 0;
}
// CMultiIPCAMERADlg dialog

CMultiIPCAMERADlg::CMultiIPCAMERADlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MULTIIPCAMERA_DIALOG, pParent)
	, m_upload_statue(_T(""))
	, m_motion_percentage(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SplitWCount = 0;
}

void CMultiIPCAMERADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERA2, var_camera2);
	DDX_Control(pDX, IDC_ADD_CAM, m_btnAddCamera);
	DDX_Control(pDX, IDC_LSTCAMERA, m_lstCamera);
	DDX_Control(pDX, IDC_BUTTON1, m_btnRefresh);
	DDX_Control(pDX, IDC_BTN_SETTINGS, m_btnSettings);
	DDX_Control(pDX, IDC_CAM5, camera5);
	DDX_Control(pDX, IDC_CAM6, camera6);
	DDX_Control(pDX, IDC_CAM7, camera7);
	DDX_Control(pDX, IDC_CAM8, camera8);
	DDX_Control(pDX, IDC_CAM9, camera9);
	DDX_Text(pDX, IDC_LABEL_UPLOAD, m_upload_statue);
	DDX_Control(pDX, IDC_BUTTON_FOUR, m_btnFour);
	DDX_Control(pDX, IDC_BUTTON_NINE, m_btnNine);
	DDX_Control(pDX, IDC_BTN_ROI_PEN, m_btn_roi_pen);
	DDX_Control(pDX, IDC_FULL_PIC, camera_full);
	DDX_Control(pDX, IDC_CAM3, camera3);
	DDX_Control(pDX, IDC_CAM4, camera4);
	DDX_Control(pDX, IDC_CME, camera1);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_btn_clear);
	DDX_Control(pDX, IDC_BUTTON_SPLIT, m_btn_split);
	DDX_Text(pDX, IDC_EDIT_DEFAULT_PERTG, m_motion_percentage);
	DDX_Control(pDX, IDC_STATIC_MOTION_PTG, m_static_ptg);
	DDX_Control(pDX, IDC_BUTTON_PTG_OK, c_btn_ptg_ok);
	DDX_Control(pDX, IDC_EDIT_DEFAULT_PERTG, c_ptg);
	//  DDX_Control(pDX, IDC_BUTTON4, m_leftLine);
	//  DDX_Control(pDX, IDC_BUTTON5, m_rightLine);
	DDX_Control(pDX, IDC_BTN_ROI_PEN2, m_leftLine);
	DDX_Control(pDX, IDC_BTN_ROI_PEN3, m_rightLine);
}

BEGIN_MESSAGE_MAP(CMultiIPCAMERADlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMultiIPCAMERADlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD_CAM, &CMultiIPCAMERADlg::OnBnClickedAddCam)
	ON_BN_CLICKED(IDC_BTN_SETTINGS, &CMultiIPCAMERADlg::OnBnClickedBtnSettings)
	ON_BN_CLICKED(IDC_BUTTON_FOUR, &CMultiIPCAMERADlg::OnBnClickedButtonFour)
	ON_BN_CLICKED(IDC_BUTTON_NINE, &CMultiIPCAMERADlg::OnBnClickedButtonNine)
	ON_STN_DBLCLK(IDC_FULL_PIC, &CMultiIPCAMERADlg::OnDblclkFullPic)
	ON_STN_DBLCLK(IDC_CME, &CMultiIPCAMERADlg::OnDblclkCme)
	ON_BN_DOUBLECLICKED(IDC_ADD_CAM, &CMultiIPCAMERADlg::OnDoubleclickedAddCam)
	ON_STN_CLICKED(IDC_CME, &CMultiIPCAMERADlg::OnStnClickedCme)
	ON_STN_DBLCLK(IDC_CAM3, &CMultiIPCAMERADlg::OnStnDblclickCam3)
	ON_BN_CLICKED(IDC_BTN_ROI_PEN, &CMultiIPCAMERADlg::OnBnClickedBtnRoiPen)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMultiIPCAMERADlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SPLIT, &CMultiIPCAMERADlg::OnBnClickedButtonSplit)
	ON_BN_CLICKED(IDC_BUTTON_PTG_OK, &CMultiIPCAMERADlg::OnBnClickedButtonPtgOk)
	ON_BN_CLICKED(IDC_BUTTON3, &CMultiIPCAMERADlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CMultiIPCAMERADlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_ROI_PEN2, &CMultiIPCAMERADlg::OnBnClickedBtnRoiPen2)
	ON_BN_CLICKED(IDC_BTN_ROI_PEN3, &CMultiIPCAMERADlg::OnBnClickedBtnRoiPen3)
END_MESSAGE_MAP()

// CMultiIPCAMERADlg message handlers

BOOL CMultiIPCAMERADlg::OnInitDialog()
{
	//set camera properties such as rtsp, w, width, height, x, y regarding each camera
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	InitializeCriticalSection(&g_cs);

	//time of interval reading statue message on taskbar 
	SetTimer(0, 2000, NULL);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	HIMAGELIST hList = ImageList_Create(32, 32, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_cImageListNormal.Attach(hList);
	hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_cImageListSmall.Attach(hList);
	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BITMAP2);
	m_cImageListNormal.Add(&cBmp, RGB(255, 0, 255));
	cBmp.DeleteObject();

	// Load the small icons
	cBmp.LoadBitmap(IDB_BITMAP3);
	m_cImageListSmall.Add(&cBmp, RGB(255, 0, 255));

	// Attach them
	m_lstCamera.SetImageList(&m_cImageListNormal, LVSIL_NORMAL);
	m_lstCamera.SetImageList(&m_cImageListSmall, LVSIL_SMALL);
	DWORD dwStyle = m_lstCamera.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_lstCamera.SetExtendedStyle(dwStyle);

	//insert coloums in camera list control
	CRect rect;
	m_lstCamera.GetClientRect(&rect);
	int nColInterval = rect.Width() / 5;
	m_lstCamera.InsertColumn(0, _T("Camera Name"), LVCFMT_LEFT, nColInterval * 5);
	m_lstCamera.InsertColumn(1, _T("Statue"), LVCFMT_LEFT, nColInterval * 3);

	// TODO: Add extra initialization here
	//init Add Camera button as CImageButton
	m_btnAddCamera.SetAlignStyle( CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btnAddCamera.SetButtonImage(IDB_BITMAP1, RGB(0, 0, 0));

	//init refresh button as CImageButton
	m_btnRefresh.SetAlignStyle( CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btnRefresh.SetButtonImage(IDB_BITMAP4, RGB(0, 0, 255));

	//init Settings button
	m_btnSettings.SetAlignStyle( CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btnSettings.SetButtonImage(IDB_BITMAP5, RGB(0, 0, 255));
	//4 split windows button
	m_btnFour.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btnFour.SetButtonImage(IDB_BITMAP6, RGB(0, 0, 255));
	//9 split windows button
	m_btnNine.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btnNine.SetButtonImage(IDB_BITMAP7, RGB(0, 0, 255));
	//ROI draw pen
	m_btn_roi_pen.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btn_roi_pen.SetButtonImage(IDB_BITMAP8, RGB(0, 0, 255));
	m_btn_clear.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btn_clear.SetButtonImage(IDB_BITMAP9, RGB(0, 0, 255));
	m_btn_split.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_btn_split.SetButtonImage(IDB_BITMAP10, RGB(0, 0, 255));

	m_leftLine.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_leftLine.SetButtonImage(IDB_BITMAP8, RGB(0, 0, 255));
	m_rightLine.SetAlignStyle(CImageButton::TEXT_INCLUDE | CImageButton::IMAGE_HCENTER);
	m_rightLine.SetButtonImage(IDB_BITMAP9, RGB(0, 0, 255));

	//inital camera windows count
	SplitWCount = 4;
	//init get and display camera list on form
	initlistCameras();
	//initial get properties of registered cameras
	getCamsProp(SplitWCount);
	//set ini file name
	strINIPath = "conf\\myini.ini";
	//check config
	checkSettings();
	camera_full.ShowWindow(SW_HIDE);
	if (SplitWCount == 4)
	{
		camera5.ShowWindow(SW_HIDE);
		camera6.ShowWindow(SW_HIDE);
		camera7.ShowWindow(SW_HIDE);
		camera8.ShowWindow(SW_HIDE);
		camera9.ShowWindow(SW_HIDE);
	}
	//refreshCams();
	getCamsProp(SplitWCount);
	//running camerastate list
	int list = 0;
	::AfxBeginThread(threadStateCameralist, (LPVOID)&list);
	//get full screen size
	/*this->GetClientRect(&m_rectCurHist);*/
	int list1 = 0;
	::AfxBeginThread(startProcess, (LPVOID)&list1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMultiIPCAMERADlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMultiIPCAMERADlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMultiIPCAMERADlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//detectable object name list
void CMultiIPCAMERADlg::ReadNameList()
{
	
	TCHAR szCurPath[MAX_PATH];
	GetModuleFileName(NULL, szCurPath, MAX_PATH);

	TCHAR* lastpath = _tcsrchr(szCurPath, _T('\\'));
	if (lastpath)
		lastpath[1] = 0;
	_tcscat(szCurPath, _T("data\\coco.names"));

	CStdioFile StdFile;
	CString sLine;

	if (StdFile.Open(szCurPath, CFile::modeNoTruncate | CFile::modeRead))
	{
		while (StdFile.ReadString(sLine))
		{
			m_vectorNameList.push_back(sLine);

		}
		StdFile.Close();
	}
}

void CMultiIPCAMERADlg::OnBnClickedButton1()
{
	ReadNameList();
	refreshCams();

	// TODO: Add your control notification handler code here
}

void CMultiIPCAMERADlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	//m_upload_statue = g_curState;
	//UpdateData(FALSE);
	
	CDialogEx::OnTimer(nIDEvent);
}

//display cameras which full or split mode
void CMultiIPCAMERADlg::DisplayFull(int param_id)
{
	displayPicControls();
	//ClearAllCamsWindows();
	if (g_full_check)
	{
		ProcFull(param_id);
	}
	else 
	{
		m_btn_roi_pen.ShowWindow(SW_HIDE);
		m_leftLine.ShowWindow(SW_HIDE);
		m_rightLine.ShowWindow(SW_HIDE);
		m_btn_clear.ShowWindow(SW_HIDE);
		m_btn_split.ShowWindow(SW_HIDE);
		m_static_ptg.ShowWindow(SW_HIDE);
		c_btn_ptg_ok.ShowWindow(SW_HIDE);
		c_ptg.ShowWindow(SW_HIDE);
		refreshCams();
	}	
}

void CMultiIPCAMERADlg::ProcFull(int param_id)
{	
	char* w = "IDC_STATIC_OUTPUT_FULL";
	CString percentage_buff;
	RTSPPARAM_FULL = RTSPPARAM[g_selected_idx];
	RTSPPARAM_FULL.W = w;
	RTSPPARAM_FULL.nThreadIndex = 100 + param_id;
	RTSPPARAM_FULL.Width = full_width;
	RTSPPARAM_FULL.Height = full_height;
	percentage_buff.Format(_T("%d"), RTSPPARAM_FULL.motion_percentage);
	m_motion_percentage = percentage_buff;
	//show tools button
	m_btn_roi_pen.ShowWindow(SW_SHOW);
	m_btn_clear.ShowWindow(SW_SHOW);
	m_btn_split.ShowWindow(SW_SHOW);
	m_leftLine.ShowWindow(SW_SHOW);
	m_rightLine.ShowWindow(SW_SHOW);
	m_static_ptg.ShowWindow(SW_SHOW);
	c_btn_ptg_ok.ShowWindow(SW_SHOW);
	c_ptg.ShowWindow(SW_SHOW);
	UpdateData(false);

	cvDestroyWindow(w);
	createCVWindows(IDC_FULL_PIC, m_rectCurHist.left + 200, m_rectCurHist.top + 100, full_width, full_height, w);
	::AfxBeginThread(displayRTSP, (LPVOID)&RTSPPARAM_FULL);
}

void CMultiIPCAMERADlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	DeleteCriticalSection(&g_cs);
}

//my customer global functions

//get camera list perpoties
void CMultiIPCAMERADlg::getCamsProp(int splitCount)
{
	this->GetClientRect(&m_rectCurHist);
	int width, height;

	// set camera list x,y,w,h
	CWnd *pWndCamList;
	pWndCamList = (CWnd*)GetDlgItem(IDC_LSTCAMERA);
	int CamList_X =  (int)m_rectCurHist.left + 10;
	int CamList_y = (int)m_rectCurHist.top + 100;
	int CamList_h = (int)(m_rectCurHist.bottom - m_rectCurHist.top) - 151;
	pWndCamList->MoveWindow(CamList_X, CamList_y, 180, CamList_h);
	
	if (splitCount == 4) {
		width = (int)(m_rectCurHist.right - m_rectCurHist.left - 220) / 2;
		height = (int)(m_rectCurHist.bottom - m_rectCurHist.top - 150) / 2;
		for (int i = 0; i < 4; i++)
		{
			RTSPPARAM[i].Width = width;
			RTSPPARAM[i].Height = height;
			int left_rest = 200;
			if (i == 0) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100);
				//camera 1 component
				createCVWindows(IDC_CME, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				//set YOLO arg
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT";

			}
			if (i == 1) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.5";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT1";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100);
				//camera 2 component
				createCVWindows(IDC_CAMERA2, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT1";
				
			}
			if (i == 2) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT3";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height);
				//camera 3 component
				createCVWindows(IDC_CAM3, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT3";
			}
			if (i == 3) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT4";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height);
				//camera 4 component
				createCVWindows(IDC_CAM4, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT4";
			}
			RTSPPARAM[i].YArg.nThreadIndex = i;
		}
	}
	if (splitCount == 9) {
		width = (int)(m_rectCurHist.right - m_rectCurHist.left - 220) / 3;
		height = (int)(m_rectCurHist.bottom - m_rectCurHist.top - 150) / 3;
		for (int i = 0; i < 9; i++)
		{
			RTSPPARAM[i].Width = width;
			RTSPPARAM[i].Height = height;
			int left_rest = 200;
			if (i == 0) {

				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100);
				//camera 1 component
				createCVWindows(IDC_CME, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT";

			}
			if (i == 1) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.5";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT1";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100);
				//camera 2 component
				createCVWindows(IDC_CAMERA2, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT1";

			}
			if (i == 2) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT3";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width * 2);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100);
				//camera 3 component
				createCVWindows(IDC_CAM3, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT3";
			}
			if (i == 3) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT4";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height);
				//camera 4 component
				createCVWindows(IDC_CAM4, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT4";
			}
			if (i == 4) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT5";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height);
				//camera 5 component
				createCVWindows(IDC_CAM5, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT5";
			}
			if (i == 5) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT6";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width * 2);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height);
				//camera 6 component
				createCVWindows(IDC_CAM6, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT6";
			}
			if (i == 6) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT7";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height * 2);
				//camera 6 component
				createCVWindows(IDC_CAM7, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT7";
			}
			if (i == 7) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT8";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height * 2);
				//camera 6 component
				createCVWindows(IDC_CAM8, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT8";
			}
			if (i == 8) {
				//RTSPPARAM[i].RTSP = "rtsp://admin:12345@192.168.1.6";
				RTSPPARAM[i].W = "IDC_STATIC_OUTPUT9";
				RTSPPARAM[i].X = (int)(m_rectCurHist.left + left_rest + width * 2);
				RTSPPARAM[i].Y = (int)(m_rectCurHist.top + 100 + height * 2);
				//camera 6 component
				createCVWindows(IDC_CAM9, RTSPPARAM[i].X, RTSPPARAM[i].Y, RTSPPARAM[i].Width, RTSPPARAM[i].Height, (char*)RTSPPARAM[i].W.c_str());
				RTSPPARAM[i].YArg.W = "IDC_STATIC_OUTPUT9";
			}
			//set YOLO arg
			RTSPPARAM[i].YArg.nThreadIndex = i;
		}
	}
}

//function of pic camera
void CMultiIPCAMERADlg::createCVWindows(int IDC, int x, int y, int width, int height, char* w)
{
	cvDestroyWindow(w);
	//camera 1 component
	CWnd *pWnd1;
	pWnd1 = (CWnd*)GetDlgItem(IDC);
	pWnd1->MoveWindow(x, y, width, height);
	cvNamedWindow(w, CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(w);
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
}

//add camera button click
void CMultiIPCAMERADlg::OnBnClickedAddCam()
{
	// TODO: Add your control notification handler code here
	CAddCameraDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		initlistCameras();
	}
}

//read camera list from camera.txt
void CMultiIPCAMERADlg::initlistCameras()
{
	leftSpeed[0] = 0; leftSpeed[1] = 0; leftSpeed[2] = 0; leftSpeed[3] = 0;
	rightSpeed[0] = 0; rightSpeed[1] = 0; rightSpeed[2] = 0; rightSpeed[3] = 0;
	carCountLeft[0] = 0; carCountLeft[1] = 0; carCountLeft[2] = 0; carCountLeft[3] = 0;
	carCountRight[0] = 0; carCountRight[1] = 0; carCountRight[2] = 0; carCountRight[3] = 0;

	//inital LIST_CAM that clear all
	myfile.open("config/result.csv", std::ios_base::app);
//	testfile.open("config/1.csv");
//	myfile << "camera, object, Direction, speed(m/s), datetime\n";
	//set list control with icons
	LVITEM lvi;
	CString nFilename = _T("conf/cameralist.txt");
	CStringArray aLineArray;
	CString sLine;
	int iLineCount = 0;
	CFileException ex;
	CStdioFile StdFile;
	if (StdFile.Open(nFilename, CFile::modeNoTruncate | CFile::modeRead, &ex))
	{
		VideoCapture checkVideoCaputure;
		while (StdFile.ReadString(sLine))
		{
			//aLineArray.Add(sLine);
			//split name and address in a line
			int nTokenPos = 0;
			CString name;
			CString address;
			CString ftp_server;
			CString ftp_user;
			CString ftp_pass;
			CString camera_statue;
			CString x, y, w, h;
			CString motion_percentage;
			CString lline1x, lline1y, lline2x, lline2y;
			CString rline1x, rline1y, rline2x, rline2y;
			AfxExtractSubString(name, sLine, 0, '>');
			AfxExtractSubString(address, sLine, 1, '>');
			AfxExtractSubString(ftp_server, sLine, 2, '>');
			AfxExtractSubString(ftp_user, sLine, 3, '>');
			AfxExtractSubString(ftp_pass, sLine, 4, '>');
			AfxExtractSubString(x, sLine, 5, '>');
			AfxExtractSubString(y, sLine, 6, '>');
			AfxExtractSubString(w, sLine, 7, '>');
			AfxExtractSubString(h, sLine, 8, '>');
			AfxExtractSubString(motion_percentage, sLine, 9, '>');
			AfxExtractSubString(lline1x, sLine, 10, '>');
			AfxExtractSubString(lline1y, sLine, 11, '>');
			AfxExtractSubString(lline2x, sLine, 12, '>');
			AfxExtractSubString(lline2y, sLine, 13, '>');
			AfxExtractSubString(rline1x, sLine, 14, '>');
			AfxExtractSubString(rline1y, sLine, 15, '>');
			AfxExtractSubString(rline2x, sLine, 16, '>');
			AfxExtractSubString(rline2y, sLine, 17, '>');

			//check camera statue
			CT2CA pszConvertedAnsiString(address);
			std::string strStd(pszConvertedAnsiString);
			camera_statue = _T("...");
			//set icon list 
			lvi.mask = LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = iLineCount;
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(name);
			lvi.iImage = 1;		// There are 8 images in the image list
			m_lstCamera.InsertItem(&lvi);

			// Set subitem 1
			lvi.iSubItem = 1;
			lvi.pszText = (LPTSTR)(LPCTSTR)(camera_statue);
			m_lstCamera.SetItem(&lvi);

			CT2CA pszName(name);
			std::string m_NameStd(pszName);
			CT2CA pszAddress(address);
			std::string m_AddressStd(pszAddress);
			CT2CA pszServer(ftp_server);
			std::string m_ServerStd(pszServer);
			CT2CA pszUser(ftp_user);
			std::string m_UserStd(pszUser);
			CT2CA pszPass(ftp_pass);
			std::string m_PassStd(pszPass);

			RTSPPARAM[iLineCount].name = (String)m_NameStd;
			RTSPPARAM[iLineCount].RTSP = (String)m_AddressStd;
			RTSPPARAM[iLineCount].FTP_SERVER = (String)m_ServerStd;
			RTSPPARAM[iLineCount].FTP_USER = (String)m_UserStd;
			RTSPPARAM[iLineCount].FTP_PASS = (String)m_PassStd;
			RTSPPARAM[iLineCount].ROI_rect = Rect(_ttoi(x), _ttoi(y), _ttoi(w), _ttoi(h));
			RTSPPARAM[iLineCount].motion_percentage = _ttoi(motion_percentage);
			RTSPPARAM[iLineCount].leftPoint[0].x = _ttoi(lline1x);
			RTSPPARAM[iLineCount].leftPoint[0].y = _ttoi(lline1y);
			RTSPPARAM[iLineCount].leftPoint[1].x = _ttoi(lline2x);
			RTSPPARAM[iLineCount].leftPoint[1].y = _ttoi(lline2y);
			RTSPPARAM[iLineCount].rightPoint[0].x = _ttoi(rline1x);
			RTSPPARAM[iLineCount].rightPoint[0].y = _ttoi(rline1y);
			RTSPPARAM[iLineCount].rightPoint[1].x = _ttoi(rline2x);
			RTSPPARAM[iLineCount].rightPoint[1].y = _ttoi(rline2y);
			leftLine[iLineCount][0].x = _ttoi(lline1x);
			leftLine[iLineCount][0].y = _ttoi(lline1y);
			leftLine[iLineCount][1].x = _ttoi(lline2x);
			leftLine[iLineCount][1].y = _ttoi(lline2y);
			rightLine[iLineCount][0].x = _ttoi(rline1x);
			rightLine[iLineCount][0].y = _ttoi(rline1y);
			rightLine[iLineCount][1].x = _ttoi(rline2x);
			rightLine[iLineCount][1].y = _ttoi(rline2y);

			if (_ttoi(w) > 0 && _ttoi(h) > 0)
				RTSPPARAM[iLineCount].ROI_rect_check = true;
			iLineCount++;
		}
		RtspCount = iLineCount;
		StdFile.Close();
	}
}

//refresh cameras
void CMultiIPCAMERADlg::refreshCams()
{
	/*
	leftSpeed[0] = 0; leftSpeed[1] = 0; leftSpeed[2] = 0; leftSpeed[3] = 0;
	rightSpeed[0] = 0; rightSpeed[1] = 0; rightSpeed[2] = 0; rightSpeed[3] = 0;
	carCountLeft[0] = 0; carCountLeft[1] = 0; carCountLeft[2] = 0; carCountLeft[3] = 0;
	carCountRight[0] = 0; carCountRight[1] = 0; carCountRight[2] = 0; carCountRight[3] = 0;
	*/
	myfile.close();
	myfile.open("config/result.csv", std::ios_base::app);
	full_width = (int)(m_rectCurHist.right - m_rectCurHist.left - 220);
	full_height = (int)(m_rectCurHist.bottom - m_rectCurHist.top - 150);
	if (RtspCount >= SplitWCount)
	{
		RtspCount = SplitWCount;
	}
	kill_All_Cams();
	loadConfig();
//	listCameras();
	for (int i = 0; i < RtspCount; i++)
	{
		RTSPPARAM[i].nThreadIndex = i;
		g_bThreadStop[i] = false;
		m_pThread[i] = ::AfxBeginThread(displayRTSP, (LPVOID)&RTSPPARAM[i]);
	}
	
	//// refresh camera state
	//int list = 0;
	//::AfxBeginThread(threadStateCameralist, (LPVOID)list);

}

void CMultiIPCAMERADlg::overwriteFile()
{
	CStdioFile file;
	CString write_content;
	g_own_file = true;
	file.Open(_T("conf/cameralist.txt"), CFile::modeCreate |	CFile::modeWrite | CFile::typeText);
	for (int i = 0 ; i < RtspCount; i++)
	{
		std::stringstream valx, valy, valw, valh, val_motion_percentage;
		valx << RTSPPARAM[i].ROI_rect.x;
		valy << RTSPPARAM[i].ROI_rect.y;
		valw << RTSPPARAM[i].ROI_rect.width;
		valh << RTSPPARAM[i].ROI_rect.height;
		val_motion_percentage << RTSPPARAM[i].motion_percentage;
		std:stringstream lline1x, lline1y, lline2x, lline2y, rline1x, rline1y, rline2x, rline2y;
		lline1x << RTSPPARAM[i].leftPoint[0].x;
		lline1y << RTSPPARAM[i].leftPoint[0].y;
		lline2x << RTSPPARAM[i].leftPoint[1].x;
		lline2y << RTSPPARAM[i].leftPoint[1].y;
		rline1x << RTSPPARAM[i].rightPoint[0].x;
		rline1y << RTSPPARAM[i].rightPoint[0].y;
		rline2x << RTSPPARAM[i].rightPoint[1].x;
		rline2y << RTSPPARAM[i].rightPoint[1].y;
		write_content = CString(RTSPPARAM[i].name.c_str())+">"+CString(RTSPPARAM[i].RTSP.c_str())
			+ ">" + CString(RTSPPARAM[i].FTP_SERVER.c_str()) +">"+ CString(RTSPPARAM[i].FTP_USER.c_str())
			+ ">" + CString(RTSPPARAM[i].FTP_PASS.c_str())
			+ ">" + CString(valx.str().c_str()) 
			+ ">" + CString(valy.str().c_str())
			+ ">" + CString(valw.str().c_str())
			+ ">" + CString(valh.str().c_str())
			+ ">" + CString(val_motion_percentage.str().c_str())
			+ ">" + CString(lline1x.str().c_str())
			+ ">" + CString(lline1y.str().c_str())
			+ ">" + CString(lline2x.str().c_str())
			+ ">" + CString(lline2y.str().c_str())
			+ ">" + CString(rline1x.str().c_str())
			+ ">" + CString(rline1y.str().c_str())
			+ ">" + CString(rline2x.str().c_str())
			+ ">" + CString(rline2y.str().c_str())
			+ ">" + CString("\n");
		try
		{
			file.WriteString(write_content.GetBuffer());
		}
		catch (exception)
		{
			continue;
		}
	}
	file.Close();
	g_own_file = false;
}
//button click settings and show dlg of settings
void CMultiIPCAMERADlg::OnBnClickedBtnSettings()
{
	// TODO: Add your control notification handler code here
	 MSettingsDlg Settings_dlg;
	 Settings_dlg.DoModal();	 
}
//read settings
void CMultiIPCAMERADlg::checkSettings()
{
	SettingsArg settings;
	//std::wifstream file(L"myini.ini");
	LPCSTR filename = "conf\\myini.ini";
	
	if (PathFileExistsA(filename))
	{
		TCHAR szBuf[MAX_PATH] = { 0, };
		CString strSection, strKey, strValue;

		strSection = _T("Server");

		strKey = _T("ThresHold1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		settings.threshold1 = _ttoi(strValue);

		strKey = _T("ThresHold2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		settings.threshold2 = _ttoi(strValue);

		strKey = _T("ThresHold3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		settings.threshold3 = _ttoi(strValue);

		strKey = _T("ThresHold4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		settings.threshold4 = _ttoi(strValue);

		strKey = _T("SkipFrames1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		skip_frames1 = _ttoi(strValue);

		strKey = _T("SkipFrames2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		skip_frames2 = _ttoi(strValue);

		strKey = _T("SkipFrames3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		skip_frames3 = _ttoi(strValue);

		strKey = _T("SkipFrames4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		skip_frames4 = _ttoi(strValue);

		strKey = _T("Rate1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		rate1 = _ttoi(strValue);

		strKey = _T("Rate2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		rate2 = _ttoi(strValue);

		strKey = _T("Rate3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		rate3 = _ttoi(strValue);

		strKey = _T("Rate4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		rate4 = _ttoi(strValue);

		strKey = _T("Model");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		selected_model = strValue;
	}
	else {
		CString strSection, strKey, strValue;
		strSection = _T("Server");
		strKey = _T("ThresHold");
		strValue = _T("10");
		WritePrivateProfileString(strSection, strKey, strValue, strINIPath);
		strKey = _T("Model");
		strValue = _T("tiny");
		WritePrivateProfileString(strSection, strKey, strValue, strINIPath);
		AfxMessageBox(_T("Please configurate ftp server"));
	}
}
//read variable in myini.ini
void CMultiIPCAMERADlg::loadConfig()
{

	TCHAR szBuf[MAX_PATH] = { 0, };
	CString strSection, strKey, strValue;

	strSection = _T("Server");

	strKey = _T("ThresHold1");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	conf_threshold1 = _ttoi(strValue);

	strKey = _T("ThresHold2");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	conf_threshold2 = _ttoi(strValue);

	strKey = _T("ThresHold3");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	conf_threshold3 = _ttoi(strValue);

	strKey = _T("ThresHold4");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	conf_threshold4 = _ttoi(strValue);

	strKey = _T("SkipFrames1");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	skip_frames1 = _ttoi(strValue);

	strKey = _T("SkipFrames2");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	skip_frames2 = _ttoi(strValue);

	strKey = _T("SkipFrames3");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	skip_frames3 = _ttoi(strValue);

	strKey = _T("SkipFrames4");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	skip_frames4 = _ttoi(strValue);

	strKey = _T("Model");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	selected_model = strValue;

	strKey = _T("MotionFPS1");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	motion_fps1 = _ttoi(strValue);

	strKey = _T("MotionFPS2");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	motion_fps2 = _ttoi(strValue);

	strKey = _T("MotionFPS3");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	motion_fps3 = _ttoi(strValue);

	strKey = _T("MotionFPS4");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	motion_fps4 = _ttoi(strValue);

	strKey = _T("Rate1");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	rate1 = _ttoi(strValue);

	strKey = _T("Rate2");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	rate2 = _ttoi(strValue);

	strKey = _T("Rate3");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	rate3 = _ttoi(strValue);

	strKey = _T("Rate4");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	rate4 = _ttoi(strValue);
}

//check camera statue in camera list
void CMultiIPCAMERADlg::checkCameraStatue()
{
	LVITEM lvi;
	m_lstCamera.DeleteAllItems();
	for (int i = 0; i < RtspCount; i++)
	{
		//aLineArray.Add(sLine);
		//split name and address in a line
		int nTokenPos = 0;
		CString name;
		CString address;
		CString ftp_server;
		CString ftp_user;
		CString ftp_pass;
		CString camera_statue;
		CString x, y, w, h;

		//check camera statue
		CT2CA pszConvertedAnsiString(address);
		std::string strStd(pszConvertedAnsiString);
		name = CString(RTSPPARAM[i].name.c_str());
		if (RTSPPARAM[i].online_offline)
			camera_statue = _T("Online");
		else
			camera_statue = _T("Offline");

		//set icon list 
		lvi.mask = LVIF_IMAGE | LVIF_TEXT;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)(name);
		lvi.iImage = 1;		// There are 8 images in the image list
		m_lstCamera.InsertItem(&lvi);

		// Set subitem 1
		lvi.iSubItem = 1;
		lvi.pszText = (LPTSTR)(LPCTSTR)(camera_statue);
		m_lstCamera.SetItem(&lvi);
	}
}

void CMultiIPCAMERADlg::OnBnClickedButtonFour()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < RtspCount; i++)
	{
		g_bThreadStop[i] = true;
	}
	Sleep(500);
	SplitWCount = 4;
	
	if (SplitWCount == 4) {
		camera5.ShowWindow(SW_HIDE);
		camera6.ShowWindow(SW_HIDE);
		camera7.ShowWindow(SW_HIDE);
		camera8.ShowWindow(SW_HIDE);
		camera9.ShowWindow(SW_HIDE);
	}
	getCamsProp(SplitWCount);
	refreshCams();
}

void CMultiIPCAMERADlg::OnBnClickedButtonNine()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < RtspCount; i++)
	{
		g_bThreadStop[i] = true;
	}
	Sleep(500);
	SplitWCount = 9;
	if (SplitWCount == 9) {
		camera5.ShowWindow(SW_SHOW);
		camera6.ShowWindow(SW_SHOW);
		camera7.ShowWindow(SW_SHOW);
		camera8.ShowWindow(SW_SHOW);
		camera9.ShowWindow(SW_SHOW);
	}
	
	getCamsProp(SplitWCount);
	refreshCams();
	
}

void CMultiIPCAMERADlg::ClearAllCamsWindows()
{
	camera1.ShowWindow(SW_HIDE);
	var_camera2.ShowWindow(SW_HIDE);
	camera3.ShowWindow(SW_HIDE);
	camera4.ShowWindow(SW_HIDE);
	camera5.ShowWindow(SW_HIDE);
	camera6.ShowWindow(SW_HIDE);
	camera7.ShowWindow(SW_HIDE);
	camera8.ShowWindow(SW_HIDE);
	camera9.ShowWindow(SW_HIDE);
	camera_full.ShowWindow(SW_HIDE);
}

void CMultiIPCAMERADlg::displayPicControls()
{
	ClearAllCamsWindows();
	if (g_full_check)
	{
		camera_full.ShowWindow(SW_SHOW);
	}
	if(!g_full_check)
	{
		if (SplitWCount == 4)
		{
			camera1.ShowWindow(SW_SHOW);
			var_camera2.ShowWindow(SW_SHOW);
			camera3.ShowWindow(SW_SHOW);
			camera4.ShowWindow(SW_SHOW);
		}
		if (SplitWCount == 9)
		{
			camera1.ShowWindow(SW_SHOW);
			var_camera2.ShowWindow(SW_SHOW);
			camera3.ShowWindow(SW_SHOW);
			camera4.ShowWindow(SW_SHOW);
			camera5.ShowWindow(SW_SHOW);
			camera6.ShowWindow(SW_SHOW);
			camera7.ShowWindow(SW_SHOW);
			camera8.ShowWindow(SW_SHOW);
			camera9.ShowWindow(SW_SHOW);
		}

	}
}

//full pic double click event
void CMultiIPCAMERADlg::OnDblclkFullPic()
{
	// TODO: Add your control notification handler code here
}

void CMultiIPCAMERADlg::OnDblclkCme()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("Would u kill all threads of camera streams"));

}

void CMultiIPCAMERADlg::OnDoubleclickedAddCam()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("Would u kill all threads of camera streams"));

}

void CMultiIPCAMERADlg::OnStnClickedCme()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("Would u kill all threads of camera streams"));
}

void CMultiIPCAMERADlg::OnStnDblclickCam3()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("Would u kill all threads of camera streams"));
}
//opencv windows event
void CMultiIPCAMERADlg::OnBnClickedBtnRoiPen()
{
	// TODO: Add your control notification handler code here
	g_selection_tool = !g_selection_tool;
	if (g_selection_tool)
	{
		sel_leftline = false;
		sel_rightline = false;
	}
}

void CMultiIPCAMERADlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	RTSPPARAM[g_selected_idx].ROI_rect_check = false;
	RTSPPARAM[g_selected_idx].ROI_rect = Rect(0, 0, 0, 0);
	RTSPPARAM[g_selected_idx].rightPoint[0] = cv::Point(0, 0);
	RTSPPARAM[g_selected_idx].rightPoint[1] = cv::Point(0, 0);
	RTSPPARAM[g_selected_idx].leftPoint[0] = cv::Point(0, 0);
	RTSPPARAM[g_selected_idx].leftPoint[1] = cv::Point(0, 0);
	leftLine[g_selected_idx][0] = cv::Point(0, 0);
	leftLine[g_selected_idx][1] = cv::Point(0, 0);
	rightLine[g_selected_idx][0] = cv::Point(0, 0);
	rightLine[g_selected_idx][1] = cv::Point(0, 0);
	RTSPPARAM[g_selected_idx].leftline_check = false;
	RTSPPARAM[g_selected_idx].rightline_check = false;
	//RTSPPARAM_FULL.ROI_rect_check = false;
	//RTSPPARAM_FULL.ROI_rect = Rect(0, 0, 0, 0);
	box = Rect(0, 0, 0, 0);
	overwriteFile();
}

void CMultiIPCAMERADlg::OnBnClickedButtonSplit()
{
	// TODO: Add your control notification handler code here
	fullScreen(g_selected_idx);
}

void CMultiIPCAMERADlg::OnBnClickedButtonPtgOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	
	RTSPPARAM_FULL.motion_percentage = _ttoi(m_motion_percentage);
	RTSPPARAM[g_selected_idx].motion_percentage = _ttoi(m_motion_percentage);
	AfxMessageBox(_T("saved motion area percentage."));

}

void copy_file(const char* srce_file, const char* dest_file)
{
	std::ifstream srce(srce_file, std::ios::binary);
	std::ofstream dest(dest_file, std::ios::binary);
	dest << srce.rdbuf();
}

void CMultiIPCAMERADlg::OnBnClickedButton3()
{
	//report dialog
	// TODO: Add your control notification handler code here
	CReportDlg ReportDlg;
	ReportDlg.DoModal();
}

void CMultiIPCAMERADlg::OnBnClickedButton2()
{
	//summary dialog
	// TODO: Add your control notification handler code here
//	myfile.close();
//	copy_file("config/result.csv", "config/result1.csv");
//	myfile.open("config/result.csv", std::ios_base::app);
	SummaryDlg SummaryDlg;
	SummaryDlg.DoModal();
}


//void CMultiIPCAMERADlg::OnBnClickedButton4()
//{
//	// TODO: Add your control notification handler code here
//}


//void CMultiIPCAMERADlg::OnBnClickedButton5()
//{
//	// TODO: Add your control notification handler code here
//}


void CMultiIPCAMERADlg::OnBnClickedBtnRoiPen2()
{
	// TODO: Add your control notification handler code here
	//select leftline
	sel_leftline = !sel_leftline;
	if (sel_leftline)
	{
		sel_rightline = false;
		g_selection_tool = false;
	}
}


void CMultiIPCAMERADlg::OnBnClickedBtnRoiPen3()
{
	// TODO: Add your control notification handler code here
	//select rightline
	sel_rightline = !sel_rightline;
	if (sel_rightline)
	{
		sel_leftline = false;
		g_selection_tool = false;
	}
}
