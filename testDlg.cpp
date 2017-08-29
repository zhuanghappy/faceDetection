
// testDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

string result = "result.txt";
ofstream out(result , ios::out);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtestDlg 对话框




CtestDlg::CtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Open, &CtestDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_Next, &CtestDlg::OnBnClickedNext)
	//ON_BN_CLICKED(IDC_Process, &CtestDlg::OnBnClickedProcess)
	ON_BN_CLICKED(IDC_Close, &CtestDlg::OnBnClickedClose)
	//ON_BN_CLICKED(IDC_Close, &CtestDlg::OnBnClickedClose)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CtestDlg 消息处理程序

BOOL CtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码0
	ImageProcess= AfxBeginThread(ImageProcessPro,this,THREAD_PRIORITY_NORMAL,0, CREATE_SUSPENDED,NULL);//线程入口
	flag=false;//循环终止标志位
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestDlg::DrawcvMat(cv::Mat m_cvImg, UINT ID)
{ 
	if (ID == NULL || m_cvImg.empty())
		return;



	//Get DC of your win control
	//	CClientDC hDC(WinCtrl);
	CDC *pDc=GetDlgItem(ID)->GetDC();
	// This is the rectangle where the control is defined
	// and where the image will appear
	RECT rr;
	GetDlgItem(ID)->GetClientRect(&rr);
	//rr.top AND rr.left are always 0
	int rectWidth = rr.right;
	int rectHeight = rr.bottom;

	///------------------------------------
	/// DWORD ALIGNMENT AND CONTINOUS MEMORY
	/// The image must be padded 4bytes and must be continuous

	int border = 0;
	cv::Mat img;
	cv::resize(m_cvImg,img,cv::Size(rectWidth,rectHeight));

	int bpp = 8 * img.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
	{
		border = 4 - (img.cols % 4);
	}

	cv::Mat tmpImg;
	if (border > 0 || img.isContinuous() == false)
	{
		// Adding needed columns on the right (max 3 px)
		cv::copyMakeBorder(img, tmpImg, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		tmpImg = img;
	}

	///----------------------
	/// PREPARE BITMAP HEADER
	/// The header defines format and shape of the source bitmap in memory ... this will produce needed bmi

	uchar buffer[sizeof(BITMAPINFO) + 256 * 4];
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = tmpImg.cols;
	bmih->biHeight = -tmpImg.rows;// DIB are bottom ->top
	bmih->biPlanes = 1;
	bmih->biBitCount = bpp;
	bmih->biCompression = BI_RGB;

	//Sets the palette if image is grey scale
	if (bpp == 8)
	{
		RGBQUAD* palette = bmi->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}

	/// -----------
	/// Draw to DC

	if (tmpImg.cols == rectWidth  && tmpImg.rows == rectHeight)
	{
		// source and destination have same size
		// transfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width

		SetDIBitsToDevice(pDc->GetSafeHdc(),
			//destination rectangle
			0, 0, rectWidth, rectHeight,
			0, 0, 0, tmpImg.rows,
			tmpImg.data, bmi, DIB_RGB_COLORS);
	}
	else
	{
		// Image is bigger or smaller than into destination rectangle
		// we use stretch in full rect

		// destination rectangle
		int destx = 0, desty = 0;
		int destw = rectWidth;
		int desth = rectHeight;

		// rectangle defined on source bitmap
		// using imgWidth instead of tmpImg.cols will ignore the padding border
		int imgx = 0, imgy = 0;
		int imgWidth = tmpImg.cols - border;
		int imgHeight = tmpImg.rows;

		StretchDIBits(pDc->GetSafeHdc(),
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			tmpImg.data, bmi, DIB_RGB_COLORS, SRCCOPY);
	}

}

void CtestDlg::GetFileFromDir(CString csDirPath)  
{  
	csDirPath+="\\*.jpg";  
	HANDLE file;  
	WIN32_FIND_DATA fileData;  
	char line[1024];  
	char fn[1000];  
	//mbstowcs(fn,csDirPath.GetBuffer(),999);  
	file = FindFirstFile(csDirPath.GetBuffer(), &fileData);  
	m_FileList.push_back(fileData.cFileName);  
	bool bState = false;  
	bState = FindNextFile(file, &fileData);  
	while(bState){  
		//wcstombs(line,(const char*)fileData.cFileName,259);  
		m_FileList.push_back(fileData.cFileName);  
		bState = FindNextFile(file, &fileData);  
	}  
} 

void CtestDlg::OnBnClickedOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	//cv::Mat img = cv::imread("1.jpg");
	//DrawcvMat(img,IDC_Pic);
	//载入四个人脸特征分类器文件，可以从opencv的安装目录中找到  
	if( mFaceDetector.empty() )  
		mFaceDetector.load( "haarcascade_frontalface_default.xml" );  
	if( mEyeDetector.empty() )  
		mEyeDetector.load( "haarcascade_mcs_eyepair_big.xml" );  
	if( mNoseDetector.empty() )  
		mNoseDetector.load("haarcascade_mcs_nose.xml" );  
	if( mMouthDetector.empty() )  
		mMouthDetector.load( "haarcascade_mcs_mouth.xml" );  
	//载入性别识别训练集

	svm.load("SVM_SEX_Model.xml");

	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框  

	if(dlg.DoModal()==IDOK)  
		m_csFileName = dlg.GetPathName();  
	else  
		m_csFileName.Empty();  

	int iEndPos = 0;  
	iEndPos = m_csFileName.ReverseFind('\\');  
	csDirParth = m_csFileName.Left(iEndPos);  

	// m_FileList.clear();  
	GetFileFromDir(csDirParth);

	//str = m_FileList[0].GetBuffer();
	//str1 = csDirParth.GetBuffer();
	//str2 = "\\";
	//str3 = str1 + str2 + str;
	str3 = m_csFileName.GetBuffer();
	imageSrc = imread(str3);
	//out <<setw(12)<<str3-str2-str1 << "\t";
	DrawcvMat(imageSrc, IDC_Pic);
	
	out<<"图片文件名称   "<<"性别    "<<"眼镜   "<<setw(4)<<"镜片颜色 "<<setw(2)<<"口罩  "<<setw(4)<<"口罩颜色  "<<setw(2)<<"帽子  "<<setw(4)<<"帽子颜色 "<<"\n";
}


