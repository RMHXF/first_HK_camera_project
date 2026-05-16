#include "DataTypes.hpp"
#include "camera.hpp"
#include <opencv2/opencv.hpp>

int main()
{
  rm::HKCamera camera;

  if (!camera.open())
  {
    std::cerr << "程序异常终止" << std::endl;
    return -1;
  }

  cv::Mat frame;
  std::cout << "开始读取视频流" << std::endl;

  // 主循环
  while (true)
  {

    if (camera.read(frame))
    {
      cv::Mat display_img;
      cv::resize(frame, display_img, cv::Size(640, 480));

      cv::imshow("RM_Vision -- HK_Camera", display_img);
    }
    int key = cv::waitKey(1);
    if (key == 'q' || key == 27)
    {
      break;
    }
  }
  cv::destroyAllWindows();
  std::cout << "程序正常退出" << std::endl;
  return 0;
}