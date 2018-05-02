#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include<cv.h>
#include<highgui.h>
#include<ml.h>
#include<fstream>
#include<string>
#include<vector>
#include"cvaux.h"
#include<ctype.h>
#include<Windows.h>
#include<stdlib.h>
#include<iostream>
#include<atltime.h>
#include<direct.h>
#include<io.h>
using namespace std;
#include<mmsystem.h>
#include<stdio.h>
#pragma comment (lib,"winmm.lib")
using namespace cv;
using namespace std;
void gettime(char* time)
{
	CTime t = CTime::GetCurrentTime();
	int y = t.GetYear();
	int m = t.GetMonth();
	int d = t.GetDay();
	int h = t.GetHour();
	int mm = t.GetMinute();
	int s = t.GetSecond();
	sprintf(time, "%04d%02d%02d%02d%02d%02d", y, m, d, h, mm, s);


}

void mvRationality(vector<CvRect>& V, const IplImage* pImage)
{
	for (int i = 0; i < V.size(); i++)
	{
		int left = max(V[i].x, 0);
		int right = min(V[i].x + V[i].width, pImage->width);
		int top = max(V[i].y, 0);
		int bottom = min(V[i].x + V[i].height, pImage->height);
		V[i].x = left;
		V[i].y = right;
		V[i].width = right - left;
		V[i].height = bottom - top;
	}
}

/*void draw(cv::HOGDescriptor hog, vector<cv::Rect> &found, IplImage* img, IplImage* show)//定义hog对象
{
	hog.detectMultiScale(img, found, 0, cv::Size(4, 4), cv::Size(1, 1), 1.1, 2);
	size_t i, j;
	for (i = 0; i < found.size(); i++)
	{
		Rect r = found[i];
		cvRectangleR(show, Rect(r.x * 2, r.y * 2, r.width * 2, r.height * 2), cv::Scalar(0, 255, 0), 2);

	}
}
*/
void my_mouse_callback(int event, int x, int y, int flags, void* param);
CvRect box;
bool drawing_box = false;
bool drawing_box1 = false;
int samplew = 32;
int sampleh = 32;
bool bSave = false;
bool bSave1 = false;


