#pragma once
#include "afxcmn.h"
using namespace std;
#include "stdafx.h"
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include "stdafx.h"
#include "afxdialogex.h"
#include <fstream>
#include <iostream>

struct  MyData
{
	int cameraType;
	int objectType;
	//0-vehicle, 1-human
	bool direction;
	//true-down->up, false-up->down
	float speed;
	string datetime;
};

class CSVRow
{
public:
	std::string const& operator[](std::size_t index) const
	{
		return m_data[index];
	}
	std::size_t size() const
	{
		return m_data.size();
	}
	void readNextRow(std::istream& str)
	{
		std::string         line;
		std::getline(str, line);

		std::stringstream   lineStream(line);
		std::string         cell;

		m_data.clear();
		while (std::getline(lineStream, cell, ','))
		{
			m_data.push_back(cell);
		}
		// This checks for a trailing comma with no data after it.
		if (!lineStream && cell.empty())
		{
			// If there was a trailing comma then add an empty element.
			m_data.push_back("");
		}
	}
private:
	std::vector<std::string>    m_data;
};

// SummaryDlg dialog

class SummaryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SummaryDlg)

public:
	SummaryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SummaryDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUMMARY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	vector<MyData> mydata;
	DECLARE_MESSAGE_MAP()
public:
	//camera1
	CListCtrl m_list1;
	CListCtrl m_list2;
	//camera2
	CListCtrl m_list3;
	CListCtrl m_list4;
	//camera3
	CListCtrl m_list5;
	CListCtrl m_list6;
	//camera4
	CListCtrl m_list7;
	CListCtrl m_list8;

	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	void InitLists();
	void LoadData();
	void showResult();
	int getDaySummary(int, int, int, bool);
	int getMonthSummary(int, int, int, bool);
};

