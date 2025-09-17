/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:26:05
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-17 21:48:55
 * @FilePath: /cyclone-HTTPServer/WebApps/SimpleServer/backend/include/MemeServer.hpp
 * @Description:
 * Copyright (c) 2025 by ${git_name} email: ${git_email}, All Rights Reserved.
 */
#pragma once
#include <muduo/base/Logging.h>
#include <memory>

#include "../../../../HttpServer/include/CorsMiddleware.hpp"
#include "../../../../HttpServer/include/HttpServer.hpp"
#include "../../../../HttpServer/include/MysqlUtil.hpp"
#include "../../../../HttpServer/include/SessionManager.hpp"
#include "../../../../HttpServer/include/SslConfig.hpp"

class EntryHandler;

class MemeServer {
private:
    http::HttpServer httpServer_;
    http::MysqlUtil mysqlUtil_;

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
    auto getSessionManager() -> http::session::SessionManager* {
        return httpServer_.getSessionManager();
    }

private:
    auto initialize() -> void {
        // 初始化数据库连接池
        http::MysqlUtil::init("tcp://127.0.0.1:3306", "root", "root123", "Meme", 10);
        // 初始化会话
        initializeSession();
        // 初始化中间件
        initializeMiddleware();
        // 初始化路由
        initializeRouter();
        initializeSsl();
    }
    auto initializeMiddleware() -> void {
        // 创建中间件
        auto corsMiddleware = std::make_shared<http::middleware::CorsMiddleware>();
        // 添加中间件
        httpServer_.addMiddleware(corsMiddleware);
    }
    auto initializeRouter() -> void;
    auto initializeSession() -> void {
        auto sessionStorage = std::make_unique<http::session::MemorySessionStorage>();
        auto sessionManager =
            std::make_unique<http::session::SessionManager>(std::move(sessionStorage));
        setSessionManager(std::move(sessionManager));
    }
    void setSessionManager(std::unique_ptr<http::session::SessionManager> manager) {
        httpServer_.setSessionManager(std::move(manager));
    }
    void initializeSsl() {
        httpServer_.enableSSL(true);
        ssl::SslConfig sslConfig;
        const std::string certPath = "/opt/cyclone/ssl";
        std::string certFile = certPath + "/cloudflare-origin-kktui.crt";
        std::string keyFile = certPath + "/cloudflare-origin-kktui.key";
        sslConfig.setCertificateFile(certFile);
        sslConfig.setPrivateKeyFile(keyFile);
        
        if(access(certFile.c_str(), R_OK != 0) != 0){
            LOG_FATAL << "SSL certificate file not found: " << certFile;
        }
        if(access(keyFile.c_str(), R_OK != 0) != 0){
            LOG_FATAL << "SSL key file not found: " << keyFile;
        }

        sslConfig.setProtocolVersion(ssl::SSLVersion::TLS_1_3);
        sslConfig.setCipherList("ECDHE+AESGCM:ECDHE+CHACHA20:DHE+AESGCM:DHE+CHACHA20:!aNULL:!MD5:!DSS");
        
        sslConfig.setVerifyClient(false);
        sslConfig.setVerifyDepth(1);
        
        sslConfig.setSessionTimeout(300); // seconds
        sslConfig.setSessionCacheSize(10240);
        httpServer_.setSslConfig(sslConfig);
    }
};