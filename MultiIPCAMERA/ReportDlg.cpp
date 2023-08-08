// ReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiIPCAMERA.h"
#include "ReportDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <iterator>
#include <sstream>
using namespace std;

// CReportDlg dialog

IMPLEMENT_DYNAMIC(CReportDlg, CDialogEx)

CReportDlg::CReportDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REPORT, pParent)
{

}

CReportDlg::~CReportDlg()
{
}

void CReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Chart1, m_chartviewer1);
	DDX_Control(pDX, IDC_Chart2, m_chartviewer2);
	DDX_Control(pDX, IDC_Chart3, m_chartviewer3);
	DDX_Control(pDX, IDC_Chart4, m_chartviewer4);
	DDX_Control(pDX, IDC_Chart5, m_chartviewer5);
	DDX_Control(pDX, IDC_Chart6, m_chartviewer6);
	DDX_Control(pDX, IDC_Chart7, m_chartviewer7);
	DDX_Control(pDX, IDC_Chart8, m_chartviewer8);
}


BEGIN_MESSAGE_MAP(CReportDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CReportDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CReportDlg message handlers


void CReportDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


BOOL CReportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	InitData();
	LoadData();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CReportDlg::InitData() {
	mydata.clear();
}

std::vector<std::string> getNextLineAndSplitIntoTokens1(std::istream& str)
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

std::istream& operator >> (std::istream& str, CSVRow1& data)
{
	data.readNextRow(str);
	return str;
}

void CReportDlg::LoadData()
{
	std::ifstream file("config/result.csv");
	CSVRow1 row;
	CString str;
	while (file >> row)
	{
		MyData1 newdata;
		newdata.cameraType = -1;
		newdata.direction = false;
		newdata.objectType = -1;
		newdata.speed = 0;
		newdata.timedata = "";
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
			newdata.timedata = row[4];
			mydata.push_back(newdata);
		}
	}
	showResult();
}


