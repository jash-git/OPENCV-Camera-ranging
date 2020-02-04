#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <zbar.h>

using namespace cv;
using namespace std;

#define HALF_LENGTH 15   //二维码宽度的二分之一

const int imageWidth = 640; //设置图片大小，即摄像头的分辨率  
const int imageHeight = 480;
Size imageSize = Size(imageWidth, imageHeight);
Mat mapx, mapy;
// 相机内参
Mat cameraMatrix = (Mat_<double>(3, 3) << 273.4985, 0, 321.2298,
0, 273.3338, 239.7912,
0, 0, 1);
// 相机外参
Mat distCoeff = (Mat_<double>(1, 4) << -0.3551, 0.1386, 0, 0);
Mat R = Mat::eye(3, 3, CV_32F);

VideoCapture cap1;

typedef struct   //定义一个二维码对象的结构体
{
  string type;
  string data;
  vector <Point> location;
} decodedObject;

void img_init(void);   
void decode(Mat &im, vector<decodedObject>&decodedObjects);
void display(Mat &im, vector<decodedObject>&decodedObjects);


int main(int argc, char* argv[])
{
  initUndistortRectifyMap(cameraMatrix, distCoeff, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
  img_init();
  namedWindow("yuantu", WINDOW_AUTOSIZE);
  Mat im;

while (waitKey(1) != 'q') {
    cap1 >> im;
if (im.empty())  break;
    remap(im, im, mapx, mapy, INTER_LINEAR);//畸变矫正
    imshow("yuantu", im);

// 已解码对象的变量
    vector<decodedObject> decodedObjects;

// 找到并解码条形码和二维码
    decode(im, decodedObjects);

// 显示位置
    display(im, decodedObjects);
//vector<Point> points_xy = decodedObjects[0].location;  //假设图中就一个二维码对象，将二维码四角位置取出
    imshow("二维码", im);

    waitKey(30);
  }

return EXIT_SUCCESS;
}

void img_init(void)
{
//初始化摄像头
  cap1.open(0);
  cap1.set(CAP_PROP_FOURCC, 'GPJM');
  cap1.set(CAP_PROP_FRAME_WIDTH, imageWidth);
  cap1.set(CAP_PROP_FRAME_HEIGHT, imageHeight);
}
// 找到并解码条形码和二维码
//输入为图像
//返回为找到的条形码对象
void decode(Mat &im, vector<decodedObject>&decodedObjects)
{

// 创建zbar扫描仪
  zbar::ImageScanner scanner;

// 配置扫描仪
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

// 转换图像为灰度图灰度
  Mat imGray;
  cvtColor(im, imGray, COLOR_BGR2GRAY);

// 将图像数据包 装在zbar图像中
//可以参考：https://blog.csdn.net/bbdxf/article/details/79356259
  zbar::Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);

// Scan the image for barcodes and QRCodes
//扫描图像中的条形码和qr码
  int n = scanner.scan(image);

// Print results
for (zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
  {
    decodedObject obj;

    obj.type = symbol->get_type_name();
    obj.data = symbol->get_data();

// Print type and data
//打印
//cout << "Type : " << obj.type << endl;
//cout << "Data : " << obj.data << endl << endl;

// Obtain location
//获取位置
for (int i = 0; i < symbol->get_location_size(); i++)
    {
      obj.location.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
    }

    decodedObjects.push_back(obj);
  }
}
// 显示位置  
void display(Mat &im, vector<decodedObject>&decodedObjects)
{
// Loop over all decoded objects
//循环所有解码对象
for (int i = 0; i < decodedObjects.size(); i++)
  {
    vector<Point> points = decodedObjects[i].location;
    vector<Point> hull;

// If the points do not form a quad, find convex hull
//如果这些点没有形成一个四边形，找到凸包
if (points.size() > 4)
      convexHull(points, hull);
else
      hull = points;
    vector<Point2f> pnts;
// Number of points in the convex hull
//凸包中的点数
    int n = hull.size();

for (int j = 0; j < n; j++)
    {
      line(im, hull[j], hull[(j + 1) % n], Scalar(255, 0, 0), 3);
      pnts.push_back(Point2f(hull[j].x, hull[j].y));
    }
    vector<Point3f> obj = vector<Point3f>{
        cv::Point3f(-HALF_LENGTH, -HALF_LENGTH, 0),  //tl
        cv::Point3f(HALF_LENGTH, -HALF_LENGTH, 0),  //tr
        cv::Point3f(HALF_LENGTH, HALF_LENGTH, 0),  //br
        cv::Point3f(-HALF_LENGTH, HALF_LENGTH, 0)  //bl
    };   //自定义二维码四个点坐标
    cv::Mat rVec = cv::Mat::zeros(3, 1, CV_64FC1);//init rvec 
    cv::Mat tVec = cv::Mat::zeros(3, 1, CV_64FC1);//init tvec
    solvePnP(obj, pnts, cameraMatrix, distCoeff, rVec, tVec, false, SOLVEPNP_ITERATIVE);
    cout << "tvec:\n " << tVec << endl;
  }
}