void CtestDlg::OnBnClickedNext()
{
	// TODO: 在此添加控件通知处理程序代码
	//imageProcess(imageSrc,mElabImage);
	ImageProcess -> ResumeThread () ;//开启线程
	/*for(i = 0;i<m_FileList.size();i++)
	{	

		str = m_FileList[i].GetBuffer();
		str1 = csDirParth.GetBuffer();
		str2 = "\\";
		str3 = str1 + str2 + str;
		imageSrc = imread(str3);
		DrawcvMat(imageSrc, IDC_Pic);

		imageProcess(imageSrc,mElabImage);
	}*/
	/*else
	{
		imageProcess(imageSrc,mElabImage);
		//str = m_FileList[--i].GetBuffer();
		//str1 = csDirParth.GetBuffer();
		//str2 = "\\";
		//str3 = str1 + str2 + str;
		//imageSrc = imread(str3);
		//DrawcvMat(imageSrc, IDC_Pic);
		//timeBegin = timeGetTime();   
		//do   
		//{  
		//	timeEnd = timeGetTime();  
		//	//cout<< timeEnd <<endl;  
		//} while (timeEnd - timeBegin <= 1000);
		//imageProcess(imageSrc,mElabImage);

		str = m_FileList[0].GetBuffer();
		str1 = csDirParth.GetBuffer();
		str2 = "\\";
		str3 = str1 + str2 + str;
		imageSrc = imread(str3);
		DrawcvMat(imageSrc, IDC_Pic);
		//timeBegin = timeGetTime();  
		//do   
		//{  
		//	timeEnd = timeGetTime();  
		//	//cout<< timeEnd <<endl;  
		//} while (timeEnd - timeBegin <= 1000);
		i = 1;
	}*/
}




