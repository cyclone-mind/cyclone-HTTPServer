/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:26:05
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-16 17:38:07
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/include/MemeServer.hpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once
#include "../../../../HttpServer/include/HttpServer.hpp"
#include "../include/handlers/EntryHandler.hpp"
#include "../../../../HttpServer/include/CorsMiddleware.hpp"

class MemeServer {
private:
    http::HttpServer httpServer_;

public:
    MemeServer(int port, const std::string& name,
               muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort)
        : httpServer_(port, name, false, option) {
        initialize();
    }
    auto setThreadNum(int num) {
        httpServer_.setThreadNums(num);
    }
    auto start() -> void {
        httpServer_.start();
    }

private:
    auto initialize() -> void {
        initializeMiddleware();
        initializeRouter();
    }
    auto initializeMiddleware() -> void {
        // 创建中间件
        auto corsMiddleware = std::make_shared<http::middleware::CorsMiddleware>();
        // 添加中间件
        httpServer_.addMiddleware(corsMiddleware);
    }
    auto initializeRouter() -> void {
        httpServer_.Get("/", std::make_shared<EntryHandler>(this));
        httpServer_.Get("/entry", std::make_shared<EntryHandler>(this));
    };
};