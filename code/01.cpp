#include <opencv2/opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

const int imageWidth = 640; //定义图片大小，即摄像头的分辨率  
const int imageHeight = 480;
Size imageSize = Size(imageWidth, imageHeight);
Mat mapx, mapy;
// 相机内参
Mat cameraMatrix = (Mat_<double>(3, 3) << 273.4985, 0, 321.2298,
0, 273.3338, 239.7912,
0, 0, 1);
// 相机外参
Mat distCoeff = (Mat_<double>(1, 4) << -0.3551, 0.1386,0, 0);
Mat R = Mat::eye(3, 3, CV_32F);

VideoCapture cap1; //打开摄像头

void img_init(void)  //初始化摄像头
{
  cap1.set(CAP_PROP_FOURCC, 'GPJM');
  cap1.set(CAP_PROP_FRAME_WIDTH, imageWidth);
  cap1.set(CAP_PROP_FRAME_HEIGHT, imageHeight);
}

int main()
{
  initUndistortRectifyMap(cameraMatrix, distCoeff, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
  Mat frame;
  img_init();
  while (1)
  {
    cap1>>frame;
    imshow("原鱼眼摄像头图像",frame);
    remap(frame,frame,mapx,mapy, INTER_LINEAR);
    imshow("消畸变后",frame);
    waitKey(30);
  }
return 0;
}