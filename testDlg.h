
// testDlg.h : 头文件
//

#pragma once
#pragma comment(lib, "winmm.lib") 
#include <MMSystem.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector> 
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui//highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>  
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/ml/ml.hpp>
#include <math.h>
using namespace std;
using namespace cv;
// CtestDlg 对话框
class CtestDlg : public CDialogEx
{
// 构造
public:
	CtestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void DrawcvMat(cv::Mat m_cvImg, UINT ID);
	void GetFileFromDir(CString csDirPath); 
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedClose();
	afx_msg void OnClose();
	static UINT ImageProcessPro(LPVOID pParam);
	CWinThread* ImageProcess;
	bool flag;
private:
	CString m_csFileName;//图片完整路径名
	vector<CString> m_FileList;//图片名称
    CString csDirParth; //文件夹路径
	//CString cs;//CString m_FileList 转 String中间变量
	string str;//m_FileList的String格式
	string str1;//csDirParth的String格式
	string str2;//"\\"
	string str3;//完整路径名str3= str1 + str2 + str
	int i;//循环遍历变量
	Mat imageSrc;  //文件打开的图片
	cv::Mat frame; // 当前载入的测试图片 
	cv::Mat mElabImage;//备份frame图像用于人脸检测  
	cv::Mat testImage;//用于性别检测的图片
	DWORD timeBegin;
	DWORD timeEnd;
	struct BGR
	{
		uchar b;
		uchar g;
		uchar r;
	};

	struct HSV
	{
		int h;
		double s;
		double v;
	};
		cv::CascadeClassifier mFaceDetector;  
		cv::CascadeClassifier mEyeDetector;  
		cv::CascadeClassifier mMouthDetector;  
		cv::CascadeClassifier mNoseDetector; 
		//cv::CascadeClassifier mSexDetector;
		CvSVM svm;
public:
	bool IsEquals(double val1, double val2);
	void BGR2HSV(BGR &bgr, HSV &hsv);
	//基于HSI色彩空间的颜色检测
	int colorDetection(HSV &hsv);
	//获取矩形中心点
	Point2f getCenterPoint(Rect rect);
	//图像处理函数
	void imageProcess(Mat imageSrc, Mat mElabImage);

};
