/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:28:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:35:17
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/include/handlers/AuthHandler.hpp
 * @Description: 认证处理器
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "../../../../../HttpServer/include/HttpRequest.hpp"
#include "../../../../../HttpServer/include/HttpResponse.hpp"
#include "../../../../../HttpServer/include/MysqlUtil.hpp"
#include "../../../../../HttpServer/include/SessionManager.hpp"
#include "../../../../../HttpServer/include/Session.hpp"
#include "../JsonResponse.hpp"

namespace meme {

class AuthHandler {
private:
    http::session::SessionManager* sessionManager_;

public:
    explicit AuthHandler(http::session::SessionManager* sessionManager)
        : sessionManager_(sessionManager) {}

    // 用户注册
    void handleRegister(const http::HttpRequest& req, http::HttpResponse* resp);

    // 用户登录
    void handleLogin(const http::HttpRequest& req, http::HttpResponse* resp);

    // 用户登出
    void handleLogout(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    // 检查用户ID是否已存在
    bool userExists(const std::string& userId);

    // 创建新用户
    bool createUser(const std::string& userId, const std::string& password,
                   const std::string& nickname = "");

    // 验证用户密码
    bool validateUser(const std::string& userId, const std::string& password);

    // 获取用户信息
    nlohmann::json getUserInfo(const std::string& userId);

    // 获取当前登录用户ID
    std::string getCurrentUserId(const http::HttpRequest& req);
};

} // namespace meme