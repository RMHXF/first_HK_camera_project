#pragma once
#include "opencv2/opencv.hpp"
#include <string>

namespace rm
{
    class HKCamera
    {
    public:
        // 构造函数和析构函数
        HKCamera();
        ~HKCamera();

        // 禁用拷贝构造和赋值操作 (硬件设备不能被随便复制)
        HKCamera(const HKCamera &) = delete;
        HKCamera &operator=(const HKCamera &) = delete;

        /**
         * @brief 打开并初始化相机
         * @return true: 成功, false: 失败
         */
        [[nodiscard]] bool open();

        /**
         * @brief 获取一帧图像
         * @param image 输出的 OpenCV Mat
         * @return true: 成功, false: 失败或超时
         */
        bool read(cv::Mat &image);

        /**
         * @brief 关闭相机释放资源 (析构函数会自动调用)
         */
        void close();

    private:
        void *handle_; // 相机句柄
        bool is_open_; // 记录相机状态
    };

}