void CtestDlg::imageProcess(Mat imageSrc, Mat mElabImage)
{
	out <<setw(12)<<str << "\t";
	//resize(imageSrc,imageSrc,Size(112,92));
	imageSrc.copyTo(frame);
	frame.copyTo( mElabImage );
	Point2f mouthcenter,nosecenter,eyecenter;
	/**********基于SVM性别测试**********/
	frame.copyTo( testImage );
	resize(testImage, testImage, Size(64, 64));

	HOGDescriptor *hog = new HOGDescriptor(cvSize(64, 64), cvSize(16, 16),
		cvSize(8, 8), cvSize(8, 8), 9);
	vector<float> descriptor;
	hog->compute(testImage, descriptor, Size(1, 1), Size(0, 0));

	Mat testHog = Mat::zeros(1, descriptor.size(), CV_32FC1);
	int n = 0;
	for (vector<float>::iterator iter = descriptor.begin(); iter != descriptor.end(); iter++)
	{
		testHog.at<float>(0, n) = *iter;
		n++;
	}
	int predictResult = (int)svm.predict(testHog);
	//输出性别测试结果
	if(predictResult==1)
	{
		out<<"男"<<"\t";
	}
	if(predictResult==2)
	{
		out<<"女"<<"\t";
	}

	/**********人脸_眼睛_鼻子_嘴巴检测**********/
	//检测脸  
	//缩放因子  
	float scaleFactor = 3.0f;   
	vector< cv::Rect > faceVec;  
	mFaceDetector.detectMultiScale( frame, faceVec, scaleFactor );  
	int a = 0;
	int j; 
	if(faceVec.size() == 0)
	{
		cout<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<endl;
		out<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\n";
	}  
	else
	{
		cv::rectangle( mElabImage, faceVec[a], CV_RGB(255,0,0), 2 );  
		cv::Mat face = frame( faceVec[a] ); 

		//检测眼睛  
		vector< cv::Rect > eyeVec;  
		mEyeDetector.detectMultiScale( face, eyeVec );  
		float halfeye = 0;
		for(j=0; j < eyeVec.size(); j++ )  
		{  
			cv::Rect rect1 = eyeVec[j];  
			rect1.x += faceVec[a].x;  
			rect1.y += faceVec[a].y;  

			cv::rectangle( mElabImage, rect1, CV_RGB(0,255,0), 2 );  
			eyecenter = getCenterPoint(rect1);
			halfeye = rect1.width/2;
		}
		//检测鼻子  
		vector< cv::Rect > noseVec;  

		mNoseDetector.detectMultiScale( face, noseVec, 3 );  

		for(j=0; j < noseVec.size(); j++ )  
		{  
			cv::Rect rect2 = noseVec[j];  
			rect2.x += faceVec[a].x;  
			rect2.y += faceVec[a].y;  

			cv::rectangle( mElabImage, rect2, CV_RGB(0,0,255), 2 ); 
			nosecenter = getCenterPoint(rect2);
		}

		//检测嘴巴  
		vector< cv::Rect > mouthVec;  
		cv::Rect halfRect = faceVec[a];  
		halfRect.height /= 2;  
		halfRect.y += halfRect.height;  
		float halfmouth = 0;
		cv::Mat halfFace = frame( halfRect );  

		mMouthDetector.detectMultiScale( halfFace, mouthVec, 3 );  

		for( j=0; j < mouthVec.size(); j++ )  
		{  
			cv::Rect rect3 = mouthVec[j];  
			rect3.x += halfRect.x;  
			rect3.y += halfRect.y;  

			cv::rectangle( mElabImage, rect3, CV_RGB(255,255,255), 2 );  
			mouthcenter = getCenterPoint(rect3);
			halfmouth = rect3.width/2;
		} 


		/*****************开始按要求检测判断***************/
		int dectPoint_x,dectPoint_y;//用于定位检测点的坐标

		//检测墨镜
		if((eyeVec.size() == 0 && noseVec.size() !=0 && mouthVec.size() !=0 ))
		{
			//计算嘴巴和鼻子的距离
			float juli;
			juli = mouthcenter.y - nosecenter.y;
			Point2f eye1;
			eye1.x = nosecenter.x - halfmouth;
			eye1.y = nosecenter.y - juli;
			//cout<<"检测的墨镜点"<<eye1.x<<","<<eye1.y;
			dectPoint_x = eye1.x;
			dectPoint_y = eye1.y;
			int b= mElabImage.at<cv::Vec3b>(dectPoint_y,dectPoint_x)[0];
			int g= mElabImage.at<cv::Vec3b>(dectPoint_y,dectPoint_x)[1];
			int r= mElabImage.at<cv::Vec3b>(dectPoint_y,dectPoint_x)[2];
			BGR bgr;
			bgr.b = b;
			bgr.g = g;
			bgr.r = r;
			HSV hsv;
			BGR2HSV(bgr,hsv);
			cout<<"是"<<"\t";
			out<<"是"<<"\t";
			if(((hsv.h>=0 && hsv.h<=30)||(hsv.h>=330 && hsv.h<360)) && hsv.v>0.2)//红色
			{
				out<<"透明色"<<"\t";
			}
			else
			{
				int sunglassescolor = 0;//墨镜颜色标识位
				sunglassescolor = colorDetection(hsv);
			}
			cout<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<endl;
			out<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\n";
		}
		//检测口罩
		if((eyeVec.size() != 0 && noseVec.size() != 0 && mouthVec.size() == 0) || (eyeVec.size() != 0 && noseVec.size() == 0 && mouthVec.size() == 0))
		{
			cout<<"否"<<"\t"<<"无"<<"\t";
			out<<"否"<<"\t"<<"无"<<"\t";
			Point2f mouth1;
			mouth1.x = eyecenter.x;
			mouth1.y = eyecenter.y + 2 * halfeye;
			//cout<<"检测的口罩点"<<mouth1.x<<","<<mouth1.y;
			dectPoint_x = mouth1.x;
			dectPoint_y = mouth1.y;
			int b= mElabImage.at<cv::Vec3b>(dectPoint_y,dectPoint_x)[0];
			int g= mElabImage.at<cv::Vec3b>(dectPoint_y,dectPoint_x)[1];
			int r= mElabImage.at<cv::Vec3b>(dectPoint_y,dectPoint_x)[2];
			BGR bgr;
			bgr.b = b;
			bgr.g = g;
			bgr.r = r;
			HSV hsv;
			BGR2HSV(bgr,hsv);
			
			cout<<"是"<<"\t";
			out<<"是"<<"\t";
			int maskcolor = 0;//口罩颜色标识位
			maskcolor = colorDetection(hsv);
			cout<<"否"<<"\t"<<"无"<<endl;
			out<<"否"<<"\t"<<"无"<<"\n";
		}
		if((eyeVec.size() != 0 && noseVec.size() !=0 && mouthVec.size() != 0) || (eyeVec.size() != 0 && noseVec.size() == 0 && mouthVec.size() != 0) ||(eyeVec.size() == 0 && noseVec.size() == 0 && mouthVec.size() == 0))
		{
			cout<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<endl;
			out<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\n";
		}
		if((eyeVec.size() == 0 && noseVec.size() !=0 && mouthVec.size() == 0)|| (eyeVec.size() == 0 && noseVec.size() ==0 && mouthVec.size() != 0))
		{
			cout<<"是"<<"\t"<<"黑色"<<"\t"<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<endl;
			out<<"是"<<"\t"<<"黑色"<<"\t"<<"否"<<"\t"<<"无"<<"\t"<<"否"<<"\t"<<"无"<<"\n";
		}
	}
	DrawcvMat(mElabImage, IDC_Pic);
	timeBegin = timeGetTime();  
	timeEnd = 0;  
	do   
	{  
		timeEnd = timeGetTime();  
		//cout<< timeEnd <<endl;  
	} while (timeEnd - timeBegin <= 1000);
}


