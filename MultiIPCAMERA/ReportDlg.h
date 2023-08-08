#pragma once
#include "afxwin.h"
#include "ChartViewer.h"
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

struct  MyData1
{
	int cameraType;
	int objectType;
	//0-vehicle, 1-human
	bool direction;
	//true-down->up, false-up->down
	float speed;
	string timedata;
};

class CSVRow1
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

// CReportDlg dialog

class CReportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReportDlg)

public:
	CChartViewer m_chartviewer;
	CReportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReportDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	vector<MyData1> mydata;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	void InitData();
	void LoadData();
	void showResult();
	int getDaySummary(int, int, int, bool);
	int getMonthSummary(int, int, int, bool);
	virtual BOOL OnInitDialog();
	CChartViewer m_chartviewer1;
	CChartViewer m_chartviewer2;
	CChartViewer m_chartviewer3;
	CChartViewer m_chartviewer4;
	CChartViewer m_chartviewer5;
	CChartViewer m_chartviewer6;
	CChartViewer m_chartviewer7;
	CChartViewer m_chartviewer8;
};
