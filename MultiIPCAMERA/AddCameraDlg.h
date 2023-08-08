#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAddCameraDlg dialog

class CAddCameraDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddCameraDlg)

public:
	CAddCameraDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddCameraDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADDCAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	LPCSTR filename;
	CString strINIPath;
	CListCtrl LIST_CAM;
	CImageList	m_cImageListNormal, m_cImageListSmall, m_cImageListState;
	CString str_camera_label;
	CString m_sText;
public:
	afx_msg void OnBnClickedBtnAddcam();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void loadCamList();
//	CListCtrl LIST_CAM;
	
	
protected:
	CString m_ftp_server;
	CString m_ftp_user;
	CString m_ftp_pass;
public:
	afx_msg void OnBnClickedBtnAddcam2();
};
