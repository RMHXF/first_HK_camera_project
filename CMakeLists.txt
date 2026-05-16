# 设置 CMake 和 C++ 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目的名称和使用语言
set(CMAKE_GENERATOR "MinGW Makefiles")
project(HK_camera_demo CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 强制生成 compile_commands.json , 完美解决 VScode 代码标红问题
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 寻找并配置第三方库 (openCV)
# 下面这句话很重要，否则找不到opencv
set(OpenCV_DIR "D:/code_env/opencv-4.5.5/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 配置海康相机(MVS) 和相机 (SDK)
if(WIN32)
    set(HK_INCLUDE_DIR "D:/code_env/MVS/MVS/Development/Includes")

    set(HK_LIB_DIR "D:/code_env/MVS/MVS/Development/Libraries/win64")

    set(OpenCV_INCLUDE_DIR "D:/code_env/opencv-4.5.5/include")
    set(OpenCV_LIB_DIR "D:/code_env/opencv-4.5.5/build/lib")

    include_directories(${HK_INCLUDE_DIR})
    link_directories(${HK_LIB_DIR})
    include_directories(${OpenCV_INCLUDE_DIR})
    link_directories(${OpenCV_LIB_DIR})
    set(HK_LIBS MvCameraControl)

elseif(UNIX)
# UNIX 配置
    set(HK_INCLUDE_DIR "/usr/local/include")
    set(HK_LIB_DIR "/usr/local/lib")

    include_directories(${HK_INCLUDE_DIR})
    link_directories(${HK_LIB_DIR})
    set(HK_LIBS MvCameraControl)
endif()

# 告诉编译器去哪里找你自己写的头文件
include_directories(${CMAKE_SOURCE_DIR}/include/core)
include_directories(${CMAKE_SOURCE_DIR}/include/camera)

# 自动查找 src 文件夹下所有的 .cpp 源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    ${OpenCV_LIBS}
    ${HK_LIBS}
)