void CReportDlg::showResult() {
	CString str;
	//camera1
	double data0[24];
	double data1[24];
	//camera2
	double data2[24];
	double data3[24];
	//camera3
	double data4[24];
	double data5[24];
	//camera4
	double data6[24];
	double data7[24];

	for (int i = 0; i < 24; i++)
	{
		//camera1
		int count1 = getDaySummary(1, i, 0, true);
		data0[i] = count1;
		int count2 = getDaySummary(1, i, 0, false);
		data1[i] = count2;

		//camera2
		count1 = getDaySummary(2, i, 0, true);
		data2[i] = count1;
		count2 = getDaySummary(2, i, 0, false);
		data3[i] = count2;

		//camera3
		count1 = getDaySummary(3, i, 0, true);
		data4[i] = count1;
		count2 = getDaySummary(3, i, 0, false);
		data5[i] = count2;

		//camera4
		count1 = getDaySummary(4, i, 0, true);
		data6[i] = count1;
		count2 = getDaySummary(4, i, 0, false);
		data7[i] = count2;
	}
	
	const char *labels[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
		"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" };

	//show chart1-camera1-day
	XYChart *c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels, (int)(sizeof(labels) / sizeof(labels[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data0, (int)(sizeof(data0) / sizeof(data0[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data1, (int)(sizeof(data1) / sizeof(data1[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer1.setChart(c);  //m_chartView

	//show chart2-camera2-day
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels, (int)(sizeof(labels) / sizeof(labels[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data2, (int)(sizeof(data2) / sizeof(data2[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data3, (int)(sizeof(data3) / sizeof(data3[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer3.setChart(c);  //m_chartView

	//show chart3-camera3-day
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels, (int)(sizeof(labels) / sizeof(labels[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data4, (int)(sizeof(data4) / sizeof(data4[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data5, (int)(sizeof(data5) / sizeof(data5[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer5.setChart(c);  //m_chartView

	//show chart4-camera4-day
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels, (int)(sizeof(labels) / sizeof(labels[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data6, (int)(sizeof(data6) / sizeof(data6[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data7, (int)(sizeof(data7) / sizeof(data7[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer7.setChart(c);  //m_chartView

	const char *labels1[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12" };
	//camera1
	double data8[12];
	double data9[12];
	//camera2
	double data10[12];
	double data11[12];
	//camera3
	double data12[12];
	double data13[12];
	//camera4
	double data14[12];
	double data15[12];

	for (int i = 1; i < 13; i++)
	{
		//camera1
		int count1 = getMonthSummary(1, i, 0, true);
		data8[i - 1] = count1;
		int count2 = getMonthSummary(1, i, 0, false);
		data9[i - 1] = count2;
		
		//camera2
		count1 = getMonthSummary(2, i, 0, true);
		data10[i - 1] = count1;
		count2 = getMonthSummary(2, i, 0, false);
		data11[i - 1] = count2;

		//camera3
		count1 = getMonthSummary(3, i, 0, true);
		data12[i - 1] = count1;
		count2 = getMonthSummary(3, i, 0, false);
		data13[i - 1] = count2;

		//camera4
		count1 = getMonthSummary(4, i, 0, true);
		data14[i - 1] = count1;
		count2 = getMonthSummary(4, i, 0, false);
		data15[i - 1] = count2;
	}
	//camera1
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels1, (int)(sizeof(labels1) / sizeof(labels1[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data8, (int)(sizeof(data8) / sizeof(data8[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data9, (int)(sizeof(data9) / sizeof(data9[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer2.setChart(c);  //m_chartView
								 //camera1
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels1, (int)(sizeof(labels1) / sizeof(labels1[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data10, (int)(sizeof(data10) / sizeof(data10[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data11, (int)(sizeof(data11) / sizeof(data11[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer4.setChart(c);  //m_chartView
//camera3
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels1, (int)(sizeof(labels1) / sizeof(labels1[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data12, (int)(sizeof(data12) / sizeof(data12[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data13, (int)(sizeof(data13) / sizeof(data13[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer6.setChart(c);  //m_chartView
//camera4
	c = new XYChart(350, 130); //chart size
	c->setPlotArea(50, 30, 250, 70); //chart position
	c->addLegend(55, 0, false, "", 8)->setBackground(Chart::Transparent);
	// Add a title to the x axis
	c->xAxis()->setTitle("Hour");
	// Add a title to the y axis
	c->yAxis()->setTitle("VehicleCount");
	// Set the labels on the x axis.
	c->xAxis()->setLabels(StringArray(labels1, (int)(sizeof(labels1) / sizeof(labels1[0]))));
	// Display 1 out of 2 labels on the x-axis. Show minor ticks for remaining
	// labels.
	c->xAxis()->setLabelStep(2, 1);
	// Add three area layers, each representing one data set. The areas are drawn in
	// semi-transparent colors.
	c->addAreaLayer(DoubleArray(data14, (int)(sizeof(data14) / sizeof(data14[0]))),
		0x808080ff, "Left", 1);
	c->addAreaLayer(DoubleArray(data15, (int)(sizeof(data15) / sizeof(data15[0]))),
		0x80ff0000, "Right", 1);
	m_chartviewer8.setChart(c);  //m_chartView
}

int getHour1(string datatime)
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

int CReportDlg::getDaySummary(int cameratype, int hour, int objType, bool direction)
{
	int count = 0;
	for (int i = 0; i < mydata.size(); i++)
	{
		if (mydata[i].cameraType == cameratype && getHour1(mydata[i].timedata) == hour && mydata[i].objectType == objType && mydata[i].direction == direction)
		{
			count++;
		}
	}
	return count;
}

int getMonth1(string datatime)
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

int CReportDlg::getMonthSummary(int cameratype, int month, int objType, bool direction)
{
	int count = 0;
	for (int i = 0; i < mydata.size(); i++)
	{
		if (mydata[i].cameraType == cameratype && getMonth1(mydata[i].timedata) == month && mydata[i].objectType == objType && mydata[i].direction == direction)
		{
			count++;
		}
	}
	return count;
}
