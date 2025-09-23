cmake_minimum_required(VERSION 3.10)
project(simple_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# 1. 查找所需要的动态库，智能地从默认目录上查找
find_library(MUDUO_NET_LIBRARY NAMES muduo_net REQUIRED) 
find_library(MUDUO_BASE_LIBRARY NAMES muduo_base REQUIRED) 
# 查找必要的库
find_library(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconn mysqlcppconn8
    PATHS /usr/lib /usr/lib64 /usr/local/lib

)

find_library(ONNXRUNTIME_LIBRARY
    NAMES onnxruntime
    PATHS /usr/lib /usr/lib64 /usr/local/lib
)

set(MAIN_SRC "${PROJECT_SOURCE_DIR}/AIApps/ChatServer/src/main.cpp")

# 2. 添加头文件搜索目录
include_directories(
    ${PROJECT_SOURCE_DIR}/HttpServer/include
    ${PROJECT_SOURCE_DIR}/AIApps/ChatServer/include
    /usr/include/mysql-cppconn-8
    /usr/include/mysql
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    /usr/include/opencv4          # OpenCV 头文件路径
    /usr/local/include            # SimpleAmqpClient & ONNX Runtime
)

# 3. 添加所有源文件
file(GLOB_RECURSE HTTP_SERVER_SRC
    "${PROJECT_SOURCE_DIR}/HttpServer/src/*.cpp"
)

# 4. 添加Cache模块源文件
file(GLOB CACHE_SRC
    "${PROJECT_SOURCE_DIR}/HttpServer/src/Cache/*.cpp"
)

# 5. 创建可执行文件
add_executable(ai_server
    ${MAIN_SRC}
    ${HTTP_SERVER_SRC}
    ${CACHE_SRC}
)

# 6. 链接动态库
target_link_libraries(
    ai_server
    pthread
    ${MUDUO_NET_LIBRARY}
    ${MUDUO_BASE_LIBRARY}
    ${MYSQLCPPCONN_LIBRARY}
    mysqlclient
    ssl
    crypto
    ${CURL_LIBRARIES}
    ${OPENCV4_LIBRARIES}
    /usr/lib/x86_64-linux-gnu/libopencv_core.so
    /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so
    /usr/lib/x86_64-linux-gnu/libopencv_highgui.so
    /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so
    /usr/lib/x86_64-linux-gnu/libopencv_videoio.so
    /usr/lib/x86_64-linux-gnu/libopencv_dnn.so

    ${ONNXRUNTIME_LIBRARY}        # 自动查找的 ONNX Runtime 库

    SimpleAmqpClient
    rabbitmq

    
)
# 如果库不在默认路径，添加链接目录
link_directories(/usr/local/lib)

# 打印调试信息
message(STATUS "Include directories:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "  ${dir}")
endforeach()

message(STATUS "ONNX Runtime library: ${ONNXRUNTIME_LIBRARY}")
message(STATUS "Main source file: ${MAIN_SRC}")
message(STATUS "HTTP Server sources: ${HTTP_SERVER_SRC}")
message(STATUS "Gomoku Server sources: ${GOMOKU_SERVER_SRC}")