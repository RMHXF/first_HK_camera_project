#include "camera.hpp"
#include "MvCameraControl.h" //底层依赖只在 cpp 文件中包含

namespace rm
{

    HKCamera::HKCamera() : handle_(nullptr), is_open_(false) {}

    HKCamera::~HKCamera()
    {
        close(); // 析构时关闭相机
    }

    bool HKCamera::open()
    {
        if (is_open_)
            return true;
        int nRet = MV_OK;

        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (nRet != MV_OK || stDeviceList.nDeviceNum <= 0)
        {
            std::cerr << "[ERROR]   未找到海康相机或枚举失败！ Errorcode: " << nRet << std::endl;
            return false;
        }
        std::cout << "[INFO]   找到 " << stDeviceList.nDeviceNum << " 个相机！" << std::endl;

        // 2 创建句柄(默认使用第0个设备)
        nRet = MV_CC_CreateHandle(&handle_, stDeviceList.pDeviceInfo[0]);
        if (nRet != MV_OK)
        {
            std::cerr << "[ERROR]   创建句柄失败！ Errorcode: " << nRet << std::endl;
            return false;
        }

        // 3 打开设备
        nRet = MV_CC_OpenDevice(handle_);
        if (nRet != MV_OK)
        {
            std::cerr << "[ERROR]   打开设备失败！ Errorcode: " << nRet << std::endl;
            MV_CC_DestroyHandle(handle_);
            handle_ = nullptr;
            return false;
        }

        // 4 开始取流
        nRet = MV_CC_StartGrabbing(handle_);
        if (nRet != MV_OK)
        {
            std::cerr << "[ERROR]   开始取流失败！ Errorcode: " << nRet << std::endl;
            close();
            return false;
        }

        is_open_ = true;
        std::cout << "[INFO]   相机打开成功！" << std::endl;
        return true;
    }

    bool HKCamera::read(cv::Mat &image)
    {
        if (!is_open_ || !handle_)
            return false;

        MV_FRAME_OUT stImageInfo = {0};
        int nRet = MV_CC_GetImageBuffer(handle_, &stImageInfo, 1000);

        if (nRet != MV_OK)
        {
            std::cerr << "[ERROR]   获取图像失败！ Errorcode: " << nRet << std::endl;
            return false;
        }

        int width = stImageInfo.stFrameInfo.nWidth;
        int height = stImageInfo.stFrameInfo.nHeight;
        auto pixel_type = stImageInfo.stFrameInfo.enPixelType;

        bool convert_success = false;

        // 将图像数据转换成 cv::Mat
        if (pixel_type == PixelType_Gvsp_Mono8)
        {
            image = cv::Mat(height, width, CV_8UC1, stImageInfo.pBufAddr).clone();
            convert_success = true;
        }
        else if (pixel_type == PixelType_Gvsp_BayerRG8)
        {
            cv::Mat bayer_mat(height, width, CV_8UC1, stImageInfo.pBufAddr);
            cv::cvtColor(bayer_mat, image, cv::COLOR_BayerRG2RGB);
            convert_success = true;
        }
        else
        {
            std::cerr << "[ERROR]   不支持的像素格式！" << std::endl;
        }

        MV_CC_FreeImageBuffer(handle_, &stImageInfo);

        return convert_success && !image.empty();
    }

    void HKCamera::close()
    {
        if (handle_)
        {
            std::cout << "[INFO]   正在关闭相机！" << std::endl;
            // 依次：停止取流 -> 关闭设备 -> 销毁句柄
            if (is_open_)
            {
                MV_CC_StopGrabbing(handle_);
            }
            MV_CC_CloseDevice(handle_);
            MV_CC_DestroyHandle(handle_);

            handle_ = nullptr;
            is_open_ = false;
        }
    }
}