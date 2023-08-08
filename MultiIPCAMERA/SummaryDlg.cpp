// SummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiIPCAMERA.h"
#include "SummaryDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <iterator>
#include <sstream>

using namespace std;
// SummaryDlg dialog
IMPLEMENT_DYNAMIC(SummaryDlg, CDialogEx)

SummaryDlg::SummaryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SUMMARY, pParent)
{
}

SummaryDlg::~SummaryDlg()
{
}

void SummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_LIST3, m_list3);
	DDX_Control(pDX, IDC_LIST4, m_list4);
	DDX_Control(pDX, IDC_LIST5, m_list5);
	DDX_Control(pDX, IDC_LIST6, m_list6);
	DDX_Control(pDX, IDC_LIST7, m_list7);
	DDX_Control(pDX, IDC_LIST8, m_list8);
}


BEGIN_MESSAGE_MAP(SummaryDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &SummaryDlg::OnBnClickedCancel)
END_MESSAGE_MAP()



void SummaryDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


BOOL SummaryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitLists();
	LoadData();
	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SummaryDlg::InitLists()
{
	m_list1.DeleteAllItems();
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list1.InsertColumn(0, _T("Hour"), LVCFMT_LEFT, 100, -1);
	m_list1.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1); 
	m_list1.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list2.DeleteAllItems();
	m_list2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list2.InsertColumn(0, _T("Month"), LVCFMT_LEFT, 100, -1);
	m_list2.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list2.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list3.DeleteAllItems();
	m_list3.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list3.InsertColumn(0, _T("Hour"), LVCFMT_LEFT, 100, -1);
	m_list3.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list3.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list4.DeleteAllItems();
	m_list4.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list4.InsertColumn(0, _T("Month"), LVCFMT_LEFT, 100, -1);
	m_list4.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list4.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list5.DeleteAllItems();
	m_list5.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list5.InsertColumn(0, _T("Hour"), LVCFMT_LEFT, 100, -1);
	m_list5.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list5.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list6.DeleteAllItems();
	m_list6.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list6.InsertColumn(0, _T("Month"), LVCFMT_LEFT, 100, -1);
	m_list6.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list6.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list7.DeleteAllItems();
	m_list7.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list7.InsertColumn(0, _T("Hour"), LVCFMT_LEFT, 100, -1);
	m_list7.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list7.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	m_list8.DeleteAllItems();
	m_list8.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list8.InsertColumn(0, _T("Month"), LVCFMT_LEFT, 100, -1);
	m_list8.InsertColumn(1, _T("VehicleCount"), LVCFMT_LEFT, 100, -1);
	m_list8.InsertColumn(2, _T("Direction"), LVCFMT_RIGHT, 100, -1);

	CString str;
	for (int i = 23; i >= 0; i--)
	{
		str.Format(_T("%d"), i);
		m_list1.InsertItem(0, str);
		m_list1.InsertItem(0, str);
		m_list3.InsertItem(0, str);
		m_list3.InsertItem(0, str);
		m_list5.InsertItem(0, str);
		m_list5.InsertItem(0, str);
		m_list7.InsertItem(0, str);
		m_list7.InsertItem(0, str);
	}
	for (int i = 12; i > 0; i--)
	{
		str.Format(_T("%d"), i);
		m_list2.InsertItem(0, str);
		m_list2.InsertItem(0, str);
		m_list4.InsertItem(0, str);
		m_list4.InsertItem(0, str);
		m_list6.InsertItem(0, str);
		m_list6.InsertItem(0, str);
		m_list8.InsertItem(0, str);
		m_list8.InsertItem(0, str);
	}
}

std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)
{
	std::vector<std::string>   result;
	std::string                line;
	std::getline(str, line);

	std::stringstream          lineStream(line);
	std::string                cell;

	while (std::getline(lineStream, cell, ','))
	{
		result.push_back(cell);
	}
	// This checks for a trailing comma with no data after it.
	if (!lineStream && cell.empty())
	{
		// If there was a trailing comma then add an empty element.
		result.push_back("");
	}
	return result;
}

std::istream& operator >> (std::istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}

void SummaryDlg::LoadData()
{
	std::ifstream file("config/result.csv");
	CSVRow row;
	CString str;
	while (file >> row)
	{
		MyData newdata;
		newdata.cameraType = -1;
		newdata.direction = false;
		newdata.objectType = -1;
		newdata.speed = 0;
		newdata.datetime = "";
		if (row.size() > 4)
		{			
			str.Format(_T("%s"), row[0]);
			newdata.cameraType = atoi((char*)(LPCTSTR)str);
			if (newdata.cameraType >= 100) {
				newdata.cameraType -= 100;
			}
			str.Format(_T("%s"), row[1]);
			newdata.objectType = atoi((char*)(LPCTSTR)str);
			str.Format(_T("%s"), row[2]);
			if (atoi((char*)(LPCTSTR)str) == 0)
				newdata.direction = true;
			else if (atoi((char*)(LPCTSTR)str) == 1)
				newdata.direction = false;
			str.Format(_T("%s"), row[3]);
			newdata.speed = atof((char*)(LPCTSTR)str);
			newdata.datetime = row[4];
			mydata.push_back(newdata);
		}
	}

	showResult();
}

