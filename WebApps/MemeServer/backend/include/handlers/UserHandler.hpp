/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:31:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:31:49
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/include/handlers/UserHandler.hpp
 * @Description: 用户处理器
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "../../../../../HttpServer/include/HttpRequest.hpp"
#include "../../../../../HttpServer/include/HttpResponse.hpp"
#include "../../../../../HttpServer/include/DbConnection.hpp"
#include "../../../../../HttpServer/include/SessionManager.hpp"
#include "../JsonResponse.hpp"

namespace meme {

class UserHandler {
private:
    std::shared_ptr<http::db::DbConnection> db_;
    http::session::SessionManager* sessionManager_;

public:
    UserHandler(std::shared_ptr<http::db::DbConnection> db,
                http::session::SessionManager* sessionManager)
        : db_(db), sessionManager_(sessionManager) {}

    // 获取用户资料
    void handleGetProfile(const http::HttpRequest& req, http::HttpResponse* resp);

    // 获取测试历史
    void handleGetHistory(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    // 获取当前登录用户ID
    std::string getCurrentUserId(const http::HttpRequest& req);

    // 获取用户基本信息
    nlohmann::json getUserProfile(const std::string& userId);

    // 获取用户称号列表
    nlohmann::json getUserTitles(const std::string& userId);

    // 获取用户测试历史
    nlohmann::json getUserHistory(const std::string& userId, int page, int limit);
};

} // namespace meme