void draw_box(IplImage* img, CvRect rect)
{
	cvRectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0xff, 0x00, 0xff));

}
char path[128];
char filepath[128];
int main(int argc, char**argv)
{
	int zoom = 8;
	int r = 0;
	int f = 0;
	IplImage* pFrame = NULL;
	cout << "SVM样本生成程序" << endl;
	cout << endl;
	int nMode;
	cout << "输入0为保存检测结果，输入1为手动截取样本" << endl;
	cout << endl;
	cin >> nMode;
	cvNamedWindow("图2");
	int i;
	int nF = 0;
	char number[128];
	char datpath[128];
	int nStartFrame = 1;
	double dZoom = 800.00;
	ifstream Datfile("./SvmPath.txt", ios::in);//文件数据输入到内存中
	Datfile >> datpath >> filepath >> nStartFrame >> dZoom;
	char cDir[128];
	char cDirTxt[128];
	sprintf(cDir, "%sResult", filepath);
	if (_access(cDir, 0) != 0)
		_mkdir(cDir);
	sprintf(cDir, "%sTxts", filepath);
	if (_access(cDirTxt, 0) != 0)
		_mkdir(cDirTxt);
	char cPos[128];
	char cNeg[128];
	sprintf(cPos, "%s\\pos.txt", cDir);
	sprintf(cNeg, "%s\\neg.txt", cDir);
	ofstream posfile(cPos, ios::app);
	ofstream negfile(cNeg, ios::app);
	if (!Datfile)
	{
		cout << "读取失败" << endl;

	}
	Datfile.close();
	FileStorage fileIn_f(datpath, FileStorage::READ);//打开文件地址datpath进行读操作（fileIn_f初试化）
	vector<float> x;
	x.clear();
	FileNode fn = fileIn_f["VECTOR"];
	for (int i = 0; i < fn.size(); i++)
	{
		x.push_back(fn[i]);

	}
	fileIn_f.release();
	vector<cv::Rect> found;
	//cv::HOGDescriptor hog(cv::Size(32,32),cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
	//hog.setSVMDetector(x);
	bool bEnd = false;
	int nT = 1;
	

	for (i = nStartFrame; i < 10000; i++)
	{
		sprintf(path, "%s\\d.jpg", filepath, i);//存入图片地址
		sprintf(number, "%d", i);
		pFrame = cvLoadImage(path);//读入图片

		if (!pFrame)
		{
			cout << "找不到图片序列" << endl;
			break;
		}
		double dScale;
		dScale = (pFrame->width > pFrame->height ? pFrame->width : pFrame->height) / dZoom;//归一化尺寸
		int m = pFrame->width / dScale;
		int n = pFrame->height / dScale;
		IplImage * pFrames = cvCreateImage(cvSize(m, n), IPL_DEPTH_8U, 3);
		cvResize(pFrame, pFrames);
		IplImage * temp = cvCreateImage(cvSize(m, n), IPL_DEPTH_8U, 3);
		cvResize(pFrame, temp);
		double t = (double)getTickCount();
		char savename[128];
		char saveimgs[128];
		if (0 == nMode)
		{
			//draw(hog,found,pFrames,temp);
			t = (double)getTickCount() - t;
			printf("detection time=%gms\n", t*1000. / cv::getTickCount());
			vector<Rect>::iterator it;//定义it 叠加器
			int nName = 0;
			for (it = found.begin(); it != found.end(); it++)//循环found容器里面的东西
			{
				char cDir[128];
				sprintf(cDir, "%sResult", filepath);
				if (_access(cDir, 0) != 0)
					_mkdir(cDir);
				nName++;//取出it里面的xywh用来计算
				CvRect rRect = CvRect(it->x*dScale, it->y*dScale, it->width*dScale, it->height*dScale);
				sprintf(savename, "%s\\%d.jpg", cDir, i);
				cvSetImageROI(pFrame, rRect);//用rRect（x,y,w,h)来裁剪pFRame
				cvSaveImage(savename, pFrame);//存在savename中
				cvResetImageROI(pFrame);//pFrame又变成原来的图，继续循环裁剪下去

			}
			cvShowImage("2", temp);
			if (27 == cvWaitKey(nT))
				bEnd = true;
		}

		if (i == nMode)
		{
			t = (double)getTickCount() - t;
			printf("detection time=%gms\n", t*1000. / cv::getTickFrequency());
			cout << "左键为保存正样本，颜色为紫色" << endl;
			cout << "右键为保存负样本，颜色为绿色" << endl;
			cout << endl;
			vector<CvRect>vRect;
			vector<CvRect>vRect1;
			box = CvRect(-1, -1, 0, 0);
			IplImage* tep = cvCloneImage(temp);
			cvSetMouseCallback("2", my_mouse_callback,(void*)temp);
			sprintf(cDir, "%sResult\\pos", filepath);
			if (_access(cDir, 0) != 0)
				_mkdir(cDir);
			sprintf(cDir, "%sResult\\neg", filepath);
			if (_access(cDir, 0) != 0)
				_mkdir(cDir);
			while (1)
			{
				cvCopy(temp, tep);
				if (drawing_box)
					draw_box(tep, box);
				CvFont font;
				char nframes[100];
				sprintf(nframes, "%d", i);
				cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.7, 0.7, 0, 1, 8);
				cvPutText(temp, nframes, cvPoint(m, 20), &font, CV_RGB(255, 0, 255));
				cvShowImage("2", tep);

				if (bSave)
				{
					vRect.push_back((cvRect(box.x*dScale, box.y*dScale, box.width*dScale, box.height*dScale)));
					bSave = false;
				}
				if (bSave1)
				{
					vRect1.push_back((cvRect(box.x*dScale, box.y*dScale, box.width*dScale, box.height*dScale)));
					bSave1 = false;
				}
				int key = cvWaitKey(nT);
				if (key == 'd')
				{
					break;
				}
				else if (key == 'f')
				{
					if (i > 2)
					{
						i = i - 1;
					}
					break;

				}
				else if (key == 'g')
				{
					vRect.clear();
					vRect.clear();
				}
				else if (key == 27)
				{
					bEnd = true;
					break;
				}
			}
			int nName = 0;
			mvRationality(vRect, pFrame);
			mvRationality(vRect1, pFrame);

			posfile << path << "" << vRect.size() << "";

			char postxtname[256];
			sprintf(postxtname,"sTxts\\%d.txt", filepath, i);
			ofstream fTxtout(postxtname);//写入到硬盘；
			fTxtout << "%bbGt version=3" << endl;
			for (int m = 0; m < vRect.size(); m++);
			{
				sprintf(savename, "%d %d %d %d ", vRect[m].x, vRect[m].y, vRect[m].width, vRect[m].height);
				posfile << savename;
				sprintf(saveimgs, "%sResult/pos/%d_%d.jpg", filepath, i, m);
				fTxtout << "person" << " " << vRect[m].x << " " << vRect[m].y << " " << vRect[m].width << " " << vRect[m].height << " "
					<< 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << endl;
				cvSetImageROI(pFrame, vRect[m]);
				cvSaveImage(saveimgs, pFrame);
				cvResetImageROI(pFrame);
			}
			fTxtout.close();
			posfile << endl;

			negfile << path << " " << vRect1.size() << " ";
			for (int m = 0; m < vRect1.size(); m++)
			{
				sprintf(savename, "%d %d %d %d", vRect1[m].x, vRect1[m].y, vRect1[m].width, vRect1[m].height);
				sprintf(saveimgs, "%sResult/neg/%d_%d.jpg", filepath, i, m);
				cvSetImageROI(pFrame, vRect1[m]);
				cvSaveImage(saveimgs, pFrame);
				cvResetImageROI(pFrame);
				negfile << savename;

			}
			negfile << savename;
			cvReleaseImage(&tep);
		}
		if (bEnd)
		{
			break;
		}
		cvReleaseImage(&pFrame);
		cvReleaseImage(&pFrames);
		cvReleaseImage(&temp);
	}
	system("pause");
	cvDestroyAllWindows;

}
void my_mouse_callback(int event, int x, int y, int flags, void* param)
{

	IplImage* image = (IplImage*)param;
	switch (event)
	{
		//鼠标移动消息
	case CV_EVENT_MOUSEMOVE:
	{
		if (drawing_box)//如果是否进行绘制的标识符为真，则记录下长和宽到RECT型变量中
		{
			box.width = x - box.x;
			box.height = y - box.y;
		}
		if (drawing_box1)//如果是否进行绘制的标识符为真，则记录下长和宽到RECT型变量中
		{
			box.width = x - box.x;
			box.height = y - box.y;
		}
	}
		break;

		//左键按下消息
	case CV_EVENT_LBUTTONDOWN:
	{
								 drawing_box = true;
							  box = Rect(x, y, 0, 0);//记录起始点
	}
		break;

		//左键抬起消息
	case CV_EVENT_LBUTTONUP:
	{
							drawing_box = false;//置标识符为false
							draw_box(image, box);//调用函数进行绘制
							//对宽和高小于0的处理
							if (box.width < 0)
							{
								box.x += box.width;
								box.width *= -1;
							}

							if (box.height < 0)
							{
								box.y += box.height;
								box.height *= -1;
							}
							char filename[100];
							char time[100];
							if (box.width>15 && box.height > 15)
							{
								bSave = true;
							}
			
	}
		break;

	}
}
