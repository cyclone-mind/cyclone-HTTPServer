/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:26:05
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:38:02
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/include/MemeServer.hpp
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

#include "../include/JsonResponse.hpp"
#include "../include/handlers/AuthHandler.hpp"
#include "../include/handlers/TestHandler.hpp"
#include "../include/handlers/LeaderboardHandler.hpp"
#include "../include/handlers/UserHandler.hpp"

class EntryHandler;

class MemeServer {
private:
    http::HttpServer httpServer_;
    http::MysqlUtil mysqlUtil_;

    // 热梗测试相关handler
    std::unique_ptr<meme::AuthHandler> authHandler_;
    std::unique_ptr<meme::TestHandler> testHandler_;
    std::unique_ptr<meme::LeaderboardHandler> leaderboardHandler_;
    std::unique_ptr<meme::UserHandler> userHandler_;

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
        http::MysqlUtil::init("tcp://127.0.0.1:3306", "root", "root123", "MemeTest", 10);
        // 初始化会话
        initializeSession();
        // 初始化中间件
        initializeMiddleware();
        // 初始化路由
        initializeRouter();
        // initializeSsl();
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
        // std::string certFile = certPath + "/cloudflare-origin-kktui.crt";
        // std::string keyFile = certPath + "/cloudflare-origin-kktui.key";
        std::string certFile = certPath + "/kktui.dpdns.org_bundle.crt";
        std::string keyFile = certPath + "/kktui.dpdns.org.key";
        sslConfig.setCertificateFile(certFile);
        sslConfig.setPrivateKeyFile(keyFile);
        

        sslConfig.setProtocolVersion(ssl::SSLVersion::TLS_1_2);
        sslConfig.setCipherList("ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA256:AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-SHA256:AES128-SHA256:HIGH:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!SRP:!CAMELLIA");
        
        sslConfig.setVerifyClient(false);
        sslConfig.setVerifyDepth(1);
        
        sslConfig.setSessionTimeout(300); // seconds
        sslConfig.setSessionCacheSize(10240);
        httpServer_.setSslConfig(sslConfig);
    }
};