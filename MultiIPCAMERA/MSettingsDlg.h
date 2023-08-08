#pragma once
#include "afxcmn.h"


// MSettingsDlg dialog

class MSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MSettingsDlg)

public:
	MSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MSettingsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS };
#endif
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void checkSettings();
	
	
//	CString m_ftppass;
//	CString m_ftpserver;
//	CString m_ftpuser;
	LPCSTR filename;
	CString strINIPath;
	afx_msg void OnBnClickedOk();
//	int m_radio_tiny;
//	int m_radio_yolo2;
	//afx_msg void OnRadio1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_nthreshold1;
	int m_nthreshold2;
	int m_nthreshold3;
	int m_nthreshold4;
	CString m_threshold1;
	CString m_threshold2;
	CString m_threshold3;
	CString m_threshold4;
	CSliderCtrl m_cSlider1;
	CSliderCtrl m_cSlider2;
	CSliderCtrl m_cSlider3;
	CSliderCtrl m_cSlider4;
	CString m_skip_frames1;
	CString m_skip_frames2;
	CString m_skip_frames3;
	CString m_skip_frames4;
	CString m_motion_fps1;
	CString m_motion_fps2;
	CString m_motion_fps3;
	CString m_motion_fps4;
	CString m_rate1;
	CString m_rate2;
	CString m_rate3;
	CString m_rate4;
};
