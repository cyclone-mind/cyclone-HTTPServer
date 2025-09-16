/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:29:23
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 17:38:29
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/src/main.cpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#include <muduo/base/Logging.h>
#include <muduo/base/TimeZone.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <iostream>
#include <string>

#include "../include/MemeServer.hpp"
#define DEFALULT_PORT 8080

int main(int argc, char* argv[]) {
 
    // 设置日志系统的时区为中国标准时间（UTC+8）
    // 参数说明：
    // 8 * 3600: 时区偏移量，8小时 * 3600秒/小时 = 28800秒，表示UTC+8时区
    // "CST": 时区名称，China Standard Time（中国标准时间）
    muduo::Logger::setTimeZone(muduo::TimeZone(8 * 3600, "CST"));
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

    muduo::Logger::setLogLevel(muduo::Logger::WARN);
    muduo::Logger::setLogLevel(muduo::Logger::DEBUG);
    MemeServer server(port, serverName);
    server.setThreadNum(4);
    server.start();
}
