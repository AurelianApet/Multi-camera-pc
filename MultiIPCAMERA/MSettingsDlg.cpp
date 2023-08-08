// MSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiIPCAMERA.h"
#include "MSettingsDlg.h"
#include "afxdialogex.h"


// MSettingsDlg dialog

IMPLEMENT_DYNAMIC(MSettingsDlg, CDialogEx)

MSettingsDlg::MSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SETTINGS, pParent)
	, m_threshold1(_T(""))	
	, m_skip_frames1(_T(""))
	, m_motion_fps1(_T(""))
	, m_rate1(_T(""))

	, m_threshold2(_T(""))
	, m_skip_frames2(_T(""))
	, m_motion_fps2(_T(""))
	, m_rate2(_T(""))

	, m_threshold3(_T(""))
	, m_skip_frames3(_T(""))
	, m_motion_fps3(_T(""))
	, m_rate3(_T(""))

	, m_threshold4(_T(""))
	, m_skip_frames4(_T(""))
	, m_motion_fps4(_T(""))
	, m_rate4(_T(""))
{

}

MSettingsDlg::~MSettingsDlg()
{
}

void MSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ThresHold1, m_threshold1);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD1, m_cSlider1);
	DDX_Text(pDX, IDC_EDIT_SKIPFRAME1, m_skip_frames1);
	DDX_Text(pDX, IDC_EDIT_MOTION_FPS1, m_motion_fps1);
	DDX_Text(pDX, IDC_Rate1, m_rate1);

	DDX_Text(pDX, IDC_EDIT_ThresHold2, m_threshold2);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD2, m_cSlider2);
	DDX_Text(pDX, IDC_EDIT_SKIPFRAME2, m_skip_frames2);
	DDX_Text(pDX, IDC_EDIT_MOTION_FPS2, m_motion_fps2);
	DDX_Text(pDX, IDC_Rate2, m_rate2);

	DDX_Text(pDX, IDC_EDIT_ThresHold3, m_threshold3);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD3, m_cSlider3);
	DDX_Text(pDX, IDC_EDIT_SKIPFRAME3, m_skip_frames3);
	DDX_Text(pDX, IDC_EDIT_MOTION_FPS3, m_motion_fps3);
	DDX_Text(pDX, IDC_Rate3, m_rate3);

	DDX_Text(pDX, IDC_EDIT_ThresHold4, m_threshold4);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD4, m_cSlider4);
	DDX_Text(pDX, IDC_EDIT_SKIPFRAME4, m_skip_frames4);
	DDX_Text(pDX, IDC_EDIT_MOTION_FPS4, m_motion_fps4);
	DDX_Text(pDX, IDC_Rate4, m_rate4);
}


BEGIN_MESSAGE_MAP(MSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &MSettingsDlg::OnBnClickedOk)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()



// MSettingsDlg message handlers


