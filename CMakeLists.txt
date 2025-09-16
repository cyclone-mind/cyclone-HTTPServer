cmake_minimum_required(VERSION 3.10)
project(simple_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 查找所需要的动态库，智能地从默认目录上查找
find_library(MUDUO_NET_LIBRARY NAMES muduo_net REQUIRED) 
find_library(MUDUO_BASE_LIBRARY NAMES muduo_base REQUIRED) 

# 2. 添加头文件搜索目录
include_directories(
    ${PROJECT_SOURCE_DIR}/HttpServer/include
    ${PROJECT_SOURCE_DIR}/WebApps/SimpleServer/backend/include
)

# 3. 添加所有源文件
file(GLOB_RECURSE HTTP_SERVER_SRC
    "${PROJECT_SOURCE_DIR}/HttpServer/src/*.cpp"
)

file(GLOB_RECURSE SIMEPLE_SERVER_SRC
    "${PROJECT_SOURCE_DIR}/WebApps/SimpleServer/backend/src/*.cpp"
)
# 4. 添加主源文件
set(MAIN_SRC "${PROJECT_SOURCE_DIR}/WebApps/SimpleServer/backend/src/main.cpp")

# 5. 创建可执行文件
add_executable(simple_server 
    ${MAIN_SRC}
    ${HTTP_SERVER_SRC}
    ${SIMEPLE_SERVER_SRC}
)

# 6. 链接动态库
target_link_libraries(
    simple_server
    pthread
    ${MUDUO_NET_LIBRARY}
    ${MUDUO_BASE_LIBRARY}
)