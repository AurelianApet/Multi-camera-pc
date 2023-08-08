// AddCameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiIPCAMERA.h"
#include "AddCameraDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <string>

using namespace std;

// CAddCameraDlg dialog

IMPLEMENT_DYNAMIC(CAddCameraDlg, CDialogEx)

CAddCameraDlg::CAddCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ADDCAM, pParent)
	, m_sText(_T(""))
	, str_camera_label(_T(""))
	, m_ftp_server(_T(""))
	, m_ftp_user(_T(""))
	, m_ftp_pass(_T(""))
{

}

CAddCameraDlg::~CAddCameraDlg()
{
}

void CAddCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sText);
	DDX_Text(pDX, IDC_EDIT_CAM_LABEL, str_camera_label);
	//  DDX_Control(pDX, IDC_LIST1, LIST_CAM);
	DDX_Control(pDX, IDC_LIST1, LIST_CAM);
	DDX_Text(pDX, IDC_SERVER, m_ftp_server);
	DDX_Text(pDX, IDC_FTPUSER, m_ftp_user);
	DDX_Text(pDX, IDC_FTPPASS, m_ftp_pass);
}


BEGIN_MESSAGE_MAP(CAddCameraDlg, CDialogEx)
	ON_BN_CLICKED(IDC_Btn_AddCam, &CAddCameraDlg::OnBnClickedBtnAddcam)
	ON_BN_CLICKED(IDOK, &CAddCameraDlg::OnBnClickedOk)

	ON_BN_CLICKED(IDC_Btn_AddCam2, &CAddCameraDlg::OnBnClickedBtnAddcam2)
END_MESSAGE_MAP()


// CAddCameraDlg message handlers


void CAddCameraDlg::OnBnClickedBtnAddcam()
{
	// TODO: Add your rtsp address process
	UpdateData(true);
	CStdioFile file;
	CString default_percentage = _T("10");
	if (m_sText.GetLength() == 0) {
		AfxMessageBox(_T("You must specify the CCTV camera address."));
		return;
	}
	if (str_camera_label.GetLength() == 0) {
		AfxMessageBox(_T("You must specify the CCTV camera label."));
		return;
	}


	CString write_content = str_camera_label + ">" + m_sText + ">" + m_ftp_server + ">" + m_ftp_user + ">" + m_ftp_pass 
		+ ">" + ">" + ">" + ">" + ">" + default_percentage + ">" "\n";
	file.Open(_T("conf/cameralist.txt"), CFile::modeCreate |
		CFile::modeWrite | CFile::modeNoTruncate);
	file.SeekToEnd();
	file.WriteString(write_content);
	file.Close();

	m_ftp_server = "";
	m_ftp_user = "";
	m_ftp_pass = "";
	str_camera_label = "";
	m_sText = "";
	UpdateData(false);
	loadCamList();
	//GetDlgItem(IDC_EDIT1)->SetWindowText(_T(""));
	//GetDlgItem(IDC_EDIT_CAM_LABEL)->SetWindowText(_T(""));
	//GetDlgItem(IDC_EDIT_FTPSERVER)->SetWindowText(_T(""));
	//GetDlgItem(IDC_FTPUSER)->SetWindowText(_T(""));
	//GetDlgItem(IDC_FTPPASS)->SetWindowText(_T(""));
	
	//UpdateData(false);*/
}


void CAddCameraDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CAddCameraDlg::loadCamList()
{
	//inital LIST_CAM that clear all
	LIST_CAM.DeleteAllItems();
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

			AfxExtractSubString(name, sLine, 0, '>');
			AfxExtractSubString(address, sLine, 1, '>');
			AfxExtractSubString(ftp_server, sLine, 2, '>');
			AfxExtractSubString(ftp_user, sLine, 3, '>');
			AfxExtractSubString(ftp_pass, sLine, 4, '>');

			//set icon list 
			lvi.mask = LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = iLineCount;
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(name);
			lvi.iImage = 1;		// There are 8 images in the image list
			LIST_CAM.InsertItem(&lvi);
			// Set subitem 1
			lvi.iSubItem = 1;
			lvi.pszText = (LPTSTR)(LPCTSTR)(address);
			LIST_CAM.SetItem(&lvi);
			//set ftp server
			lvi.iSubItem = 2;
			lvi.pszText = (LPTSTR)(LPCTSTR)(ftp_server);
			LIST_CAM.SetItem(&lvi);
			//set ftp user
			lvi.iSubItem = 3;
			lvi.pszText = (LPTSTR)(LPCTSTR)(ftp_user);
			LIST_CAM.SetItem(&lvi);
			//set ftp pass
			lvi.iSubItem = 4;
			lvi.pszText = (LPTSTR)(LPCTSTR)(ftp_pass);
			LIST_CAM.SetItem(&lvi);


			iLineCount++;
		}
		StdFile.Close();
	}
	
}


BOOL CAddCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	HIMAGELIST hList = ImageList_Create(32, 32, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_cImageListNormal.Attach(hList);

	hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_cImageListSmall.Attach(hList);


	// Load the large icons
	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BITMAP2);
	m_cImageListNormal.Add(&cBmp, RGB(255, 0, 255));
	cBmp.DeleteObject();

	// Load the small icons
	cBmp.LoadBitmap(IDB_BITMAP3);
	m_cImageListSmall.Add(&cBmp, RGB(255, 0, 255));

	// Attach them
	LIST_CAM.SetImageList(&m_cImageListNormal, LVSIL_NORMAL);
	LIST_CAM.SetImageList(&m_cImageListSmall, LVSIL_SMALL);


	DWORD dwStyle = LIST_CAM.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	LIST_CAM.SetExtendedStyle(dwStyle);

	CRect rect;
	LIST_CAM.GetClientRect(&rect);
	int nColInterval = rect.Width() / 5;

	LIST_CAM.InsertColumn(0, _T("Camera Name"), LVCFMT_LEFT, nColInterval);
	LIST_CAM.InsertColumn(1, _T("CCTV Address"), LVCFMT_LEFT, nColInterval);
	LIST_CAM.InsertColumn(2, _T("FTP Server"), LVCFMT_LEFT, nColInterval);
	LIST_CAM.InsertColumn(3, _T("FTP User"), LVCFMT_LEFT, nColInterval);
	LIST_CAM.InsertColumn(4, _T("FTP Password"), LVCFMT_LEFT, nColInterval);
	filename = "conf\\myini.ini";
	strINIPath = _T("conf\\myini.ini");
	loadCamList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAddCameraDlg::OnBnClickedBtnAddcam2()
{
	// TODO: Add your control notification handler code here
	//delete
	UpdateData(true);
	POSITION pos;
	LIST_CAM.GetNextSelectedItem(pos);
	if (int(pos) < 1)
		MessageBox(L"Please select item to delete!", L"Warning!");
	CString nFilename = _T("conf/cameralist.txt");
	CString sLine;
	CFileException ex;
	CStdioFile StdFile;
	CString content;
	int lineCount = 0;
	if (StdFile.Open(nFilename, CFile::modeNoTruncate | CFile::modeRead, &ex))
	{
		while (StdFile.ReadString(sLine))
		{
			lineCount++;
			if(lineCount != int(pos))
				content += sLine + _T("\n");
		}
		StdFile.Close();
	}

	std::ofstream ofs;
	ofs.open("conf/cameralist.txt");
	CT2CA pszConvertedAnsiString(content);
	std::string strStd(pszConvertedAnsiString);
	ofs << strStd;
	ofs.close();

	UpdateData(false);
	loadCamList();
}