void SummaryDlg::showResult()
{
	CString str;
	for (int i = 0; i < 24; i++)
	{
		//camera1
		int count1 = getDaySummary(1, i, 0, true);
		int count2 = getDaySummary(1, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list1.SetItem(i * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list1.SetItem(i * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list1.SetItem(i * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list1.SetItem(i * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);
		//camera2
		count1 = getDaySummary(2, i, 0, true);
		count2 = getDaySummary(2, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list3.SetItem(i * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list3.SetItem(i * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list3.SetItem(i * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list3.SetItem(i * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);
		//camera3
		count1 = getDaySummary(3, i, 0, true);
		count2 = getDaySummary(3, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list5.SetItem(i * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list5.SetItem(i * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list5.SetItem(i * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list5.SetItem(i * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);
		//camera4
		count1 = getDaySummary(4, i, 0, true);
		count2 = getDaySummary(4, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list7.SetItem(i * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list7.SetItem(i * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list7.SetItem(i * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list7.SetItem(i * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);
	}
	for (int i = 1; i < 13; i++)
	{
		//camera1
		int count1 = getMonthSummary(1, i, 0, true);
		int count2 = getMonthSummary(1, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list2.SetItem((i - 1) * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list2.SetItem((i - 1) * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list2.SetItem((i - 1) * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list2.SetItem((i - 1) * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);

		//camera2
		count1 = getMonthSummary(2, i, 0, true);
		count2 = getMonthSummary(2, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list4.SetItem((i - 1) * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list4.SetItem((i - 1) * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list4.SetItem((i - 1) * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list4.SetItem((i - 1) * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);

		//camera3
		count1 = getMonthSummary(3, i, 0, true);
		count2 = getMonthSummary(3, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list6.SetItem((i - 1) * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list6.SetItem((i - 1) * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list6.SetItem((i - 1) * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list6.SetItem((i - 1) * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);

		//camera4
		count1 = getMonthSummary(4, i, 0, true);
		count2 = getMonthSummary(4, i, 0, false);
		str.Format(_T("%d"), count1);
		m_list8.SetItem((i - 1) * 2, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list8.SetItem((i - 1) * 2, 2, LVFIF_TEXT, _T("L"), 0, 0, 0, NULL);
		str.Format(_T("%d"), count2);
		m_list8.SetItem((i - 1) * 2 + 1, 1, LVFIF_TEXT, str, 0, 0, 0, NULL);
		m_list8.SetItem((i - 1) * 2 + 1, 2, LVFIF_TEXT, _T("R"), 0, 0, 0, NULL);
	}
}

int getHour(string datatime)
{
	SYSTEMTIME curTime;
	GetSystemTime(&curTime);
	int curYear = curTime.wYear;
	int curMonth = curTime.wMonth;
	int curDay = curTime.wDay;
	int hour = 0;
	int pos = datatime.find(':');
	string year = datatime.substr(0, pos);//year
	if (std::stoi(year) != curYear) {
		return -1;
	}

	datatime = datatime.substr(pos + 1);
	pos = datatime.find(':');
	string month = datatime.substr(0, pos);//month
	if (std::stoi(month) != curMonth) {
		return -1;
	}

	datatime = datatime.substr(pos + 1);
	pos = datatime.find('*');
	string day = datatime.substr(0, pos);//day
	if (std::stoi(day) != curDay) {
		return -1;
	}

	datatime = datatime.substr(pos + 1);
	pos = datatime.find(':');
	datatime = datatime.substr(0, pos);
	hour = std::stoi(datatime);
	return  hour;
}

int SummaryDlg::getDaySummary(int cameratype, int hour, int objType, bool direction)
{
	int count = 0;
	for (int i = 0; i < mydata.size(); i++)
	{
		if (mydata[i].cameraType == cameratype && getHour(mydata[i].datetime) == hour && mydata[i].objectType == objType && mydata[i].direction == direction)
		{
			count++;
		}
	}
	return count;
}

int getMonth(string datatime)
{
	SYSTEMTIME curTime;
	GetSystemTime(&curTime);
	int curYear = curTime.wYear;
	int curMonth = curTime.wMonth;
	int curDay = curTime.wDay;
	int hour = 0;
	int pos = datatime.find(':');
	string year = datatime.substr(0, pos);//year
	if (std::stoi(year) != curYear) {
		return -1;
	}

	datatime = datatime.substr(pos + 1);
	pos = datatime.find(':');
	string month = datatime.substr(0, pos);//month
	return  std::stoi(month);
}

int SummaryDlg::getMonthSummary(int cameratype, int month, int objType, bool direction)
{
	int count = 0;
	for (int i = 0; i < mydata.size(); i++)
	{
		if (mydata[i].cameraType == cameratype && getMonth(mydata[i].datetime) == month && mydata[i].objectType == objType && mydata[i].direction == direction)
		{
			count++;
		}
	}
	return count;
}
