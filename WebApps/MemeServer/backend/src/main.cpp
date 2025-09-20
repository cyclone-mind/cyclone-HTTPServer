/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:39:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 22:21:42
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/src/main.cpp
 * @Description: 热梗测试服务器主程序
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#include <muduo/base/Logging.h>
#include <iostream>
#include "../include/MemeServer.hpp"
#include <muduo/base/TimeZone.h>

int main(int argc, char* argv[]) {
    constexpr int DEFALULT_PORT = 8080;
    constexpr int TIME_ZONE_MOVE = 8 * 3600;
    // 设置日志系统的时区为中国标准时间（UTC+8）
    // 参数说明：
    // 8 * 3600: 时区偏移量，8小时 * 3600秒/小时 = 28800秒，表示UTC+8时区
    // "CST": 时区名称，China Standard Time（中国标准时间）
    muduo::Logger::setTimeZone(muduo::TimeZone(TIME_ZONE_MOVE, "CST"));
    LOG_INFO << "pid = " << getpid();

    std::string serverName = "MemeServer";
    int port = DEFALULT_PORT;

    // 参数解析：解析命令行参数
    // -p: 指定服务器监听端口号
    int opt;
    const char* str = "p:";

    // getopt函数用于解析命令行参数
    // argc: 命令行参数个数
    // argv: 命令行参数数组
    // str: 选项字符串，"p:"表示选项p需要一个参数
    // 返回值: 当前解析到的选项字符，解析完毕时返回-1
    // 例如命令行输入： ./program -p 9000
    // 第一次调用：getopt 找到 -p 选项，返回字符 'p' 不等于-1 进入循环
    while ((opt = getopt(argc, argv, str)) != -1) {
        switch (opt) {
            case 'p': {
                // atoi 将字符串参数转换为整数作为端口号
                // optarg 是 getopt 提供的全局变量，包含 -p 选项后面的参数值
                port = atoi(optarg);
                break;
            }
            default:
                // 未知参数，忽略处理
                break;
        }
    }
    
    // 设置日志级别
    muduo::Logger::setLogLevel(muduo::Logger::DEBUG);

    try {
        // 创建服务器
        MemeServer server(port, serverName);

        // 设置线程数
        server.setThreadNum(4);

        LOG_INFO << "MemeServer starting on port " << port;

        // 启动服务器
        server.start();

        LOG_INFO << "MemeServer started successfully";

        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "Server error: " << e.what();
        return 1;
    }
}
