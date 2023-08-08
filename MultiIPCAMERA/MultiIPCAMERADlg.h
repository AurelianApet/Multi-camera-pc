
// MultiIPCAMERADlg.h : header file
//

#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "ImageButton.h"
#include <vector>
using namespace cv;
using namespace std;

int static KillThreadFlag = 0;
struct FileUploadArg
{
	string ftpserver;
	string usr;
	string pass;
	string src;
	string dst;
};
struct ThreadYOLOArg
{
	int nThreadIndex;
	Mat image;
	String W;

};
struct ThreadRTSPArg
{
	int nThreadIndex;
	String RTSP;
	String W;
	String FTP_SERVER;
	String FTP_USER;
	String FTP_PASS;
	String name;
	int Width;
	int Height;
	int X;
	int Y;
	ThreadYOLOArg YArg;
	Rect ROI_rect;
	bool ROI_rect_check;
	cv::Point leftPoint[2];
	bool leftline_check;
	cv::Point rightPoint[2];
	bool rightline_check;
	bool online_offline;
	int motion_percentage;
};
struct SettingsArg
{
	int threshold1;
	int threshold2;
	int threshold3;
	int threshold4;

	CString ftpserver;
	CString ftpuser;
	CString ftppass;
};
//struct TriggerArg
//{
//	Mat image;
//	int trigger;
//};


// CMultiIPCAMERADlg dialog
class CMultiIPCAMERADlg : public CDialogEx
{
// Construction
public:
	CMultiIPCAMERADlg(CWnd* pParent = NULL);	// standard constructor
	void DisplayFull(int param_id);
	void createCVWindows(int IDC, int x, int y, int width, int height, char* w);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MULTIIPCAMERA_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedAddCam();				//add camera address button for open add new form
	afx_msg void OnBnClickedBtnSettings();			//Setting button click event funciton
	afx_msg void OnBnClickedButtonFour();			//click event function of 4 split windows
	afx_msg void OnBnClickedButtonNine();			//click event function of 9 split windows
	afx_msg void OnDblclkFullPic();					//double click event function of Full picture control 
	afx_msg void OnDblclkCme();						//double click event function of camera 1
	afx_msg void OnDoubleclickedAddCam();
	afx_msg void OnStnClickedCme();
	afx_msg void OnStnDblclickCam3();
	afx_msg void OnBnClickedBtnRoiPen();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSplit();
	DECLARE_MESSAGE_MAP()

protected:

	CStatic		var_camera2;
	CStatic		camera5;
	CStatic		camera6;
	CStatic		camera7;
	CStatic		camera8;
	CStatic		camera9;
	CStatic		camera_full;
	CStatic		camera3;
	CStatic		camera4;
	CStatic		camera1;

public:
	int		SplitWCount;//camera windows count
	static std::string extractIP(cv::String rtspURL);
	static UINT displayRTSP(LPVOID args);  //stream camera thread proc
	static UINT uploadVideoThread(LPVOID args);  //stream yolo image thread proc
	static UINT threadStateCameralist(LPVOID Param);
	static UINT startProcess(LPVOID Param);
	CImageButton m_btnAddCamera;											//camera button of parent form for add new camera
	CImageButton m_btnRefresh;												//camera refresh button as CImageButton
	CImageButton m_btnSettings;												//camera settings button as CImageButton	
	CImageButton m_btnFour;													//4 split button
	CImageButton m_btnNine;													//9 split button
	CImageButton m_btn_roi_pen;												//ROI Pen button
	CImageButton m_btn_clear;
	CImageButton m_btn_split;

	CImageButton m_leftLine;
	CImageButton m_rightLine;

	//afx_msg void OnBnClickedButton5();
	CListCtrl m_lstCamera;													//root form camera list control
	CImageList	m_cImageListNormal, m_cImageListSmall, m_cImageListState;   //camera list icon image list
	CWinThread *m_pThread[9];												//camera streaming thread array
	CString strINIPath;														//ini file path
	CString m_upload_statue;
	CString selected_model;
	
	

//	string m_ftpserver;
	void checkCameraStatue();												//function of check camera
	void loadConfig();
	void refreshCams();														//refresh cameras on form
	void initlistCameras();														//get cameras list proc
	void checkSettings();													//check ini file for setting variables
	void ReadNameList(); 
	void getCamsProp(int splitCount);										//get camera properties e.g: rtsp, window, h,w, x, y
	void ClearAllCamsWindows();
	void displayPicControls();												//display Picture controls according to windows count
	void ProcFull(int param_id);
	void overwriteFile();
	
	

	
	
	
	
	CString m_motion_percentage;
	CStatic m_static_ptg;
	CButton c_btn_ptg_ok;
	CEdit c_ptg;
	afx_msg void OnBnClickedButtonPtgOk();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
//	afx_msg void OnBnClickedButton4();
//	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedBtnRoiPen2();
	afx_msg void OnBnClickedBtnRoiPen3();
};