bool CtestDlg::IsEquals(double val1, double val2)
{
	return fabs(val1 - val2) < 0.001;
}

void CtestDlg::BGR2HSV(BGR &bgr, HSV &hsv)
{
	double b, g, r;
	double h, s, v;
	double min, max;
	double delta;

	b = bgr.b / 255.0;
	g = bgr.g / 255.0;
	r = bgr.r / 255.0;

	if (r > g)
	{
		max = MAX(r, b);
		min = MIN(g, b);
	}
	else
	{
		max = MAX(g, b);
		min = MIN(r, b);
	}

	v = max;
	delta = max - min;

	if (IsEquals(max, 0))
	{
		s = 0.0;
	}
	else
	{
		s = delta / max;
	}

	if (max == min)
	{
		h = 0.0;
	}
	else
	{
		if (IsEquals(r, max) && g >= b)
		{
			h = 60 * (g - b) / delta + 0;
		}
		else if (IsEquals(r, max) && g < b)
		{
			h = 60 * (g - b) / delta + 360;
		}
		else if (IsEquals(g, max))
		{
			h = 60 * (b - r) / delta + 120;
		}
		else if (IsEquals(b, max))
		{
			h = 60 * (r - g) / delta + 240;
		}
	}

	hsv.h = (int)(h + 0.5);
	hsv.h = (hsv.h > 359) ? (hsv.h - 360) : hsv.h;
	hsv.h = (hsv.h < 0) ? (hsv. h + 360) : hsv.h;
	hsv.s = s;
	hsv.v = v;

}

