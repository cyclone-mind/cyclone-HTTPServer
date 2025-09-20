/*
 * @Author: shouyu zhousy953933@gmail.com
 * @Date: 2025-09-20 21:30:00
 * @LastEditors: shouyu zhousy953933@gmail.com
 * @LastEditTime: 2025-09-20 21:31:07
 * @FilePath: /cyclone-HTTPServer/WebApps/MemeServer/backend/include/handlers/LeaderboardHandler.hpp
 * @Description: 排行榜处理器
 * Copyright (c) 2025 by cyclone, All Rights Reserved.
 */
#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "../../../../../HttpServer/include/HttpRequest.hpp"
#include "../../../../../HttpServer/include/HttpResponse.hpp"
#include "../../../../../HttpServer/include/DbConnection.hpp"
#include "../JsonResponse.hpp"

namespace meme {

class LeaderboardHandler {
private:
    std::shared_ptr<http::db::DbConnection> db_;

public:
    explicit LeaderboardHandler(std::shared_ptr<http::db::DbConnection> db)
        : db_(db) {}

    // 获取总排行榜
    void handleGetTotalLeaderboard(const http::HttpRequest& req, http::HttpResponse* resp);

    // 获取分类排行榜
    void handleGetCategoryLeaderboard(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    // 获取排行榜数据
    nlohmann::json getLeaderboardData(const std::string& category, int page, int limit);

    // 获取用户当前排名
    int getUserRank(const std::string& userId, const std::string& category = "");

    // 获取排行榜总数
    int getLeaderboardTotal(const std::string& category = "");
};

} // namespace meme