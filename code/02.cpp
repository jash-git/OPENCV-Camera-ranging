#include <zbar.h>
#include <opencv2\opencv.hpp>
#include <iostream>

int main(int argc, char*argv[])
{
  zbar::ImageScanner scanner;
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
  cv::VideoCapture capture;
  capture.open(0);  //打开摄像头
  cv::Mat image;
  cv::Mat imageGray;
std::vector<cv::Point2f> obj_location;
bool flag = true;

if (!capture.isOpened())
  {
std::cout << "cannot open cam!" << std::endl;
  }
else
  {
while (flag)
    {
      capture >> image;
      cv::cvtColor(image, imageGray, CV_RGB2GRAY);
int width = imageGray.cols;
int height = imageGray.rows;
      uchar *raw = (uchar *)imageGray.data;
      zbar::Image imageZbar(width, height, "Y800", raw, width * height);
      scanner.scan(imageZbar);  //扫描条码
      zbar::Image::SymbolIterator symbol = imageZbar.symbol_begin();
if (imageZbar.symbol_begin() != imageZbar.symbol_end())  //如果扫描到二维码
      {
        flag = false;
//解析二维码
for (int i = 0; i < symbol->get_location_size(); i++)
        {
          obj_location.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
        }
for (int i = 0; i < obj_location.size(); i++)
        {
          cv::line(image, obj_location[i], obj_location[(i + 1) % obj_location.size()], cv::Scalar(255, 0, 0), 3);//定位条码
        }
for (; symbol != imageZbar.symbol_end(); ++symbol)
        {
std::cout << "Code Type: " << std::endl << symbol->get_type_name() << std::endl; //获取条码类型
std::cout << "Decode Result: " << std::endl << symbol->get_data() << std::endl;  //解码
        }
        imageZbar.set_data(NULL, 0);
      }
      cv::imshow("Result", image);
      cv::waitKey(50);
    }
    cv::waitKey();
  }
return 0;
}