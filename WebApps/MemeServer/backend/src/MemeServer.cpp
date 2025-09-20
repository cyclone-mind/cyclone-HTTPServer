/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-16 13:29:16
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 22:58:01
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/src/MemeServer.cpp
 * @Description: 
 * Copyright (c) 2025 by cyclone, All Rights Reserved. 
 */
#include "../include/MemeServer.hpp"
#include "../include/handlers/EntryHandler.hpp"
#include "../include/handlers/TestDbConHandler.hpp"
#include <memory>

auto MemeServer::initializeRouter() -> void {
    // 获取数据库连接用于handler
    auto dbConn = http::db::DbConnectionPool::getInstance().getConnection();

    // 初始化handler
    authHandler_ = std::make_unique<meme::AuthHandler>(getSessionManager());
    testHandler_ = std::make_unique<meme::TestHandler>(dbConn, getSessionManager());
    leaderboardHandler_ = std::make_unique<meme::LeaderboardHandler>(dbConn);
    userHandler_ = std::make_unique<meme::UserHandler>(dbConn, getSessionManager());

    // 热梗测试API路由

    // 认证相关
    httpServer_.Post("/api/auth/register",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            authHandler_->handleRegister(req, resp);
        });

    httpServer_.Post("/api/auth/login",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            authHandler_->handleLogin(req, resp);
        });

    httpServer_.Post("/api/auth/logout",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            authHandler_->handleLogout(req, resp);
        });

    // 测试相关
    httpServer_.Get("/api/test/questions",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            testHandler_->handleGetQuestions(req, resp);
        });

    httpServer_.Post("/api/test/submit",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            testHandler_->handleSubmitTest(req, resp);
        });

    httpServer_.Post("/api/test/restart",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            testHandler_->handleRestartTest(req, resp);
        });

    // 排行榜相关
    httpServer_.Get("/api/leaderboard/total",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            leaderboardHandler_->handleGetTotalLeaderboard(req, resp);
        }, http::cache::CacheConfig(300));

    httpServer_.addRoute(http::HttpMethod::CGet, "/api/leaderboard/category/:category",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            leaderboardHandler_->handleGetCategoryLeaderboard(req, resp);
        });

    // 个人中心相关
    httpServer_.Get("/api/user/profile",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            userHandler_->handleGetProfile(req, resp);
        });

    httpServer_.Get("/api/user/history",
        [this](const http::HttpRequest& req, http::HttpResponse* resp) {
            userHandler_->handleGetHistory(req, resp);
        });

    // 原有路由保持兼容
    httpServer_.Get("/", std::make_shared<EntryHandler>(this));
    httpServer_.Get("/test-db", std::make_shared<TestDbConHandler>(this));
}