BOOL MSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	filename = "conf\\myini.ini";
	strINIPath = _T("conf\\myini.ini");
	checkSettings();

	
	m_cSlider1.SetRange(1, 70);
	m_cSlider1.SetPageSize(10);
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void MSettingsDlg::checkSettings()
{
	

	if (PathFileExistsA(filename))
	{
		TCHAR szBuf[MAX_PATH] = { 0, };
		CString strSection, strKey, strValue;

		strSection = _T("Server");

		strKey = _T("ThresHold1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_threshold1 = strValue;
		m_nthreshold1 = _wtoi(m_threshold1);
		m_cSlider1.SetPos(m_nthreshold1);

		strKey = _T("ThresHold2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_threshold2 = strValue;
		m_nthreshold2 = _wtoi(m_threshold2);
		m_cSlider2.SetPos(m_nthreshold2);

		strKey = _T("ThresHold3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_threshold3 = strValue;
		m_nthreshold3 = _wtoi(m_threshold3);
		m_cSlider3.SetPos(m_nthreshold3);

		strKey = _T("ThresHold4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_threshold4 = strValue;
		m_nthreshold4 = _wtoi(m_threshold4);
		m_cSlider4.SetPos(m_nthreshold4);

		strKey = _T("SkipFrames1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_skip_frames1 = strValue;

		strKey = _T("SkipFrames2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_skip_frames2 = strValue;

		strKey = _T("SkipFrames3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_skip_frames3 = strValue;

		strKey = _T("SkipFrames4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_skip_frames4 = strValue;

		strKey = _T("Rate1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_rate1 = strValue;

		strKey = _T("Rate2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_rate2 = strValue;

		strKey = _T("Rate3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_rate3 = strValue;

		strKey = _T("Rate4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_rate4 = strValue;

		strKey = _T("MotionFPS1");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_motion_fps1 = strValue;

		strKey = _T("MotionFPS2");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_motion_fps2 = strValue;

		strKey = _T("MotionFPS3");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_motion_fps3 = strValue;

		strKey = _T("MotionFPS4");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		m_motion_fps4 = strValue;


		strKey = _T("Model");
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		UpdateData(false);
	}
}


void MSettingsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
	UpdateData(true);
	if (_ttoi(m_threshold1) < 1 || _ttoi(m_threshold1) > 70
		|| _ttoi(m_threshold2) < 1 || _ttoi(m_threshold2) > 70
		|| _ttoi(m_threshold3) < 1 || _ttoi(m_threshold3) > 70
		|| _ttoi(m_threshold4) < 1 || _ttoi(m_threshold4) > 70)
	{
		AfxMessageBox(_T("threshold range is 1~70"));
		return;
	}
	
	
	CString strSection, strKey, strValue;
	strSection = _T("Server");
	strKey = _T("ThresHold1");
	strValue = m_threshold1;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("ThresHold2");
	strValue = m_threshold2;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("ThresHold3");
	strValue = m_threshold3;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("ThresHold4");
	strValue = m_threshold4;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("SkipFrames1");
	if (_ttoi(m_skip_frames1) < 0) 
	{
		m_skip_frames1 = "0";
	}
	strValue = m_skip_frames1;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("SkipFrames2");
	if (_ttoi(m_skip_frames2) < 0)
	{
		m_skip_frames2 = "0";
	}
	strValue = m_skip_frames2;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("SkipFrames3");
	if (_ttoi(m_skip_frames3) < 0)
	{
		m_skip_frames3 = "0";
	}
	strValue = m_skip_frames3;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("SkipFrames4");
	if (_ttoi(m_skip_frames4) < 0)
	{
		m_skip_frames4 = "0";
	}
	strValue = m_skip_frames4;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("MotionFPS1");
	if (_ttoi(m_motion_fps1) < 0)
	{
		m_motion_fps1 = "1";
	}
	strValue = m_motion_fps1;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("MotionFPS2");
	if (_ttoi(m_motion_fps2) < 0)
	{
		m_motion_fps2 = "1";
	}
	strValue = m_motion_fps2;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("MotionFPS3");
	if (_ttoi(m_motion_fps3) < 0)
	{
		m_motion_fps3 = "1";
	}
	strValue = m_motion_fps3;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("MotionFPS4");
	if (_ttoi(m_motion_fps4) < 0)
	{
		m_motion_fps4 = "1";
	}
	strValue = m_motion_fps4;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Rate1");
	if (_ttoi(m_rate1) < 0)
	{
		m_rate1 = "50";
	}
	strValue = m_rate1;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Rate2");
	if (_ttoi(m_rate2) < 0)
	{
		m_rate2 = "50";
	}
	strValue = m_rate2;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Rate3");
	if (_ttoi(m_rate3) < 0)
	{
		m_rate3 = "50";
	}
	strValue = m_rate3;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Rate4");
	if (_ttoi(m_rate4) < 0)
	{
		m_rate4 = "50";
	}
	strValue = m_rate4;
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Model");	
}


void MSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);

	// Check which slider sent the notification  
	if (pSlider == &m_cSlider1 )
	{
		// Check what happened  
		switch (nSBCode)
		{
		case TB_PAGEUP:
			m_nthreshold1 -= 10;
			if (m_nthreshold1 < 1)
				m_nthreshold1 = 1;
			break;
			
		case TB_PAGEDOWN:
			m_nthreshold1 += 10;
			if (m_nthreshold1 > 70)
				m_nthreshold1 = 70;

			break;
		case TB_THUMBPOSITION:
		case TB_THUMBTRACK:			
			m_nthreshold1 = nPos;
			break;
		default:
			break;

		}
		m_threshold1.Format(_T("%d"), m_nthreshold1);
		UpdateData(FALSE);
	}
	else if (pSlider == &m_cSlider2)
	{
		// Check what happened  
		switch (nSBCode)
		{
		case TB_PAGEUP:
			m_nthreshold2 -= 10;
			if (m_nthreshold2 < 1)
				m_nthreshold2 = 1;
			break;

		case TB_PAGEDOWN:
			m_nthreshold2 += 10;
			if (m_nthreshold2 > 70)
				m_nthreshold2 = 70;

			break;
		case TB_THUMBPOSITION:
		case TB_THUMBTRACK:
			m_nthreshold1 = nPos;
			break;
		default:
			break;

		}
		m_threshold2.Format(_T("%d"), m_nthreshold2);
		UpdateData(FALSE);
	}
	else if (pSlider == &m_cSlider3)
	{
		// Check what happened  
		switch (nSBCode)
		{
		case TB_PAGEUP:
			m_nthreshold3 -= 10;
			if (m_nthreshold3 < 1)
				m_nthreshold3 = 1;
			break;

		case TB_PAGEDOWN:
			m_nthreshold3 += 10;
			if (m_nthreshold3 > 70)
				m_nthreshold3 = 70;
			break;
		case TB_THUMBPOSITION:
		case TB_THUMBTRACK:
			m_nthreshold3 = nPos;
			break;
		default:
			break;
		}
		m_threshold3.Format(_T("%d"), m_nthreshold3);
		UpdateData(FALSE);
	}
	else if (pSlider == &m_cSlider4)
	{
		// Check what happened  
		switch (nSBCode)
		{
		case TB_PAGEUP:
			m_nthreshold4 -= 10;
			if (m_nthreshold4 < 1)
				m_nthreshold4 = 1;
			break;

		case TB_PAGEDOWN:
			m_nthreshold4 += 10;
			if (m_nthreshold4 > 70)
				m_nthreshold4 = 70;
			break;
		case TB_THUMBPOSITION:
		case TB_THUMBTRACK:
			m_nthreshold4 = nPos;
			break;
		default:
			break;
		}
		m_threshold4.Format(_T("%d"), m_nthreshold4);
		UpdateData(FALSE);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