//基于HSI色彩空间的颜色检测
int CtestDlg::colorDetection(HSV &hsv)
{
	int color;
	if(((hsv.h>=0 && hsv.h<=30)||(hsv.h>=330 && hsv.h<360)) && hsv.v>0.2)//红色
	{
		color = 1;
		out<<"红色"<<"\t";
	}
	else if(hsv.h>30 && hsv.h<=90 && hsv.v>0.2)//黄色
	{
		color = 2;
		out<<"黄色"<<"\t";
	}
	else if(hsv.h>90 && hsv.h<=150 && hsv.v>0.2)//绿色
	{
		color = 3;
		out<<"绿色"<<"\t";
	}
	else if(hsv.h>150 && hsv.h<=210 && hsv.v>0.2)//青色
	{
		color = 4;
		out<<"青色"<<"\t";
	}
	else if(hsv.h>210 && hsv.h<=270 && hsv.v>0.2)//蓝色
	{
		color = 5;
		out<<"蓝色"<<"\t";
	}
	else if(hsv.h>=270 && hsv.h<=330 && hsv.v>0.2)//紫色
	{
		color = 6;
		out<<"紫色"<<"\t";
	}
	else if(hsv.v>=0 && hsv.v<= 0.2)//黑色
	{
		color = 7;
		out<<"黑色"<<"\t";
	}
	return color; 
}

//获取矩形中心点
Point2f CtestDlg::getCenterPoint(Rect rect)
{
	Point2f cpt;  
	cpt.x = rect.x + cvRound(rect.width/2.0);  
	cpt.y = rect.y + cvRound(rect.height/2.0);  
	return cpt; 
}

UINT CtestDlg::ImageProcessPro(LPVOID pParam)
{
	CtestDlg* This= (CtestDlg*)pParam;
	for(This->i = 0;This->i<This->m_FileList.size();This->i++)
	{	
		if(This ->flag)
		{
			break;
		}
		This->str = This->m_FileList[This->i].GetBuffer();
		This->str1 = This->csDirParth.GetBuffer();
		This->str2 = "\\";
		This->str3 = This->str1 + This->str2 + This->str;
		This->imageSrc = imread(This->str3);
		This->DrawcvMat(This->imageSrc, IDC_Pic);
		This->imageProcess(This->imageSrc,This->mElabImage);
	}
	return 0;
}


//停止循环按钮
void CtestDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码

	//ImageProcess->SuspendThread();
	flag = true;
}

//对话框自带关闭按钮
void CtestDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//ImageProcess->SuspendThread();
	flag = true;
	CDialogEx::OnClose();
}
